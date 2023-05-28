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

#ifndef EVALUATION_SYSTEM_ENV_HPP
#define EVALUATION_SYSTEM_ENV_HPP

#include <iostream>
#include <array>
#include <random>
#include <algorithm>
#include <string>
#include <fstream>
#include <memory>
#include <utility>
#include <unordered_map>

#include <cassert>
#include <sys/fcntl.h>
#include <cstdio>
#include <unistd.h>
#include <sys/syscall.h>         /* Definition of SYS_* constants */
#include <cstring>

#ifdef __unix__
#include <stdio_ext.h>
#endif

#include <thread>
#include <sys/mman.h>

#include "timer_pack.hpp"
#include "../monitor/meminfo_monitor.hpp"
#include "config.hpp"

//#define LOCAL_MAC


namespace measurement {

    class system_env {

    private:

        int random_seek = 10;
        static constexpr int syscall_measure_repeats = 200000;
        static constexpr int syscall_measure_seek_chunk_size = 128;
        static constexpr long device_bandwidth_measure_data_size = 1024l * 1024l * 256l;
        static constexpr long device_bandwidth_measure_chunk_number = 10;
        static constexpr long ramdisk_bandwidth_measure_data_size = 512l * 1024l * 1024l;
        static constexpr long memory_bandwidth_measure_data_size = 32 * 1024l;

        inline static std::string default_config_file = "config.io";
        config conf;
        const std::string device;
        const std::string dummyfile;

        template <typename ... T>
        static ssize_t __attribute__ ((noinline)) dummycall (T ... t);

        [[nodiscard]] std::tuple <double, double, double, double>
        perform_regression_experiment (long min_size, long max_size, long step, long repeats) const;

        template <typename Callable, typename DummyCallable>
        [[nodiscard]] double measure_syscall_cost (Callable syscall_fn, DummyCallable dummycall_fn) const;

        [[nodiscard]] double measure_write_syscall_cost (int flags) const;

        [[nodiscard]] double measure_seek_syscall_cost () const;

        [[nodiscard]] double measure_clib_latency () const;

        [[nodiscard]] std::pair <double, double> measure_device_bandwidth ();

        [[nodiscard]] static std::pair <long, long> fetch_dirty_limits ();

        [[nodiscard]] static int fetch_dirty_expire_centisecs ();

        [[nodiscard]] std::tuple <double, double, double> measure_ramdisk_bandwidths () const;

        [[nodiscard]] static double measure_memory_write_bandwidth () ;

        [[nodiscard]] long fetch_clib_buffer_size () const;

        [[nodiscard]] std::string get_config_section () const;

        [[nodiscard]] static long fetch_pagesize () ;

        bool load_from_config ();

        void store_to_config ();

    public:

        double sc_w {};
        double sc_sw {};
        double sc_sk {};
        long bs {};
        double bw_rdev {};

        double bw_dev {};
        double bw_sync {};
        double bw_ramdisk {};
        long limit_bg {};
        long limit_hard {};
        int dirty_expire {};
        double coeff_bg {};

        double bw_mem {};
        long bf {};
        long pagesize {};
        double lib_metacost {};

        system_env (const std::string &device_path, const std::string &config_file):
        conf {config_file},
        device {device_path},
        dummyfile {device_path + "/dummyfile"} {

            const auto &[bg, hard] = fetch_dirty_limits ();
            limit_bg = bg;
            limit_hard = hard;

            if (!load_from_config ()) {
                measure_host ();
                store_to_config ();
            }
        }

        explicit system_env (const std::string &device_path):
        system_env (device_path, default_config_file) {}

        void measure_host ();
        
        static void blocking_sync ();


        friend std::ostream& operator << (std::ostream& os, const system_env& sys) {
            double gb = 1024.0*1024.0*1024.0;

            os  << "sc_w " << sys.sc_w
                << ", sc_sw " << sys.sc_sw
                << ", pg_size " << sys.pagesize
                << ", sc_sk " << sys.sc_sk
                << ", bs " << sys.bs
                << ", bw_rdev " << sys.bw_rdev / gb
                << ", bw_dev " << sys.bw_dev / gb
                << ", bw_sync " << sys.bw_sync / gb
                << ", bw_ramdisk " << sys.bw_ramdisk / gb
                << ", limit_bg " << sys.limit_bg / gb
                << ", limit_hard " << sys.limit_hard /gb
                << ", dirty_expire " << sys.dirty_expire
                << ", coeff_bg " << sys.coeff_bg
                << ", bw_mem " << sys.bw_mem / gb
                << ", libc_latency " << sys.lib_metacost
                << ", bf " << sys.bf;

            return os;
        }


        [[nodiscard]] static long fetch_logical_block_size ();
        [[nodiscard]] static std::string get_hostname ();


    };

    template <typename... T>
    ssize_t __attribute__ ((noinline)) system_env::dummycall (T... t) {
        return 0;
    }

    template <typename Callable, typename DummyCallable>
    double system_env::measure_syscall_cost (Callable syscall_fn, DummyCallable dummycall_fn) const {

        timer_pack <2> timers;
        timers.start (0);
        for (int i = 0; i < syscall_measure_repeats; i++) {
            syscall_fn ();
        }
        timers.stop (0);

        timers.start (1);
        for (int i = 0; i < syscall_measure_repeats; i++) {
            dummycall_fn ();
        }
        timers.stop (1);
        return (timers.duration (0) - timers.duration (1)) / static_cast <double> (syscall_measure_repeats);
    }

}

#endif //EVALUATION_SYSTEM_ENV_HPP
