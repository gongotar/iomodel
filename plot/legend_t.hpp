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
// Created by Masoud Gholami on 10.02.22.
//

#ifndef EVALUATION_LEGEND_T_HPP
#define EVALUATION_LEGEND_T_HPP

namespace plot {

    class legend_t {
    private:
        bool on_ {true};
        bool vertical_ {true};
        bool box_ {false};
        bool outside_ {false};
        std::string font_;
        std::string str_position_; // {left | right | center} {top | bottom | center}
        std::string title_;
        std::vector <double> coordinate_;
    public:
        legend_t() = default;

        legend_t &disable () {
            on_ = false;
            return *this;
        }
        legend_t &horizontal () {
            vertical_ = false;
            return *this;
        }
        legend_t &vertical () {
            vertical_ = true;
            return *this;
        }
        legend_t &with_box () {
            box_ = true;
            return *this;
        }
        legend_t &inside_plot () {
            outside_ = false;
            return *this;
        }
        legend_t &outside_plot () {
            outside_ = true;
            return *this;
        }
        legend_t &position (const std::string &pos) {
            str_position_ = pos;
            return *this;
        }
        legend_t &legend_title (const std::string &title) {
            title_ = title;
            return *this;
        }
        /**
         * Adjusts the font of the legends.
         *
         * @param f     legends font
         * @return      a gplot::legend_t reference to enable method chaining.
         */
        legend_t &font (const std::string &f) {
            maintain_string_pair (font_, ',', f, true);
            return *this;
        }
        /**
         * Adjusts the font size of the legends.
         *
         * @param fs    legends font size
         * @return      a gplot::legend_t reference to enable method chaining.
         */
        legend_t &font_size (double fs) {
            maintain_string_pair (font_, ',', std::to_string (fs), false);
            return *this;
        }
        legend_t &coordinates (double x, double y) {
            coordinate_.push_back (x);
            coordinate_.push_back (y);
            return *this;
        }
        legend_t &coordinates_3D (double x, double y, double z) {
            coordinate_.push_back (x);
            coordinate_.push_back (y);
            coordinate_.push_back (z);
            return *this;
        }

        [[nodiscard]] auto is_enabled () const noexcept {
            return on_;
        }
        [[nodiscard]] auto is_vertical () const noexcept {
            return vertical_;
        }
        [[nodiscard]] auto has_box () const noexcept {
            return box_;
        }
        [[nodiscard]] auto is_outside () const noexcept {
            return outside_;
        }
        [[nodiscard]] auto get_position () const noexcept {
            return str_position_;
        }
        [[nodiscard]] auto get_legend_title () const noexcept {
            return title_;
        }
        [[nodiscard]] auto get_coordinates () const noexcept {
            return coordinate_;
        }
        [[nodiscard]] auto get_font () const noexcept {
            return font_;
        }
    };

}

#endif //EVALUATION_LEGEND_T_HPP
