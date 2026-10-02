#ifndef PINDORAMA_SAVEMANAGER_HPP
#define PINDORAMA_SAVEMANAGER_HPP

#include <cstddef>
#include <string>
#include <vector>

#include "ErrorLogEntry.hpp"

namespace pindorama {

class Game;  // Forward declaration: Game owns a SaveManager and SaveManager
             // reads/restores the Game, so including Game.hpp here would be
             // a circular include. SaveManager.cpp includes Game.hpp.

/**
 * @brief Saves and loads the game and keeps the save-failure log
 *        (CRC card: GerenciadorDeSave).
 *
 * Save file: plain text. The first line is `PINDORAMA_SAVE <version>` (see
 * kFormatVersion). Of the clock it stores only the GameMinutes instant,
 * never the accumulator of real seconds. The rest holds the player, the
 * inventory, the state of every tile (and its crop, by species id), the
 * shipping bin, the pending deliveries and the current day.
 *
 * Log: when a save fails, one ErrorLogEntry is appended to the current log
 * file inside the log folder. When the current log is larger than the size
 * limit and a new error occurs, it is archived (renamed with the date and
 * time of the archiving) and a new file is started.
 *
 * Saving never throws because of input/output problems: it returns false,
 * logs the failure and lets the Game decide what to do (continue and warn the
 * player, retry, ...).
 */
class SaveManager {
public:
    /** @brief Version written in the first line of every save file. */
    static constexpr int kFormatVersion = 1;

    /** @brief Default size limit of the current log file: 5 MB. */
    static constexpr std::size_t kDefaultMaxLogBytes = 5u * 1024u * 1024u;

    /**
     * @brief Builds a manager. Nothing is created on disk until the first
     *        save or log write.
     * @param savePath Path of the save file.
     * @param logDirectory Folder where the log files live.
     * @param maxLogBytes Size above which the current log is archived when a
     *        new error occurs. Configurable so tests do not need to write 5 MB.
     * @pre !savePath.empty() && !logDirectory.empty() && maxLogBytes > 0.
     * @throws std::invalid_argument If any precondition is violated.
     */
    SaveManager(const std::string& savePath,
                const std::string& logDirectory,
                std::size_t maxLogBytes = kDefaultMaxLogBytes);

    /** @return Path of the save file. */
    const std::string& getSavePath() const noexcept;

    /** @return Folder of the log files. */
    const std::string& getLogDirectory() const noexcept;

    /** @return Size limit of the current log file, in bytes. */
    std::size_t getMaxLogBytes() const noexcept;

    /**
     * @brief Tells whether a save file exists, so the Game can choose between
     *        a new game and loading.
     * @return true if the save file exists and can be opened for reading.
     */
    bool hasSave() const;

    /**
     * @brief Writes the current state of the game to the save file.
     *
     * The new save is written to a temporary file and only then replaces the
     * previous one, so a failure never destroys the last valid save.
     * @param game Game to read (const access only).
     * @return true on success. On failure returns false, appends an
     *         ErrorLogEntry to the log and records the cause (see
     *         getLastErrorType()).
     * @post On success: no log entry is added.
     * @post On failure: the previous save file, if any, is intact.
     */
    bool save(const Game& game);

    /**
     * @brief Tells whether the last call to save() failed.
     * @return false if save() was never called or the last call succeeded.
     */
    bool didLastSaveFail() const noexcept;

    /**
     * @brief Cause of the last failed save.
     * @pre didLastSaveFail().
     * @return The error type that was logged.
     */
    ErrorType getLastErrorType() const noexcept;

    /**
     * @brief Restores the state saved in the file into a game.
     * @param game Game to restore; it is left unchanged if loading fails.
     * @pre hasSave().
     * @throws std::runtime_error If the file cannot be read, if the first line
     *         is not `PINDORAMA_SAVE <version>`, if the version is not
     *         kFormatVersion, if a line is malformed, or if it refers to a
     *         crop species that is not in the game's CropRegistry.
     */
    void load(Game& game) const;

    /**
     * @brief Appends one record to the current log file, archiving the
     *        current file first if it is larger than getMaxLogBytes().
     *
     * Creates the log folder if it does not exist. Used by save() on every
     * failure and public so it can be tested with fixed times.
     * @param entry Record to append.
     * @return true if the record was written; false if the log itself could
     *         not be written (the error is then dropped, never thrown).
     * @post If true: the record is the last line of the current log file.
     */
    bool logError(const ErrorLogEntry& entry);

    /**
     * @brief Reads the failure history, archived logs included.
     * @return Every record found in the log folder, oldest first. Lines that
     *         cannot be parsed are skipped. Empty if there is no log.
     */
    std::vector<ErrorLogEntry> readErrorHistory() const;

private:
    std::string savePath_;
    std::string logDirectory_;
    std::size_t maxLogBytes_;
    bool lastSaveFailed_ = false;
    ErrorType lastErrorType_ = ErrorType::Unknown;
};

}  // namespace pindorama

#endif  // PINDORAMA_SAVEMANAGER_HPP
