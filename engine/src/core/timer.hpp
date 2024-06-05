#pragma once

#include "../mtpch.hpp"

/**
 * @author github.com/R1ssanen
 */

namespace mt {

    using nanoseconds  = std::chrono::nanoseconds;
    using microseconds = std::chrono::microseconds;
    using milliseconds = std::chrono::milliseconds;
    using seconds      = std::chrono::seconds;

    class Timer {
      private:

        static inline constexpr f64 SecondsRatio      = 1.0 / 1000000000.0,
                                    MillisecondsRatio = 1.0 / 1000000.0,
                                    MicrosecondsRatio = 1.0 / 1000.0;

      public:

        /**
         * @brief Get the total elapsed time.
         * @tparam Unit Unit wanted
         * @return double
         */
        template <typename Unit = milliseconds> f64 GetTimeElapsed() const {
            f64 ElapsedNs = m.TimeElapsed.count();

            if (std::is_same<Unit, seconds>::value) {
                return ElapsedNs * SecondsRatio;
            } else if (std::is_same<Unit, milliseconds>::value) {
                return ElapsedNs * MillisecondsRatio;
            } else if (std::is_same<Unit, microseconds>::value) {
                return ElapsedNs * MicrosecondsRatio;
            } else {
                return ElapsedNs;
            }
        }

        /**
         * @brief Get time taken since the last call to Timer::Tick().
         * @tparam Unit Unit wanted
         * @return double
         */
        template <typename Unit = milliseconds> f64 GetFrametime() const {
            double FrametimeNs = m.Frametime.count();

            if (std::is_same<Unit, seconds>::value) {
                return FrametimeNs * SecondsRatio;
            } else if (std::is_same<Unit, milliseconds>::value) {
                return FrametimeNs * MillisecondsRatio;
            } else if (std::is_same<Unit, microseconds>::value) {
                return FrametimeNs * MicrosecondsRatio;
            } else {
                return FrametimeNs;
            }
        }

        /**
         * @brief Get total times the clock has ticked.
         * @return const u64&
         */
        const u64& GetTicks() const noexcept;

        /**
         * @brief Update the timer.
         */
        void Tick() noexcept;

        explicit Timer(const Timer& Other);
        explicit Timer(Timer&& Other);
        Timer();
        ~Timer() = default;

      private:

        using timepoint_t = std::chrono::high_resolution_clock::time_point;

        struct _M {
            timepoint_t TimerStart, FrameStart;
            nanoseconds TimeElapsed, Frametime;
            u64         Ticks;
        } m;
    };

} // namespace mt
