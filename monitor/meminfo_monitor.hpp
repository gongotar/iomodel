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
// Created by Masoud Gholami on 03.02.22.
//

#ifndef EVALUATION_MEMINFO_MONITOR_HPP
#define EVALUATION_MEMINFO_MONITOR_HPP

#include <fstream>
#include <sstream>

namespace monitor {

    class meminfo_monitor {
        private:
        std::ifstream meminfo;
        public:
        meminfo_monitor (): meminfo {"/proc/meminfo"} {}

        long get_property (const std::string &key) {
            meminfo.seekg (0);
            auto fixed_key = key + ":";
            long value = -1;
            std::string word;
            while (meminfo >> word) {
                if (word == fixed_key) {
                    meminfo >> value;
                    break;
                }
            }
            return value;
        }

    };
}

#endif //EVALUATION_MEMINFO_MONITOR_HPP
