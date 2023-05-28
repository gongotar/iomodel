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
// Created by Masoud Gholami on 09.02.22.
//

#ifndef EVALUATION_HEADER_HPP
#define EVALUATION_HEADER_HPP

#include <vector>

#include "plot_utility.hpp"

namespace plot {

    class title_t {
    private:
        std::string text_;
        std::string font_;
        std::vector <double> offset_;
    public:
        title_t () = default;
        explicit title_t (std::string text): text_ {std::move (text)} {}

        title_t &text (const std::string &t) {
            text_ = t;
            return *this;
        }

        title_t &font (const std::string &f) {
            maintain_string_pair (font_, ',', f, true);
            return *this;
        }

        title_t &font_size (double fs) {
            maintain_string_pair (font_, ',', std::to_string (fs), false);
            return *this;
        }

        title_t &offset (double x, double y) {
            offset_.push_back (x);
            offset_.push_back (y);
            return *this;
        }

        title_t &offset_3D (double x, double y, double z) {
            offset_.push_back (x);
            offset_.push_back (y);
            offset_.push_back (z);
            return *this;
        }


        [[nodiscard]] auto get_text () const noexcept {
            return text_;
        }
        [[nodiscard]] auto get_font () const noexcept {
            return font_;
        }
        [[nodiscard]] auto get_offset () const noexcept {
            return offset_;
        }
    };

}

#endif //EVALUATION_HEADER_HPP
