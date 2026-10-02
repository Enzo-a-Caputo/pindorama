#ifndef PINDORAMA_CROPREGISTRY_HPP
#define PINDORAMA_CROPREGISTRY_HPP

#include <cstddef>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include "CropType.hpp"  // Integrante 3

namespace pindorama {

/**
 * @brief What went wrong with one place of the crops CSV.
 */
enum class CsvFailureKind {
    FileNotFound,  ///< The file does not exist or cannot be opened.
    WrongHeader,   ///< The header row does not match the expected column names.
    MalformedRow,  ///< The row has the wrong number of columns.
    InvalidValue,  ///< A value has the wrong type or is out of its allowed range.
    DuplicateId    ///< The id was already defined in an earlier row.
};

/**
 * @brief One failure found while reading the crops CSV.
 */
struct CsvFailure {
    CsvFailureKind kind;  ///< Kind of failure.
    std::string file;     ///< Path of the file.
    int line;             ///< 1-based line number (0 when the whole file is the problem).
    int column;           ///< 1-based column number (0 when the whole line is the problem).
    std::string message;  ///< Human-readable explanation.
};

/**
 * @brief Thrown when the crops CSV cannot be turned into a valid table.
 *
 * Carries every failure found, not only the first one, so the author of the
 * file can fix them all at once.
 */
class CropRegistryError : public std::runtime_error {
public:
    /**
     * @brief Builds the exception.
     * @param failures All failures found.
     * @pre !failures.empty().
     * @post what() summarizes the number of failures and the first one.
     */
    explicit CropRegistryError(const std::vector<CsvFailure>& failures);

    /** @return Every failure found, in file order. */
    const std::vector<CsvFailure>& getFailures() const noexcept;

private:
    std::vector<CsvFailure> failures_;
};

/**
 * @brief Table of crop species loaded from a CSV (CRC card: TabelaDeCultivos).
 *
 * Owns every CropType; Crop only keeps a non-owning pointer to them, so a
 * CropRegistry must outlive every Crop and must never move. For that reason
 * it cannot be copied.
 *
 * Expected CSV: one header row with the names returned by expectedHeader(),
 * then one row per species, comma separated, e.g.
 * `carrot,Carrot,60,120,180,240,3,20,35`.
 */
class CropRegistry {
public:
    /**
     * @brief The column names the CSV header must have, in order. They live
     *        here, in a single place, and the header check uses them.
     * @return `id, display_name, stage1_minutes, stage2_minutes,
     *         stage3_minutes, stage4_minutes, harvest_yield, seed_price,
     *         sell_price`.
     */
    static const std::vector<std::string>& expectedHeader();

    /**
     * @brief Reads and validates the CSV and builds the table.
     *
     * Validation per row: number of columns, integer type and range of every
     * numeric value (the CropType preconditions: positive stage durations and
     * yield, non-negative prices, non-empty id) and duplicate ids. All
     * failures are accumulated before giving up.
     * @param csvPath Path of the CSV file.
     * @pre None.
     * @post Every species in the file is available through getType().
     * @throws CropRegistryError If the file does not exist, if the header is
     *         wrong, or if any row fails validation. The table is never
     *         delivered partially.
     */
    explicit CropRegistry(const std::string& csvPath);

    /** @brief Copying is forbidden: Crop holds pointers into this table. */
    CropRegistry(const CropRegistry&) = delete;
    CropRegistry& operator=(const CropRegistry&) = delete;

    /**
     * @brief Tells whether an id exists.
     * @param id Stable identifier of a species.
     * @return true if a species with that id was loaded.
     */
    bool has(const std::string& id) const;

    /**
     * @brief Gives the species of an id.
     * @param id Stable identifier of a species.
     * @pre has(id).
     * @return The species; the reference stays valid as long as the registry lives.
     * @throws std::out_of_range If the id does not exist.
     */
    const CropType& getType(const std::string& id) const;

    /**
     * @brief Lists every species.
     * @return Pointers to all species, ordered by id; they stay valid as long
     *         as the registry lives.
     */
    std::vector<const CropType*> listAll() const;

    /** @return Number of species in the table (always >= 0). */
    std::size_t size() const noexcept;

private:
    std::map<std::string, CropType> types_;
};

}  // namespace pindorama

#endif  // PINDORAMA_CROPREGISTRY_HPP
