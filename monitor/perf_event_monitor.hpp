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

#ifndef EVALUATION_PERF_EVENT_MONITOR_HPP
#define EVALUATION_PERF_EVENT_MONITOR_HPP

#include <fcntl.h> // open
#include <sys/ioctl.h>

#ifdef __unix__
#include <linux/perf_event.h>    /* Definition of PERF_* constants */
#include <linux/hw_breakpoint.h> /* Definition of HW_* constants */
#include <sys/syscall.h>         /* Definition of SYS_* constants */
#endif

namespace monitor {

#ifdef __unix__
    class perf_event_monitor {
        int perf_fd;

    public:

        struct data_t {
            long long total_instructions {};
            long long instructions {};
            long count {};
        } data;


        perf_event_monitor () : perf_fd {-1} {
            // init perf to count instructions and other cpu events
            perf_event_attr perf_event {};
            memset (&perf_event, 0, sizeof (struct perf_event_attr));
            perf_event.type = PERF_TYPE_HARDWARE;
            perf_event.size = sizeof (struct perf_event_attr);
            perf_event.config = PERF_COUNT_HW_INSTRUCTIONS;
            perf_event.disabled = 1;
            perf_event.exclude_user = 1;
            perf_fd = syscall (SYS_perf_event_open, &perf_event, 0, -1, -1, 0);
            if (perf_fd == -1) {
                perror ("Error opening perf event");
            }
            ioctl (perf_fd, PERF_EVENT_IOC_RESET, 0);
        }

        inline void start_monitor () const noexcept {
            ioctl (perf_fd, PERF_EVENT_IOC_RESET, 0);
            ioctl (perf_fd, PERF_EVENT_IOC_ENABLE, 0);
        }

        inline void stop_monitor () noexcept {
            ioctl (perf_fd, PERF_EVENT_IOC_DISABLE, 0);
            read (perf_fd, &(data.instructions), sizeof (data.instructions));
            data.total_instructions += data.instructions;
            data.count++;
        }

        ~perf_event_monitor () {
            ioctl (perf_fd, PERF_EVENT_IOC_DISABLE, 0);
            close (perf_fd);
        }

        perf_event_monitor (const perf_event_monitor &) = delete;
    };

#else
    class perf_event_monitor {
        int perf_fd;

    public:

        struct data_t {
            long long total_instructions {};
            long long instructions {};
            long count {};
        } data;


        perf_event_monitor () {
        }

        inline void start_monitor () const noexcept {
        }

        inline void stop_monitor () noexcept {
        }

        ~perf_event_monitor () {
        }

        perf_event_monitor (const perf_event_monitor &) = delete;
    };

#endif
}

#endif //EVALUATION_PERF_EVENT_MONITOR_HPP
