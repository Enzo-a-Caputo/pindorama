#ifndef PINDORAMA_GAMECLOCK_HPP
#define PINDORAMA_GAMECLOCK_HPP

#include <vector>
#include "TimeObserver.hpp"

namespace pindorama {

/**
 * @brief Game time: advances in 10-minute ticks and notifies its observers.
 *
 * Receives real elapsed seconds from outside (never reads raylib), so it can be
 * tested without a window. Does not own its observers.
 */
class GameClock {
public:
    /**
     * @brief Creates the clock at a given instant.
     * @param start Initial instant (minutes since the game started).
     */
    explicit GameClock(GameMinutes start);

    /**
     * @brief Advances game time by an amount of real time.
     * @param realSeconds Real seconds elapsed since the last call.
     * @pre realSeconds >= 0
     * @post Every tick crossed was notified, in order; at 06:00 onTick comes
     *       before onNewDay.
     */
    void advance(double realSeconds);

    /**
     * @brief Jumps straight to 06:00 of the next day (player sleeps).
     * @note Minimum hour to sleep, the 02:00 rule and autosave are still open.
     */
    void skipToNextDay();

    /**
     * @brief Registers an observer to receive onTick and onNewDay.
     * @param observer Observer to notify.
     * @pre observer is not null and outlives this clock.
     */
    void addObserver(TimeObserver* observer);

    /** @brief Current instant. */
    GameMinutes now() const;
    /** @brief Current day, starting at 1. */
    int day() const;
    /** @brief Current hour, 0 to 23. */
    int hour() const;
    /** @brief Current minute, 0 to 59. */
    int minute() const;

private:
    GameMinutes now_;
    double accumulator_;                     ///< Real seconds not yet turned into a tick.
    std::vector<TimeObserver*> observers_;   ///< Not owned.
};

} // namespace pindorama

#endif // PINDORAMA_GAMECLOCK_HPP