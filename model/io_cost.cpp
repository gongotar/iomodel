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

#include "io_cost.hpp"


double model::io_cost::syscall_io_cost (long size, double delay) {
    time += delay;
    double taskrate = sys.bw_ramdisk;
    background_flush (delay);
    bool exp = exist_expired_pages ();
    bool async_run = (dirty < setpoint) && (dirty >= sys.limit_bg || exp);
    bool throttle_run = dirty >= setpoint;
    if (async_run) {
        taskrate = sys.bw_ramdisk * sys.coeff_bg;
    }
    else if (throttle_run) {
        double pos_ratio = get_pos_ratio ();
        taskrate = bw_avg * pos_ratio;
        taskrate = taskrate * sys.bw_ramdisk * sys.coeff_bg /
                   (sys.bw_ramdisk * sys.coeff_bg + taskrate * (1 - sys.coeff_bg));
    }
    double cost = static_cast <double> (size) / taskrate + sys.sc_w;
    dirty += size;
    io_list.push_back ({size, time + cost});
    bw_avg = (bw_avg * io_time + size) / (io_time + cost);
    time += cost;
    io_time += cost;
    background_flush (cost);
    return cost;
}

double model::io_cost::library_io_cost (long size, double delay) {
    double cost = sys.lib_metacost;
    if (size <= sys.bf - pending) {
        cost += static_cast <double> (size) / sys.bw_mem;
        pending += size;
        pending_delay += delay + cost;
    }
    else {
        cost += static_cast <double> (sys.bf - pending) / sys.bw_mem;
        pending_delay += delay + cost;
        cost += syscall_io_cost (sys.bf, pending_delay);
        const long rest = size - sys.bf + pending;
        const long rem = rest % sys.bf;
        if (rest >= sys.bf) {
            cost += syscall_io_cost (rest - rem, 0);
        }
        cost += static_cast <double> (rem) / sys.bw_mem;
        pending = rem;
        pending_delay = static_cast <double> (rem) / sys.bw_mem;
    }
    return cost;
}

void model::io_cost::background_flush (double interval) {
    while (exist_expired_pages() || dirty >= sys.limit_bg) {
        long to_be_cleaned_size = io_list.at (id_clean).size;
        const double sync_time = static_cast <double> (to_be_cleaned_size) / sys.bw_sync;
        if (interval >= sync_time) {
            interval -= sync_time;
            dirty -= to_be_cleaned_size;
            id_clean ++;
        }
        else {
            const long interval_size = static_cast <long> (interval * sys.bw_sync);
            io_list.at (id_clean).size = to_be_cleaned_size - interval_size;
            dirty -= interval_size;
            break;
        }
    }
}

double model::io_cost::syscall_io_cost_complete (double delay, int fd, long offset, long size) {

    time += delay;
    double taskrate = sys.bw_ramdisk;
    background_flush_complete (delay);
    const bool exp = exist_expired_pages_complete ();
    const bool async_run = (dirty < setpoint) && (dirty >= sys.limit_bg || exp);
    const bool throttle_run = dirty >= setpoint;
    if (async_run) {
        taskrate = sys.bw_ramdisk * sys.coeff_bg;
    }
    else if (throttle_run) {
        const double pos_ratio = get_pos_ratio ();
        taskrate = std::min (bw_avg * pos_ratio, sys.bw_ramdisk * sys.coeff_bg);
//        taskrate = bw_avg * pos_ratio;
//        taskrate = taskrate * sys.bw_ramdisk * sys.coeff_bg /
//                   (sys.bw_ramdisk * sys.coeff_bg + taskrate * (1 - sys.coeff_bg));
    }
    const double cost = static_cast <const double> (size) / taskrate + sys.sc_w;

    const data_block dblock {fd, offset, size, time + cost};
    place_data_block_in_cache (dblock);

    bw_avg = (bw_avg * io_time + size) / (io_time + cost);
    time += cost;
    io_time += cost;

    background_flush_complete (cost);

    return cost;
}

