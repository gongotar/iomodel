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

#ifndef EVALUATION_MULTIPLOT_HPP
#define EVALUATION_MULTIPLOT_HPP

#include <limits>
#include "gnuplot.hpp"

namespace plot {

    class gnuplot;

    class multiplot: public plot_base {

    private:

        std::vector <gnuplot> plots;
        const int rows_, cols_;
        std::vector <double> rows_weights_;
        std::vector <double> cols_weights_;
        bool share_x_ {false}, share_y_ {false};

        [[nodiscard]] margin_t define_margins (int i, int j) const;

        [[nodiscard]] std::string subplot_data_file_name (int count) const;

    public:

        multiplot (const std::string &plot_output, int rows, int cols):
                multiplot (plot_output, rows, cols, 0, 0) {}

        multiplot (const std::string &plot_output, int rows, int cols, int output_width, int output_height):
                plot_base {plot_output, output_width, output_height}, rows_ (rows), cols_ (cols) {}

        template <typename ... Plot>
        void set_plots (Plot && ... plot);

        void share_x ();
        void share_y ();

        template <typename ...T>
        void rows_weights (T&& ...w);
        template <typename ...T>
        void cols_weights (T&& ...w);

        void generate_plots ();
        void generate_config ();
    };


    template <typename... Plot>
    void multiplot::set_plots (Plot &&... plot) {
        static_assert ((std::is_same_v <std::decay_t <Plot>, gnuplot> && ...));
        assert ((plot.output.empty () && ...));
        (plots.emplace_back (std::move (plot)), ...);
        assert(plots.size() == rows_ * cols_);
    }


    template <typename... T>
    void multiplot::rows_weights (T &&... w) {
        (rows_weights_.emplace_back (std::forward <T> (w)), ...);
        assert (rows_weights_.size() == rows_);
    }

    template <typename... T>
    void multiplot::cols_weights (T &&... w) {
        (cols_weights_.emplace_back (std::forward <T> (w)), ...);
        assert (cols_weights_.size() == cols_);
    }


}

#endif //EVALUATION_MULTIPLOT_HPP
