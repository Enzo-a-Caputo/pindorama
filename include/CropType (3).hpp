#ifndef PINDORAMA_CROPTYPE_HPP
#define PINDORAMA_CROPTYPE_HPP

#include <array>
#include <stdexcept>
#include <string>

namespace pindorama {

/// @brief Immutable data describing one species of crop (Type Object pattern).
///
/// Stores only the static parameters shared by every Crop instance of a
/// given species. Per-instance state (growth progress, life/death,
/// fertilization) lives in Crop, never here. Instances are expected to be
/// owned and looked up by TabelaDeCultivos/CropRegistry (Integrante 6);
/// Crop only ever holds a non-owning pointer.
class CropType {
public:
    /// @brief Number of visual growth stages every crop goes through
    ///        (seedling to fully grown), fixed by the game design (CRC:
    ///        "conhecer a duração de cada um dos 4 estágios").
    static constexpr int kStageCount = 4;

    /// @brief Builds a crop species definition.
    /// @param id Stable identifier used as the lookup key in
    ///        TabelaDeCultivos/CropRegistry and in save files. Never shown
    ///        to the player, never changes.
    /// @param displayName Name shown to the player in menus and the HUD.
    /// @param stageDurations Wet game-minutes each of the kStageCount
    ///        stages takes on its own (not cumulative); every entry must
    ///        be > 0.
    /// @param harvestYield Quantity of produce obtained from one harvest.
    /// @param seedPrice Price of one seed of this species (needed by the
    ///        shop / CentralDeEncomendas, Integrante 5).
    /// @param sellPrice Base price paid per harvested unit when sold
    ///        (needed by Integrante 4 to value the resulting
    ///        ProdutoColhido item).
    /// @pre !id.empty()
    /// @pre harvestYield > 0
    /// @pre seedPrice >= 0 && sellPrice >= 0
    /// @pre every entry in stageDurations is > 0
    /// @throws std::invalid_argument if any precondition above is violated.
    CropType(std::string id,
             std::string displayName,
             std::array<int, kStageCount> stageDurations,
             int harvestYield,
             int seedPrice,
             int sellPrice);

    /// @brief Stable identifier, used for TabelaDeCultivos lookup and save
    ///        files.
    const std::string& getId() const;

    /// @brief Name shown to the player (menus, HUD).
    const std::string& getDisplayName() const;

    /// @brief Wet game-minutes the given stage takes on its own (not
    ///        cumulative).
    /// @param stage Index in [0, kStageCount).
    /// @pre stage >= 0 && stage < kStageCount
    int getStageDuration(int stage) const;

    /// @brief The growth stage that corresponds to the given accumulated
    ///        (wet) growth (CRC: "informar o estágio correspondente a um
    ///        crescimento acumulado").
    /// @param accumulatedMinutes Total wet game-minutes accumulated so far.
    /// @return The highest stage index whose cumulative duration has been
    ///         reached, clamped to [0, kStageCount - 1].
    int getStageForGrowth(int accumulatedMinutes) const;

    /// @brief Wet game-minutes still needed, from the given accumulated
    ///        growth, to reach the next stage.
    /// @param accumulatedMinutes Total wet game-minutes accumulated so far.
    /// @return 0 once accumulatedMinutes reaches the last stage.
    int getMinutesUntilNextStage(int accumulatedMinutes) const;

    /// @brief Total wet game-minutes needed to be fully grown (sum of
    ///        every stage's duration).
    int getGrowthMinutes() const;

    /// @brief Quantity of produce obtained from one harvest of this species.
    int getHarvestYield() const;

    /// @brief Price of one seed of this species.
    int getSeedPrice() const;

    /// @brief Base sale price of one harvested unit of this species.
    int getSellPrice() const;

private:
    std::string id_;
    std::string displayName_;
    std::array<int, kStageCount> stageDurations_;
    int harvestYield_;
    int seedPrice_;
    int sellPrice_;
};

} // namespace pindorama

#endif // PINDORAMA_CROPTYPE_HPP
