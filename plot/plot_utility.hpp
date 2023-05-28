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
// Created by Masoud Gholami on 07.02.22.
//

#ifndef EVALUATION_PLOT_UTILITY_HPP
#define EVALUATION_PLOT_UTILITY_HPP

#include <string>
#include <vector>
#include <sstream>
#include <functional>
#include <cassert>

namespace plot {

    void maintain_string_pair (std::string &pair, char delim, const std::string &val, bool before_delim);

    template <typename T, typename OP = int>
    std::string delimiter_join (const std::vector <T> &values, char delim, OP op = 0);

    bool range_has_max (const std::string &range, char delim);

    bool range_has_min (const std::string &range, char delim);

    std::string generate_pair_string (const std::pair <double, double> &p);

    std::string tics_specifier (int i, std::string tics_fn);

}



template <typename T, typename OP>
std::string plot::delimiter_join (const std::vector <T> &values, char delim, OP op) {
    std::stringstream out;
    for (int i = 0; i < values.size (); i++) {
        if constexpr(std::is_same_v <int, OP>) {
            out << values.at (i);
        }
        else {
            out << op (values.at (i));
        }
        if (i != values.size () - 1) {
            out << delim;
        }
    }
    return out.str ();
}

#endif //EVALUATION_PLOT_UTILITY_HPP
