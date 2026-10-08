#ifndef PINDORAMA_TIMEOBSERVER_HPP
#define PINDORAMA_TIMEOBSERVER_HPP

namespace pindorama{
    /**
     * @brief Minutes since game started. Uses a normal int 32bits, so it can comport ~4000 game years.
     * 
     * (signed: difference between instants can be negative)
     */
    using GameMinutes = int;

    /**
     * @brief Constant variable of how many game minutes is represented by one tick.
     */
    const GameMinutes kTickMinutes = 10;

    /**
     * @brief Receives time notifications from GameClock.
     * 
     * Implemented by anyone that reacts to game minutes.
     * GameClock does not own its observers.
     */
    class TimeObserver{
    public:
        /**
         * @brief Called by GameClock once per tick (every 10 game minutes).
         *
         * Default implementation does nothing: override only if the observer
         * needs to react to time passing within a day (ShippingBin does not).
         * @param now Current instant, already advanced by this tick.
         */
        virtual void onTick(GameMinutes now) {(void)now;}

        /**
         * @brief Called by GameClock once when a new day begins, at 06:00.
         *
         * Every observer must implement it.
         * @param dayStart Instant at which the new day begins; always a 06:00
         *                 (dayStart % 1440 == 360).
         * @pre The 06:00 onTick has been delivered when this method is called.
         */
        virtual void onNewDay(GameMinutes dayStart) = 0;

        /**
         * @brief Virtual so that deleting a derived observer through a
         *        TimeObserver* runs the derived destructor.
         */
        virtual ~TimeObserver() {}
    };
}
#endif