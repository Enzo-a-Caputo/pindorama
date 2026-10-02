#ifndef PINDORAMA_ERRORLOGENTRY_HPP
#define PINDORAMA_ERRORLOGENTRY_HPP

#include <ctime>
#include <stdexcept>
#include <string>

namespace pindorama {

/**
 * @brief Kind of failure that can happen while saving the game.
 */
enum class ErrorType {
    FileOpenFailed,       ///< The save file or its folder could not be created or opened.
    WriteFailed,          ///< The stream failed while writing (e.g. disk full).
    ReplaceFailed,        ///< The new save could not replace the previous one.
    SerializationFailed,  ///< The game state could not be converted to the save format.
    Unknown               ///< Any other failure.
};

/**
 * @brief One record of the error log (CRC card: LogDeErro).
 *
 * Describes a single failure: when it happened and what kind it was. It is
 * not the log itself; the SaveManager owns the log files and writes, reads
 * and archives these records.
 *
 * Text form (one line per record, used in the log file):
 * `YYYY-MM-DD HH:MM:SS;TYPE;message`, with the time in UTC and TYPE being
 * the name returned by typeToString().
 */
class ErrorLogEntry {
public:
    /**
     * @brief Builds a record.
     * @param when Real (wall-clock) time at which the error occurred; it is
     *        passed in so tests can use fixed times.
     * @param type Kind of the error.
     * @param message Optional free text with details. It must not contain
     *        line breaks; any '\n' or '\r' is replaced by a space.
     * @post getTime() == when, getType() == type.
     */
    ErrorLogEntry(std::time_t when, ErrorType type, const std::string& message = "");

    /** @return Real time at which the error occurred. */
    std::time_t getTime() const noexcept;

    /** @return Kind of the error. */
    ErrorType getType() const noexcept;

    /** @return Details of the error (may be empty). */
    const std::string& getMessage() const noexcept;

    /**
     * @brief Text form of the record, ready to be appended to the log file.
     * @return One line, without a trailing line break.
     */
    std::string toLine() const;

    /**
     * @brief Rebuilds a record from its text form.
     * @param line A line produced by toLine().
     * @return The record.
     * @throws std::invalid_argument If the line does not have the form
     *         `YYYY-MM-DD HH:MM:SS;TYPE;message`, if the date is invalid or if
     *         TYPE is unknown.
     */
    static ErrorLogEntry fromLine(const std::string& line);

    /**
     * @brief Stable name of an error type, as written in the log file.
     * @param type Error type.
     * @return For example "FileOpenFailed".
     */
    static std::string typeToString(ErrorType type);

    /**
     * @brief Chronological order: earlier errors come first.
     * @param other Record to compare with.
     * @return true if this record happened strictly before @p other.
     */
    bool operator<(const ErrorLogEntry& other) const noexcept;

private:
    std::time_t time_;
    ErrorType type_;
    std::string message_;
};

}  // namespace pindorama

#endif  // PINDORAMA_ERRORLOGENTRY_HPP
