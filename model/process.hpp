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
// Created by Masoud Gholami on 04.02.22.
//

#ifndef EVALUATION_IMAGE_PROCESS_HPP
#define EVALUATION_IMAGE_PROCESS_HPP

#include <functional>

#include "../measurement/timer_pack.hpp"

namespace model {

    template <typename WriteFn, typename ComputeTimeFn>
    class process {
    private:
        WriteFn write_fn_;
        ComputeTimeFn compute_time_fn_;
        std::size_t dummy_val;
    public:
        explicit process (WriteFn write_fn, ComputeTimeFn compute_time_fn):
        write_fn_ {write_fn}, compute_time_fn_ {compute_time_fn}, dummy_val {sizeof (write_fn)} {}

        inline void write () noexcept {
            write_fn_ ();
        }

        inline void compute () noexcept {

            measurement::timer_pack timer;
            timer.start (0);
            while (timer.live_duration (0) < compute_time_fn_ ()) {
                for (int i = 0; i < 10; ++i) {
                    dummy_val = std::hash<std::size_t> {} (dummy_val);
                }
            }
        }
    };
}
#endif //EVALUATION_IMAGE_PROCESS_HPP
