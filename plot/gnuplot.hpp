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

#ifndef EVALUATION_GNUPLOT_HPP
#define EVALUATION_GNUPLOT_HPP

#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>

#include <cassert>

#include "style.hpp"
#include "axis.hpp"
#include "label_t.hpp"
#include "arrow_t.hpp"
#include "linestyle_t.hpp"
#include "plot_base.hpp"
#include "legend_t.hpp"
#include "plot_utility.hpp"

namespace plot {

    class multiplot;

    // TODO: custom plot shares in multiplot, multiple axes,  3d plot
    class gnuplot: public plot_base {
    private:

        std::string data_file_name_;
        std::ofstream data_file;
        std::stringstream tmp_data;

        title_t plot_title;
        legend_t legend_config_;
        std::vector <std::string> legends;
        std::unordered_map <int, linestyle_t> linestyles;
        std::vector <label_t> labels;
        std::vector <arrow_t> arrows;
        axis x_ax, y_ax;
//
        double x_max {};
        double y_max {};
        double z_max {};
        double x_min {};
        double y_min {};
        double z_min {};
//

        bool grid {false};
        double y_x_ratio {}, x_scale_ {}, y_scale_ {};

        int count_datasets {};

        std::tuple <double, double, double> define_min_max_step (const axis &ax, const std::string &axis_name) const;

        static std::string generate_style_commands (const style &st);

        void config_plot_margins ();

        void config_plot_size (std::ostream &conf);

        void config_title (std::ostream &conf);

        void config_ax (std::ostream &conf, const axis &ax, const std::string &axis_name);

        void draw_labels (std::ostream &conf);

        void draw_arrows (std::ostream &conf);

        void draw_datasets (std::ostream &conf, const std::string &data_file_name);

        void config_legends (std::ostream &conf);

        friend multiplot;

    public:

        gnuplot () = default;

        explicit gnuplot (const std::string &plot_output) :
                gnuplot(plot_output, 0, 0) {}

        gnuplot (const std::string &plot_output, int output_width, int output_height) :
                plot_base (plot_output, output_width, output_height),
                data_file_name_ {qualified_name + ".dat"},
                data_file {data_file_name_} {}

        title_t &title (const std::string &text);
        axis &x_axis ();
        axis &y_axis ();
        linestyle_t &linestyle (int line_id);
        legend_t &legends_config ();
        void add_label (const label_t &l);
        void add_arrow (const arrow_t &a);
        void add_grid ();
        void plot_y_x_ratio (double ratio);
        void plot_scale (double xscale, double yscale);
        template <typename X, typename ... Y>
        void set_data (const X &x, const Y &...y);
        template <typename ... T>
        void set_legends (T &&... t);
        template <typename ... LineStyle>
        void set_linestyles (LineStyle &&... ls);
        void generate_plot ();
        void generate_config ();

    };

    template <typename X, typename... Y>
    void gnuplot::set_data (const X &x, const Y &...y) {
        assert (count_datasets == 0);

        std::vector <const std::common_type_t <Y ...> *> all_data;
        (all_data.push_back (&y), ...);

        long size = x.size ();

        x_max = *std::max_element (x.cbegin (), x.cend ());
        x_min = *std::min_element (x.cbegin (), x.cend ());

        std::for_each (all_data.begin (), all_data.end (),
                       [&size, this] (const auto data) {
            assert((*data).size () == size);
            y_max = std::max (y_max, *std::max_element ((*data).cbegin (), (*data).cend ()));
            y_min = std::min (y_min, *std::min_element ((*data).cbegin (), (*data).cend ()));
        });

        static auto all_data_to_stream = [] (const auto &x,
                const auto &all_data,
                long size,
                auto &stream) {
                    for (int i = 0; i < size; i++) {
                        stream << x.at (i) << "\t";
                        for (const auto data: all_data) {
                            stream << std::fixed << (*data).at (i) << "\t";
                        }
                        stream << "\n";
                    }};
        if (data_file.is_open()) {
            all_data_to_stream (x, all_data, size, data_file);
            data_file.flush ();
        }
        else {
            all_data_to_stream (x, all_data, size, tmp_data);
        }
        count_datasets = all_data.size ();
        assert(count_datasets > 0);
        assert (legends.empty () || count_datasets == legends.size ());

    }


    template <typename... T>
    void gnuplot::set_legends (T &&... t) {
        static_assert ((std::is_constructible_v <std::string, T &&> && ...));
        assert (legends.empty ());
        (legends.push_back (std::forward <T> (t)), ...);
        assert (count_datasets == 0 || count_datasets == legends.size ());
    }


    template <typename... LineStyle>
    void gnuplot::set_linestyles (LineStyle &&... ls) {
        static_assert ((std::is_same_v <std::decay_t <LineStyle>, linestyle_t> && ...));
        int i = 0;
        (linestyles.emplace (i++, std::forward <LineStyle> (ls)), ...);
        assert (count_datasets == 0 || count_datasets == linestyles.size ());
    }

}

#endif //EVALUATION_GNUPLOT_HPP
