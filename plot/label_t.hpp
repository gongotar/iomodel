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

#ifndef EVALUATION_LABEL_T_HPP
#define EVALUATION_LABEL_T_HPP

#include <vector>
#include <string>

#include "plot_utility.hpp"

namespace plot {

    class label_t {
    private:
        std::string l_;
        std::vector <double> coordinate;
        std::string str_position;   // {left | center | right}
        std::string font_;
        double rotate_ {};
    public:
        label_t () = default;
        explicit label_t (std::string text): l_ {std::move (text)} {};


        label_t &text (const std::string &l) {
            l_ = l;
            return *this;
        }

        label_t &coordination (double x, double y) {
            coordinate.push_back (x);
            coordinate.push_back (y);
            return *this;
        }
        label_t &coordination_3D (double x, double y, double z) {
            coordinate.push_back (x);
            coordinate.push_back (y);
            coordinate.push_back (z);
            return *this;
        }
        label_t &position (const std::string& pos) {
            str_position = pos;
            return *this;
        }
        label_t &rotate (double r) {
            rotate_ = r;
            return *this;
        }
        label_t &font (const std::string &f) {
            maintain_string_pair (font_, ',', f, true);
            return *this;
        }
        label_t &font_size (double fs) {
            maintain_string_pair (font_, ',', std::to_string (fs), false);
            return *this;
        }

        [[nodiscard]] auto get_text () const noexcept {
            return l_;
        }
        [[nodiscard]] auto get_coordinate () const noexcept {
            return coordinate;
        }
        [[nodiscard]] auto get_string_position () const noexcept {
            return str_position;
        }
        [[nodiscard]] auto get_font () const noexcept {
            return font_;
        }
        [[nodiscard]] auto get_rotate () const noexcept {
            return rotate_;
        }
    };

}

#endif //EVALUATION_LABEL_T_HPP
