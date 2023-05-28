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

#include "multiplot.hpp"

void plot::multiplot::generate_plots () {

    for (auto &p: plots) {
        assert (p.count_datasets > 0 && p.count_datasets == p.legends.size ());
    }

    generate_config();

    exec_gnuplot ();

}


void plot::multiplot::generate_config () {

    assert(plots.size() == rows_ * cols_);

    config_terminal ();

    conf_file << "set output \"" << output << "\"\n";
    conf_file << "set multiplot layout " << rows_ << "," << cols_ << " rowsfirst\n";


    // specify fixed margins for each row and column

    std::vector <double> cols_left_margins (cols_, std::numeric_limits <double>::lowest());
    std::vector <double> cols_right_margins (cols_, std::numeric_limits <double>::lowest());
    std::vector <double> rows_top_margins (rows_, std::numeric_limits <double>::lowest());
    std::vector <double> rows_bottom_margins (rows_, std::numeric_limits <double>::lowest());

    int count = 0;
    for (const auto &p: plots) {
        int row = count / cols_;
        int col = count % cols_;

        auto lmargin = cleft(plot_margins) + cleft (p.plot_margins);
        auto rmargin = cright(plot_margins) + cright (p.plot_margins);
        auto tmargin = ctop(plot_margins) + ctop (p.plot_margins);
        auto bmargin = cbottom(plot_margins) + cbottom (p.plot_margins);

        if (!share_y_ || col == 0) {
            lmargin += 4;
        }

        if (!share_x_ || row == rows_ - 1) {
            bmargin += 2;
        }

        if (const auto &text = p.plot_title.get_text (); !text.empty ()) {
            const auto &title_offset = p.plot_title.get_offset ();
            double distance = 2.0;
            if (!title_offset.empty()) {
                distance += title_offset.at (1);
            }
            tmargin += std::max (0.0, distance);
        }
        if (const auto &xlabel = p.x_ax.get_label(); !xlabel.empty ()) {
            const auto &xlabel_offset = p.x_ax.get_label_offset();
            double distance = 2.0;
            if (!xlabel_offset.empty()) {
                distance -= xlabel_offset.at (1);
            }
            bmargin += std::max (0.0, distance);
        }
        if (const auto &ylabel = p.y_ax.get_label(); !ylabel.empty ()) {
            const auto &ylabel_offset = p.y_ax.get_label_offset();
            double distance = 2.0;
            if (!ylabel_offset.empty()) {
                distance -= ylabel_offset.at (1);
            }
            lmargin += std::max (0.0, distance);
        }

        cols_left_margins.at (col) = std::max (cols_left_margins.at (col), lmargin);
        cols_right_margins.at (col) = std::max (cols_right_margins.at (col), rmargin);
        rows_top_margins.at (row) = std::max (rows_top_margins.at (row), tmargin);
        rows_bottom_margins.at (row) = std::max (rows_bottom_margins.at (row), bmargin);

        count ++;
    }

    // generate sub plots

    count = 0;
    for (auto &p: plots) {
        int row = count / cols_;
        int col = count % cols_;

        p.count_datasets = p.legends.size ();

        conf_file << "unset title; ";
        conf_file << "unset grid; ";
        conf_file << "unset arrow; ";
        conf_file << "unset label; ";
        conf_file << "unset xlabel; ";
        conf_file << "unset ylabel\n";
        conf_file << "set key default; ";
        conf_file << "set xtics auto; ";
        conf_file << "set xtics scale default; ";
        conf_file << "set xtics nooffset; ";
        conf_file << "set ytics scale default; ";
        conf_file << "set ytics nooffset; ";
        conf_file << "set ytics auto; ";
        conf_file << "set autoscale; ";
        conf_file << "set size noratio; ";
        conf_file << "set format;\n";

        if (share_y_ && col > 0) {
            conf_file << "set format y \'\'\n";
        }

        if (share_x_ && row < rows_ - 1) {
            conf_file << "set format x \'\'\n";
        }

        conf_file << "set lmargin " << cols_left_margins.at (col) << "; ";
        conf_file << "set rmargin " << cols_right_margins.at (col) << "; ";
        conf_file << "set tmargin " << rows_top_margins.at (row) << "; ";
        conf_file << "set bmargin " << rows_bottom_margins.at (row) << "\n";

        if (p.grid) {
            conf_file << "set grid\n";
        }

        const auto &data_file_name = subplot_data_file_name (count);
        std::ofstream data_file {data_file_name};
        data_file << p.tmp_data.str();

        p.config_plot_size (conf_file);

        p.config_title (conf_file);

        p.config_ax (conf_file, p.x_ax, "x");
        p.config_ax (conf_file, p.y_ax, "y");

        p.draw_labels (conf_file);
        p.draw_arrows (conf_file);

        p.config_legends (conf_file);
        p.draw_datasets (conf_file, data_file_name);

        count ++;
    }

    conf_file << "unset multiplot\n";

    conf_file << "set output\n";
    conf_file.flush ();

}


plot::multiplot::margin_t plot::multiplot::define_margins (int i, int j) const {
    double x_size = 1.0 - cleft (plot_margins) - cright (plot_margins);
    double y_size = 1.0 - ctop (plot_margins) - cbottom (plot_margins);

    double plot_x_size = x_size / rows_;
    double plot_y_size = y_size / cols_;

    margin_t plot_margin;
    top (plot_margin) = (i + 1) * plot_y_size + cbottom (plot_margins);
    bottom (plot_margin) = i * plot_y_size + cbottom (plot_margins);
    left (plot_margin) = j * plot_x_size + cleft (plot_margins);
    right (plot_margin) = (j + 1) * plot_x_size + cleft (plot_margins);

    return plot_margin;
}

std::string plot::multiplot::subplot_data_file_name (int count) const {
    return get_qualified_name (output) + "_" + std::to_string (count) + ".dat";
}

void plot::multiplot::share_x () {
    assert(!share_y_);
    share_x_ = true;
}

void plot::multiplot::share_y () {
    assert(!share_x_);
    share_y_ = true;
}


