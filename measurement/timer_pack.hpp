// Copyright 2023 Zuse Institute Berlin
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.


 //
// Created by Masoud Gholami on 01.02.22.
//

#ifndef EVALUATION_TIMER_PACK_HPP
#define EVALUATION_TIMER_PACK_HPP

#include <array>
#include <chrono>

namespace measurement {

    template <int ntimers = 1>
    class timer_pack {
    private:
        std::array <std::chrono::time_point <std::chrono::steady_clock>, ntimers> timers {};
        std::array <std::chrono::duration <double>, ntimers> total_durations {};
        std::array <std::chrono::duration <double>, ntimers> durations {};
        std::array <long, ntimers> counts {};
    public:

        constexpr inline void start (int id) noexcept {
            timers.at (id) = std::chrono::steady_clock::now ();
        }

        constexpr inline void stop (int id) noexcept {
            auto now = std::chrono::steady_clock::now ();
            durations.at (id) = now - timers.at (id);
            total_durations.at (id) += durations.at (id);
            counts.at (id)++;
        }

        constexpr inline double total_duration (int id) {
            return total_durations.at (id).count ();
        }

        constexpr inline double avg_duration (int id) {
            return total_durations.at (id).count () / counts.at (id);
        }

        constexpr inline double duration (int id) {
            return durations.at (id).count ();
        }

        constexpr inline double live_duration (int id) {
            return std::chrono::duration <double> (std::chrono::steady_clock::now () - timers.at (id)).count();
        }

        constexpr inline long count (int id) {
            return counts.at (id);
        }

        constexpr inline void reset (int id) {
            counts.at (id) = 0;
            durations.at (id) = {};
            total_durations.at (id) = {};
            timers.at (id) = {};
        }

    };
}

#endif //EVALUATION_TIMER_PACK_HPP
