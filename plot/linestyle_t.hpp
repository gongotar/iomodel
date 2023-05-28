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

#ifndef EVALUATION_LINESTYLE_T_HPP
#define EVALUATION_LINESTYLE_T_HPP

#include "style.hpp"

namespace plot {

    class linestyle_t : public style {

    public:
        linestyle_t &color (const std::string &c) override {
            color_ = c;
            return *this;
        }

        linestyle_t &type (const std::string &t) override {
            type_ = t;
            return *this;
        }

        linestyle_t &size (double s) override {
            point_size = s;
            return *this;
        }

        linestyle_t &width (double w) override {
            line_width_ = w;
            return *this;
        }
    };

}

#endif //EVALUATION_LINESTYLE_T_HPP
