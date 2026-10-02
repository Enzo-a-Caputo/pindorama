#ifndef PINDORAMA_CROP_HPP
#define PINDORAMA_CROP_HPP

#include <memory>
#include <stdexcept>
#include <vector>

#include "CropType.hpp"

namespace pindorama {

class Item; // Integrante 4 (Itens). Only forward-declared: Crop returns
            // produce as Item pointers, but the concrete construction
            // detail is still a pendência to align with that block (see
            // the division doc's "conversa de 20 min antes do header").

/// @brief A single planted crop and its growth/life state.
///
/// Growth accumulates only on ticks during which the tile holding this crop
/// was wet (minutes-based, never a decreasing counter). Fertilization is a
/// property of the soil (Tile), not chosen by Crop itself: once any tick
/// reports the soil as fertilized, the crop remembers it for the rest of
/// its life. Dying does not remove the crop: it becomes a dead,
/// unharvestable state that stays on the tile (sprite "dries") until
/// cleared by Tile::clearCrop() or Tile::harvest().
class Crop {
public:
    /// @brief Consecutive days without water this crop tolerates before
    ///        dying on its own (CRC: "conhecer os dias consecutivos sem
    ///        receber água" / "deixar de existir ao morrer" -- both are
    ///        Crop's own responsibility, not Tile's).
    /// @note Placeholder value; the exact N is still a pendência do grupo.
    static constexpr int kMaxDaysWithoutWater = 1;

    /// @brief Plants a new crop of the given species, at its first growth
    ///        stage.
    /// @param type Species definition. Crop does not take ownership; the
    ///        CropType is expected to outlive the Crop (owned by
    ///        TabelaDeCultivos/CropRegistry).
    /// @pre type != nullptr
    /// @throws std::invalid_argument if type == nullptr.
    explicit Crop(const CropType* type);

    /// @brief Accumulates growth for one tick. The caller (Tile) is
    ///        responsible for only calling this while isAlive(): a dead
    ///        crop must never receive a tick.
    /// @param wasWet Whether the tile holding this crop was wet during this
    ///        tick (decided by Tile, not by Crop).
    /// @param wasFertilized Whether the tile holding this crop was
    ///        fertilized during this tick (decided by Tile).
    /// @param tickMinutes Game-minutes represented by one tick (e.g. 10).
    /// @pre isAlive()
    /// @post getStage()/getGrowthFraction()/isFullyGrown() may advance.
    /// @post isFertilized() becomes (and stays) true if wasFertilized.
    void onTick(bool wasWet, bool wasFertilized, int tickMinutes);

    /// @brief Notified once per new day by the holding Tile.
    /// @param wasWateredToday Whether the tile was watered at least once
    ///        since the previous onNewDay.
    /// @pre isAlive()
    /// @post getDaysWithoutWater() resets to 0 if wasWateredToday,
    ///       otherwise increments; calls die() on itself once it reaches
    ///       kMaxDaysWithoutWater.
    void onNewDay(bool wasWateredToday);

    /// @brief Marks the crop as dead.
    /// @post isAlive() == false. The crop is not removed; it stays on the
    ///       tile until cleared externally (Tile::clearCrop()/harvest()).
    void die();

    /// @brief Generates this crop's harvested produce (CRC: "gerar os
    ///        itens colhidos").
    /// @pre isFullyGrown() && isAlive()
    /// @return getHarvestYield() items, built from the species' data.
    ///         Concrete item type depends on ProdutoColhido (Integrante 4,
    ///         not yet published); returned as the common Item base for
    ///         now, forward-declared only (see the note by the class).
    std::vector<std::unique_ptr<Item>> harvestItems() const;

    /// @brief Current growth stage, in [0, CropType::kStageCount - 1].
    /// @return CropType::getStageForGrowth() for the accumulated growth so
    ///         far; used by GameView to pick the sprite.
    int getStage() const;

    /// @brief Wet game-minutes still needed to reach the next growth stage.
    /// @return 0 once isFullyGrown().
    int getMinutesUntilNextStage() const;

    /// @brief Growth progress as a fraction of total growth needed.
    /// @return A value in [0.0, 1.0]; 1.0 once isFullyGrown().
    double getGrowthFraction() const;

    /// @brief Whether accumulated growth has reached the species'
    ///        requirement (CropType::getGrowthMinutes()).
    bool isFullyGrown() const;

    /// @brief Whether the crop's soil has been fertilized at any point
    ///        during its growth so far.
    bool isFertilized() const;

    /// @brief Whether the crop is alive (has not died).
    bool isAlive() const;

    /// @brief Consecutive days without water so far (resets to 0 on
    ///        onNewDay(true)).
    int getDaysWithoutWater() const;

    /// @brief Quantity of produce this crop yields if harvested now.
    /// @pre isFullyGrown() && isAlive()
    /// @return CropType::getHarvestYield(), increased if isFertilized()
    ///         (exact bonus still a pendência do grupo).
    int getHarvestYield() const;

    /// @brief Species this crop belongs to.
    const CropType& getType() const;

private:
    const CropType* type_;
    int accumulatedMinutes_ = 0;
    int daysWithoutWater_ = 0;
    bool alive_ = true;
    bool fertilized_ = false;
};

} // namespace pindorama

#endif // PINDORAMA_CROP_HPP
