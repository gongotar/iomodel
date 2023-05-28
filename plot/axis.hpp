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

#ifndef EVALUATION_AXIS_HPP
#define EVALUATION_AXIS_HPP

#include <string>
#include <vector>
#include <utility>
#include <functional>

#include "plot_utility.hpp"

namespace plot {

    class axis {
    private:
        std::string range_;
        std::string label_;
        std::string format_;
        double tics_ {};
        double min_tic {};
        double max_tic {};
        std::vector <double> tic_list;
        std::vector <std::pair <double, std::string>> tic_label_list;
        std::vector <std::pair <double, double>> tic_number_list;
        double rotate_tics_ {};
        double tics_scale_ {};
        std::vector <double> tics_offset_ {};
        bool logscale_ {false};
        std::string tics_font_;
        std::string label_font_;
        std::vector <double> label_offset_;
        std::string tics_spec_;
//
        std::function <double(double)> tics_op {nullptr};
//

    public:

        axis () = default;

        axis &range (double min, double max) {
            range_ = std::to_string (min) + ":" + std::to_string (max);
            min_tic = min;
            max_tic = max;
            return *this;
        }
        axis &min (double min) {
            maintain_string_pair (range_, ':', std::to_string (min), true);
            return *this;
        }
        axis &max (double max) {
            maintain_string_pair (range_, ':', std::to_string (max), false);
            return *this;
        }
        axis &label (const std::string &l) {
            label_ = l;
            return *this;
        }
        axis &format (const std::string &f) {
            format_ = f;
            return *this;
        }
        axis &tics (double t) {
            assert (tics_ == 0 && tic_list.empty() && tic_label_list.empty() && tic_number_list.empty());
            tics_ = t;
            return *this;
        }
        axis &tics_step (const std::vector <double> &ts) {
            assert (tics_ == 0 && tic_list.empty() && tic_label_list.empty() && tic_number_list.empty());
            tic_list = ts;
            return *this;
        }
        axis &tics (const std::vector <std::pair <double, std::string>> &ts) {
            assert (tics_ == 0 && tic_list.empty() && tic_label_list.empty() && tic_number_list.empty());
            tic_label_list = ts;
            return *this;
        }
        axis &tics (const std::vector <std::pair <double, double>> &ts) {
            assert (tics_ == 0 && tic_list.empty() && tic_label_list.empty() && tic_number_list.empty());
            tic_number_list = ts;
            return *this;
        }
//
        axis &transform_tics (std::function <double(double)> &&op) {
            tics_op = std::forward <std::function <double(double)>> (op);
            return *this;
        }
//
        axis &rotate_tics (double r) {
            rotate_tics_ = r;
            return *this;
        }
        axis &scale_tics (double s) {
            tics_scale_ = s;
            return *this;
        }
        axis &tics_offset (double x, double y) {
            tics_offset_.push_back (x);
            tics_offset_.push_back (y);
            return *this;
        }
        axis &tics_offset_3D (double x, double y, double z) {
            tics_offset_.push_back (x);
            tics_offset_.push_back (y);
            tics_offset_.push_back (z);
            return *this;
        }
        axis &logscale () {
            logscale_ = true;
            return *this;
        }
        axis &tics_font (const std::string &tf) {
            maintain_string_pair (tics_font_, ',', tf, true);
            return *this;
        }
        axis &tics_font_size (double tfs) {
            maintain_string_pair (tics_font_, ',', std::to_string (tfs), false);
            return *this;
        }
        axis &label_font(const std::string &lf) {
            maintain_string_pair (label_font_, ',', lf, true);
            return *this;
        }
        axis &label_font_size (double lfs) {
            maintain_string_pair (label_font_, ',', std::to_string (lfs), false);
            return *this;
        }
        axis &label_offset (double x, double y) {
            label_offset_.push_back (x);
            label_offset_.push_back (y);
            return *this;
        }
        axis &label_offset_3D (double x, double y, double z) {
            label_offset_.push_back (x);
            label_offset_.push_back (y);
            label_offset_.push_back (z);
            return *this;
        }
        axis &tics_specifier (const std::string &spec) {
            tics_spec_ = spec;
            return *this;
        }

        [[nodiscard]] auto get_range () const noexcept {
            return range_;
        }
        [[nodiscard]] auto get_tics_min () const noexcept {
            return min_tic;
        }
        [[nodiscard]] auto get_tics_max () const noexcept {
            return max_tic;
        }
        [[nodiscard]] auto get_label () const noexcept {
            return label_;
        }
        [[nodiscard]] auto get_format () const noexcept {
            return format_;
        }
        [[nodiscard]] auto get_tics_step () const noexcept {
            return tics_;
        }
        [[nodiscard]] auto get_tic_list () const noexcept {
            return tic_list;
        }
        [[nodiscard]] auto get_tic_label_list () const noexcept {
            return tic_label_list;
        }
        [[nodiscard]] auto get_tic_number_list () const noexcept {
            return tic_number_list;
        }
//
        [[nodiscard]] auto get_tics_op () const noexcept {
            return tics_op;
        }
//
        [[nodiscard]] auto is_logscale () const noexcept {
            return logscale_;
        }
        [[nodiscard]] auto get_label_font () const noexcept {
            return label_font_;
        }
        [[nodiscard]] auto get_label_offset () const noexcept {
            return label_offset_;
        }
        [[nodiscard]] auto get_tics_offset () const noexcept {
            return tics_offset_;
        }
        [[nodiscard]] auto get_tics_font () const noexcept {
            return tics_font_;
        }
        [[nodiscard]] auto get_rotate_tics () const noexcept {
            return rotate_tics_;
        }
        [[nodiscard]] auto get_tics_scale () const noexcept {
            return tics_scale_;
        }
        [[nodiscard]] auto get_tics_spec () const noexcept {
            return tics_spec_;
        }
    };

}

#endif //EVALUATION_AXIS_HPP
