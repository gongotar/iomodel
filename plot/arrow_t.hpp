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

#ifndef EVALUATION_ARROW_T_HPP
#define EVALUATION_ARROW_T_HPP

#include "style.hpp"

namespace plot {

    class arrow_t: public style {
    private:
        std::vector <double> from_coordinate_;
        std::vector <double> to_coordinate_;
        bool nohead_ {false};
    public:

        arrow_t &nohead () {
            nohead_ = true;
            return *this;
        }

        template <typename ... Coordinate>
        arrow_t &from (Coordinate &&... c) {
            (from_coordinate_.push_back (std::forward <Coordinate> (c)), ...);
            return *this;
        }

        template <typename ... Coordinate>
        arrow_t &to (Coordinate &&... c) {
            (to_coordinate_.push_back (std::forward <Coordinate> (c)), ...);
            return *this;
        }

        arrow_t &color (const std::string &c) override {
            color_ = c;
            return *this;
        }

        arrow_t &type (const std::string &t) override {
            type_ = t;
            return *this;
        }

        arrow_t &size (double s) override {
            point_size = s;
            return *this;
        }

        arrow_t &width (double w) override {
            line_width_ = w;
            return *this;
        }

        [[nodiscard]] auto get_from () const noexcept {
            return from_coordinate_;
        }

        [[nodiscard]] auto get_to () const noexcept {
            return to_coordinate_;
        }

        [[nodiscard]] auto is_nohead () const noexcept {
            return nohead_;
        }

    };

}

#endif //EVALUATION_ARROW_T_HPP
