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

#include "plot_utility.hpp"

void plot::maintain_string_pair (std::string &pair, char delim, const std::string &val, bool before_delim) {
    if (pair.empty ()) {
        if (before_delim) {
            pair = val + delim;
        } else {
            pair = delim + val;
        }
    } else {
        auto pos = pair.find (delim);
        if (before_delim) {
            pair = val + delim + pair.substr (pos + 1);
        } else {
            pair = pair.substr (0, pos + 1) + val;
        }
    }
}


std::string plot::tics_specifier (int i, std::string tics_fn) {
    if (tics_fn.empty()) {
        return std::to_string (i);
    }
    else {
        auto pos = tics_fn.find ('$');
        assert (pos != std::string::npos);
        tics_fn.insert (pos + 1, std::to_string (i));
        return "(" + tics_fn + ")";
    }
}

std::string plot::generate_pair_string (const std::pair <double, double> &p) {
    std::stringstream out;
    out <<  "\"" << p.second << "\" " << p.first;
    return out.str();
}


bool plot::range_has_max (const std::string &range, char delim) {
    if (range.empty()) {
        return false;
    }
    auto pos = range.find (delim);
    return (pos < range.size() - 1);
}

bool plot::range_has_min (const std::string &range, char delim) {
    if (range.empty()) {
        return false;
    }
    auto pos = range.find (delim);
    return (pos > 0);
}