void model::io_cost::background_flush_complete (double interval) {

    static auto inactive_pred = [] (const auto &data) {return data.active == false;};

    while (exist_expired_pages_complete () || dirty >= sys.limit_bg) {

        balance_active_inactive ();

        auto to_be_cleaned = std::ranges::find_if (page_cache, inactive_pred);

        const double sync_time = static_cast <double> (to_be_cleaned->size) / sys.bw_dev;//sys.bw_sync;
        if (interval >= sync_time) {
            interval -= sync_time;
            dirty -= to_be_cleaned->size;
            page_cache.erase (to_be_cleaned);
        }
        else {
            const long interval_size = static_cast <long> (interval * sys.bw_dev);//sys.bw_sync);
            to_be_cleaned->size = to_be_cleaned->size - interval_size;
            dirty -= interval_size;
            break;
        }

    }

}

void model::io_cost::balance_active_inactive () {

    if (active_pages < page_cache.size() / 2) {
        return;
    }

    std::ranges::sort (page_cache, {}, &data_block::io_finish_time);

    static auto active_filter = [] (const auto &data) {return data.active == true;};

    const auto make_inactive = active_pages - page_cache.size() / 2;
    if (make_inactive > 0) {
        auto active_cache = page_cache | std::views::filter (active_filter);
        static auto make_inactive_pred = [] (auto &active) {active = false;};
        std::ranges::for_each_n (active_cache.begin (), make_inactive, make_inactive_pred, &data_block::active);
        active_pages -= make_inactive;
    }

}

void model::io_cost::place_data_block_in_cache (const model::io_cost::data_block &dblock) {

    std::ranges::sort (page_cache, {}, &data_block::offset);

    static auto file_filter = [&dblock] (const auto &data) {return data.fd == dblock.fd;};
    static auto range_filter = [&dblock] (const auto &data) {
        const auto s1 = data.offset;
        const auto e1 = data.offset + data.size;
        const auto s2 = dblock.offset;
        const auto e2 = dblock.offset + dblock.size;
        const auto s1__s2__e1 = s1 <= s2 && e1 > s2;
        const auto s2__s1__e2 = s2 <= s1 && e2 > s1;
        return s1__s2__e1 || s2__s1__e2;
    };

    auto covered_data = page_cache | std::views::filter (file_filter) | std::views::filter (range_filter);

    std::vector <data_block> insert_to_cache;
    auto offset = dblock.offset;

    for (auto pos = covered_data.begin (); pos != covered_data.end (); pos ++) {

        // evict this part of data
        pos->evict = true;
        dirty -= pos->size;
        if (pos->active) {
            active_pages --;
        }

        if (offset < pos->offset) {
            insert_to_cache.emplace_back (dblock.fd, offset, pos->offset - offset, dblock.io_finish_time, false);
            offset = pos->offset;
        }
        else if (offset > pos->offset) {
            insert_to_cache.emplace_back (dblock.fd, pos->offset, offset - pos->offset, pos->io_finish_time, pos->active);
        }

        const auto min_end = std::min (dblock.offset + dblock.size, pos->offset + pos->size);
        insert_to_cache.emplace_back (dblock.fd, offset, min_end - offset, dblock.io_finish_time, true);
        offset = min_end;

        if (dblock.offset + dblock.size < pos->offset + pos->size) {
            insert_to_cache.emplace_back (dblock.fd, min_end, pos->offset + pos->size, pos->io_finish_time, pos->active);
            offset = pos->offset + pos->size;
        }

    }

    if (offset < dblock.offset + dblock.size) {
        insert_to_cache.emplace_back (dblock.fd, offset, dblock.offset + dblock.size - offset, dblock.io_finish_time, false);
    }

    std::erase_if (page_cache, [] (const auto &data) {return data.evict == true;});

    static auto update_meta_info = [this] (const auto &data) {
        dirty += data.size;
        if (data.active) {
            active_pages ++;
        }
    };

    std::ranges::for_each (insert_to_cache, update_meta_info);

//    page_cache.insert (page_cache.end(), insert_to_cache.begin(), insert_to_cache.end());
    std::move (insert_to_cache.begin(), insert_to_cache.end(), std::back_inserter (page_cache));
}
