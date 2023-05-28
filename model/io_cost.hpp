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
// Created by Masoud Gholami on 02.02.22.
//

#ifndef EVALUATION_IO_COST_HPP
#define EVALUATION_IO_COST_HPP

#include <vector>
#include <ranges>
#include <algorithm>
#include "../measurement/system_env.hpp"

namespace model {

    class io_cost {
    private:
        const measurement::system_env &sys;
        const long setpoint;

        struct io_info {
            long size;
            double endtime;
        };

        struct data_block {
            int fd;
            long offset;
            long size;
            double io_finish_time;
            bool active;
            bool evict {false};
        };

        std::vector <io_info> io_list {};

        long active_pages {};
        std::vector <data_block> page_cache {};



        long id_clean {};
        double time {};
        double io_time {};
        double bw_avg {};

        long pending {};
        double pending_delay {};

        [[nodiscard]] inline bool exist_expired_pages () const noexcept {
            return (!io_list.empty ()) && (io_list.at (id_clean).endtime < time - sys.dirty_expire);
        }

        [[nodiscard]] inline bool exist_expired_pages_complete () const noexcept {

            if (page_cache.empty())
                return false;

            bool expired = std::any_of (page_cache.cbegin(),
                         page_cache.cend(),
                         [this] (const auto &dblock) {
                return dblock.io_finish_time < time - sys.dirty_expire;
            });

            if (expired)
                return true;

            return false;
        }

        [[nodiscard]] inline constexpr double get_pos_ratio () const noexcept {
            double val = static_cast <double> (setpoint - dirty) / static_cast <double> (sys.limit_hard - setpoint);
            return 1.0 + val * val * val;
        }

        void background_flush (double interval);
        void background_flush_complete (double interval);
        void balance_active_inactive ();


        void place_data_block_in_cache (const data_block &dblock);

    public:
        long dirty {};

        explicit io_cost (const measurement::system_env &env) : sys {env},
                                                                setpoint {(env.limit_bg + env.limit_hard) / 2} {}


        double syscall_io_cost (long size, double delay);

        double syscall_io_cost_complete (double delay, int fd, long offset, long size);

        double library_io_cost (long size, double delay);

        inline constexpr double sync_io_cost (long size, bool is_rnd) noexcept {
            long rem = size % sys.pagesize;
            long fit = size - rem;
            auto dbs = static_cast <double> (sys.bs);
            double penalty = (dbs / sys.bw_rdev + dbs / sys.bw_dev) * (rem > 0);
            double cost = sys.sc_sw + is_rnd * sys.sc_sk + static_cast <double> (size) / sys.bw_ramdisk +
                          static_cast <double> (fit) / sys.bw_dev;
            return cost + penalty;
        }

        inline double direct_io_cost (long size, bool is_rnd) noexcept {
            return sys.sc_sw + is_rnd * sys.sc_sk + static_cast <double> (size) / sys.bw_dev;
        }

    };
}

#endif //EVALUATION_IO_COST_HPP
