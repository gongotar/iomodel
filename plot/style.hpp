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
// Created by Masoud Gholami on 06.02.22.
//

#ifndef EVALUATION_STYLE_HPP
#define EVALUATION_STYLE_HPP

namespace plot {

    class style {
    protected:
        std::string color_;
        std::string type_;   // lines, points, linespoints, impulses, dots, steps, fsteps, histeps
        double line_width_ {}, point_size {};

    public:

        style () = default;

        virtual style &color (const std::string &c) = 0;
        virtual style &type (const std::string &t) = 0;
        virtual style &size (double s) = 0;
        virtual style &width (double w) = 0;

        [[nodiscard]] auto get_color () const noexcept {
            return color_;
        }

        [[nodiscard]] auto get_type () const noexcept {
            return type_;
        }

        [[nodiscard]] auto get_width () const noexcept {
            return line_width_;
        }

        [[nodiscard]] auto get_point_size () const noexcept {
            return point_size;
        }
    };

}



#endif //EVALUATION_STYLE_HPP
