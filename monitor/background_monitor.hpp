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

#ifndef EVALUATION_BACKGROUND_MONITOR_HPP
#define EVALUATION_BACKGROUND_MONITOR_HPP

#include <thread>
#include <condition_variable>
#include <atomic>

#include "perf_event_monitor.hpp"

namespace monitor {

    class background_monitor {

        std::thread thr;
        std::condition_variable wake_up;
        std::mutex mtx;

        const std::chrono::duration <double> monitor_timeout;
        enum class cmds {
            non, abort, stop_monitor, start_monitor
        };
        std::atomic <cmds> cmd = cmds::non;

        // monitors
        perf_event_monitor perf_event_mon {};

        void monitor () {
            for (;;) {
                auto local_cmd = cmd.exchange (cmds::non, std::memory_order_acquire);

                switch (local_cmd) {
                    case cmds::start_monitor: {
                        std::unique_lock <std::mutex> lk (mtx);

                        perf_event_mon.start_monitor ();

                        auto now = std::chrono::steady_clock::now ();
                        wake_up.wait_until (lk, now + monitor_timeout,
                                            [this] () { return cmd.load (std::memory_order_relaxed) != cmds::non; });
                        perf_event_mon.stop_monitor ();

                        break;
                    }
                    case cmds::non: {
                        std::unique_lock <std::mutex> lk (mtx);
                        wake_up.wait (lk, [this] () { return cmd.load (std::memory_order_relaxed) != cmds::non; });
                        break;
                    }
                    case cmds::stop_monitor:
                        break;
                    case cmds::abort:
                        return;
                }
            }
        }

    public:
        explicit background_monitor (double timeout = 20) :
                thr (&background_monitor::monitor, this),
                monitor_timeout {std::chrono::duration <double> (timeout)} {}


        inline void start_monitor () noexcept {
            cmd.store (cmds::start_monitor, std::memory_order_release);
            wake_up.notify_one ();
        }

        inline void stop_monitor () noexcept {
            wake_up.notify_one ();
        }

        [[nodiscard]] inline perf_event_monitor::data_t get_perf_event_data () const noexcept {
            return perf_event_mon.data;
        }

        ~background_monitor () {
            cmd.store (cmds::abort, std::memory_order_release);
            wake_up.notify_one ();
            thr.join ();
        }
    };

}

#endif //EVALUATION_BACKGROUND_MONITOR_HPP
