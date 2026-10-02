#ifndef PINDORAMA_TILE_HPP
#define PINDORAMA_TILE_HPP

#include <memory>
#include <vector>

#include "Crop.hpp"
#include "TimeObserver.hpp" // GameMinutes

namespace pindorama {

/// @brief What kind of ground a Tile is (CRC: "conhecer sua posição e tipo
///        (solo ou não solo)"). Only Soil tiles can be plowed, watered,
///        fertilized or planted.
enum class TileKind {
    Soil,
    NonSoil
};

/// @brief Coarse soil-visual state a Tile reports for GameView to pick a
///        sprite (CRC: "informar a aparência correspondente ao seu
///        estado"). When the state is Planted, GameView additionally reads
///        getCrop().getStage()/isAlive() to pick the plant's own sprite.
/// @note Exact states/combinations still a pendência to align with
///       whoever implements GameView (Integrante 1/2).
enum class TileAppearance {
    NonSoil,
    CommonDry,
    CommonWet,
    PlowedDry,
    PlowedWet,
    Planted
};

/// @brief One tile of farmable ground: soil state (plowed, fertilized, wet)
///        and an optional planted crop.
///
/// Tile holds state only, never a sprite -- the GameView chooses the image
/// from that state (see getAppearance()). It does not register itself with
/// the clock: TileMap is the TimeObserver and relays onTick/onNewDay to
/// each Tile directly (the tiles don't register themselves).
class Tile {
public:
    /// @brief Consecutive days with no crop planted before plowed soil
    ///        reverts to common (unplowed) soil (CRC: "ser arado e voltar
    ///        a solo comum se ficar sem semente por [N] dias").
    /// @note Placeholder value; the exact N is still a pendência do grupo.
    static constexpr int kMaxDaysWithoutSeed = 3;

    /// @brief Builds a tile at the given grid coordinates.
    /// @param row Row in the farm grid (Fazenda/TileMap).
    /// @param col Column in the farm grid.
    /// @param kind Soil or non-soil; a non-soil tile stays in its default
    ///        (dry, unplowed, empty) state for its whole lifetime.
    Tile(int row, int col, TileKind kind = TileKind::Soil);

    /// @brief Row in the farm grid.
    int getRow() const;

    /// @brief Column in the farm grid.
    int getCol() const;

    /// @brief Whether this is a farmable soil tile (as opposed to e.g. a
    ///        path or water tile).
    bool isSoil() const;

    /// @brief Plows the tile, allowing a crop to be planted.
    /// @pre isSoil()
    /// @post isPlowed() == true.
    void plow();

    /// @brief Whether the tile has been plowed (arado).
    bool isPlowed() const;

    /// @brief Applies fertilizer to the soil. Affects whatever crop is
    ///        planted here while fertilized (see Crop::isFertilized()).
    /// @pre isSoil()
    /// @post isFertilized() == true.
    void fertilize();

    /// @brief Whether the soil is currently fertilized.
    bool isFertilized() const;

    /// @brief Waters the tile, keeping it wet until the given instant.
    /// @param wetUntil Absolute game-time instant up to which the tile
    ///        stays wet (never a decreasing counter). The CRC's "[N]
    ///        horas" drying duration is still a pendência do grupo; until
    ///        then the caller computes wetUntil from whatever N gets
    ///        agreed on.
    /// @pre isSoil()
    /// @return true if the water reaches a crop that can use it (no crop
    ///         planted, or a living one); false if a dead crop occupies the
    ///         tile, in which case the soil still becomes wet but watering
    ///         has no further effect (the story says nothing happens to a
    ///         dead plant).
    /// @post isWet(now) is true for any now <= wetUntil.
    /// @post The tile counts as watered for the current day (reflected the
    ///       next time onNewDay forwards to the crop).
    bool water(GameMinutes wetUntil);

    /// @brief Whether the tile is wet at the given instant.
    /// @param now Current game-time instant.
    bool isWet(GameMinutes now) const;

    /// @brief Wet game-minutes remaining before the tile dries.
    /// @param now Current game-time instant.
    /// @return 0 if already dry (isWet(now) == false).
    int getMinutesUntilDry(GameMinutes now) const;

    /// @brief Advances the tile and its crop by one tick.
    /// @param now Current game-time instant, already advanced by the tick.
    /// @param tickMinutes Game-minutes represented by one tick (e.g. 10).
    /// @post If hasCrop() && getCrop().isAlive(), forwards to
    ///       Crop::onTick(isWet(now), isFertilized(), tickMinutes). A dead
    ///       crop never receives a tick (respects its precondition).
    void onTick(GameMinutes now, int tickMinutes);

    /// @brief Notified once per new day (called by TileMap on the
    ///        GameClock's onNewDay event).
    /// @param now Instant of the new day.
    /// @post If hasCrop() && getCrop().isAlive(), forwards
    ///       Crop::onNewDay(wasWateredSinceLastNewDay) -- the crop itself
    ///       decides whether to die (see Crop::kMaxDaysWithoutWater).
    /// @post Days-without-seed: increments unless hasCrop() (then resets to
    ///       0); once it reaches kMaxDaysWithoutSeed, isPlowed() becomes
    ///       false (the tile reverts to common soil).
    /// @post The watered-since-last-new-day flag resets for the new day.
    void onNewDay(GameMinutes now);

    /// @brief Receives a seed and creates the crop at its first growth
    ///        stage (CRC: "receber uma semente e criar a planta no
    ///        primeiro estágio").
    /// @param type Species to plant, identified beforehand from the seed
    ///        being used (see Item::getType() for Semente, Integrante 4).
    /// @pre isSoil() && isPlowed() && !hasCrop()
    /// @return true if the crop was created; false if the tile is not
    ///         soil, not plowed, or already has a crop (nothing changes).
    /// @post On success: hasCrop() == true, getCrop().getStage() == 0, and
    ///       the days-without-seed counter resets to 0.
    bool plant(const CropType& type);

    /// @brief Harvests the fully grown crop: generates its produce items,
    ///        removes it from the tile and reverts the tile to common
    ///        (unplowed) soil (CRC: "entregar a planta madura na colheita
    ///        e voltar a solo comum").
    /// @pre hasCrop() && getCrop().isAlive() && getCrop().isFullyGrown()
    /// @return The produce items generated by the crop (see
    ///         Crop::harvestItems()), ready to be deposited into
    ///         Inventario by the caller (Game).
    /// @post !hasCrop() && !isPlowed().
    std::vector<std::unique_ptr<Item>> harvest();

    /// @brief Removes whatever crop is on the tile, alive or dead (e.g. a
    ///        hoe action). Unlike harvest(), does not revert the plowed
    ///        state or generate items.
    /// @post !hasCrop()
    void clearCrop();

    /// @brief Whether a crop (alive or dead) currently occupies this tile.
    bool hasCrop() const;

    /// @brief The planted crop.
    /// @pre hasCrop()
    Crop& getCrop();
    /// @brief The planted crop (const overload).
    /// @pre hasCrop()
    const Crop& getCrop() const;

    /// @brief Coarse visual state for GameView to pick a sprite.
    TileAppearance getAppearance() const;

private:
    int row_;
    int col_;
    TileKind kind_;
    bool plowed_ = false;
    bool fertilized_ = false;
    GameMinutes wetUntil_{};
    bool wateredSinceLastNewDay_ = false;
    int daysWithoutSeed_ = 0;
    std::unique_ptr<Crop> crop_;
};

} // namespace pindorama

#endif // PINDORAMA_TILE_HPP
