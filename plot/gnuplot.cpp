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

#include "gnuplot.hpp"


void plot::gnuplot::generate_plot () {
    assert (count_datasets > 0 && count_datasets == legends.size ());

    generate_config();

    exec_gnuplot ();
}

void plot::gnuplot::generate_config () {
    assert (!output.empty());

    count_datasets = legends.size();

    config_terminal ();

    conf_file << "set output \"" << output << "\"\n";

    if (grid) {
        conf_file << "set grid\n";
    }

    config_plot_margins ();

    config_plot_size (conf_file);

    config_title (conf_file);

    config_ax (conf_file, x_ax, "x");
    config_ax (conf_file, y_ax, "y");

    draw_labels (conf_file);
    draw_arrows (conf_file);

    config_legends (conf_file);
    draw_datasets (conf_file, data_file_name_);

    conf_file << "set output\n";
    conf_file.flush ();

}

std::string plot::gnuplot::generate_style_commands (const style &st) {

    std::stringstream style_commands;

    if (const auto &type = st.get_type (); !type.empty ()) {
        style_commands << " with " << type;
    }
    if (const auto &color = st.get_color (); !color.empty ()) {
        style_commands << " linecolor rgb \"" << color << "\"";
    }
    if (const auto width = st.get_width (); width > 0) {
        style_commands << " linewidth " << width;
    }
    if (const auto psize = st.get_point_size (); psize > 0) {
        style_commands << " pointsize " << psize;
    }
    return  style_commands.str();
}

void plot::gnuplot::config_plot_margins () {
    if (ctop (plot_margins) != 0) {
        conf_file << "set tmargin " << ctop (plot_margins) << "\n";
    }
    if (cbottom (plot_margins) != 0) {
        conf_file << "set bmargin " << cbottom (plot_margins) << "\n";
    }
    if (cleft (plot_margins) != 0) {
        conf_file << "set lmargin " << cleft (plot_margins) << "\n";
    }
    if (cright (plot_margins) != 0) {
        conf_file << "set rmargin " << cright (plot_margins) << "\n";
    }
}

void plot::gnuplot::config_plot_size (std::ostream &conf) {

    if (y_x_ratio == 0 && y_scale_ * x_scale_ == 0) {
        return;
    }


    if (y_x_ratio > 0) {
        conf << "set size";

        conf << " ratio " << y_x_ratio << "\n";
    }
    if (x_scale_ > 0) {
        conf << "set term " << terminal << " size";
        conf << " " << x_scale_ << "," << y_scale_;
    }

    conf << "\n";
}

void plot::gnuplot::config_title (std::ostream &conf) {
    if (const auto &text = plot_title.get_text (); !text.empty ()) {
        conf << "set title \"" << text << "\"";

        if (const auto &font = plot_title.get_font(); !font.empty ()) {
            conf << " font \"" << font << "\"";
        }
        if (const auto &offset = plot_title.get_offset(); !offset.empty ()) {
            conf << " offset " << delimiter_join (offset, ',');
        }
        conf << "\n";
    }
}


void plot::gnuplot::config_ax (std::ostream &conf, const axis &ax, const std::string &axis_name) {

    if (const auto &ax_range = ax.get_range(); !ax.get_range().empty()) {
        conf << "set " << axis_name << "range [" << ax.get_range () << "]\n";
    }

    if (const auto &ax_label = ax.get_label(); !ax_label.empty()) {
        conf << "set " << axis_name << "label \"" << ax_label << "\"";
        if (const auto &ax_label_font = ax.get_label_font(); !ax_label_font.empty()) {
            conf << " font \"" << ax_label_font << "\"";
        }
        if (const auto &label_offset = ax.get_label_offset (); !label_offset.empty()) {
            conf << " offset " << delimiter_join (label_offset, ',') << "\n";
        }
        conf << "\n";

    }
    if (ax.is_logscale()) {
        conf << "set logscale " << axis_name << "\n";
    }
    if (const auto &ax_tics_font = ax.get_tics_font (); !ax_tics_font.empty()) {
        conf << "set " << axis_name << "tics font \"" << ax_tics_font << "\"\n";
    }
    if (const auto &ax_format = ax.get_format(); !ax_format.empty()) {
        conf << "set format " << axis_name << " \"" << ax_format << "\"\n";
    }
    if (ax.get_tics_step () > 0) {
        conf << "set " << axis_name << "tics " << ax.get_tics_step () << "\n";
    }
    if (const auto &ax_tic_list = ax.get_tic_list(); !ax_tic_list.empty()) {
        conf << "set " << axis_name << "tics (" << delimiter_join (ax_tic_list, ',') << ")\n";
    }
    if (const auto &ax_tic_label_list = ax.get_tic_label_list(); !ax_tic_label_list.empty()) {
        conf << "set " << axis_name << "tics (" << delimiter_join (ax_tic_label_list, ',',[](const auto &p){
            return "\'" + p.second + "\' " + std::to_string (p.first);}) << ")\n";
    }
    if (const auto &ax_tic_number_list = ax.get_tic_number_list(); !ax_tic_number_list.empty()) {
        conf << "set " << axis_name << "tics (" << delimiter_join (ax_tic_number_list, ',', generate_pair_string) << ")\n";
    }
    if (const auto &rotate_tics = ax.get_rotate_tics(); rotate_tics > 0) {
        conf << "set " << axis_name << "tics rotate by " << rotate_tics << " right\n";
    }
    if (const auto &tics_scale = ax.get_tics_scale (); tics_scale > 0) {
        conf << "set " << axis_name << "tics scale " << tics_scale << "\n";
    }
    if (const auto &tics_offset = ax.get_tics_offset (); !tics_offset.empty()) {
        conf << "set " << axis_name << "tics offset " << delimiter_join (tics_offset, ',') << "\n";
    }
//
    if (const auto &tics_op = ax.get_tics_op (); tics_op != nullptr) {

        const auto &[min_val, max_val, step] = define_min_max_step (ax, axis_name);

        std::vector <double> values;
        values.reserve (static_cast <long> ((max_val - min_val) / step) + 1);
        auto v = min_val;
        do {
            values.emplace_back (v);
            v += step;
        } while (v < max_val);

        conf << "set " << axis_name << "tics (" << delimiter_join (values, ',',[&tics_op](const auto &v){
            return generate_pair_string ({v, tics_op (v)});
        }) << ")\n";

    }
//
}

void plot::gnuplot::draw_labels (std::ostream &conf) {
    for (const auto &l: labels) {
        conf << "set label \"" << l.get_text() << "\"";

        if (const auto &coordinate = l.get_coordinate(); !coordinate.empty()) {
            conf << " at " << delimiter_join (coordinate, ',');
        }
        if (const auto &str_position = l.get_string_position(); !str_position.empty()) {
            conf << " " << str_position;
        }
        if (const auto &font = l.get_font(); !font.empty()) {
            conf << " font \"" << font << "\"";
        }
        if (const auto rotate = l.get_rotate(); rotate > 0) {
            conf << " rotate by " << rotate;
        }

        conf << "\n";
    }
}


void plot::gnuplot::draw_arrows (std::ostream &conf) {

    for (const auto &ar: arrows) {
        conf << "set arrow";
        if (const auto& from = ar.get_from (); !from.empty()) {
            conf << " from " << delimiter_join (from, ',');
        }
        if (const auto& to = ar.get_to (); !to.empty()) {
            conf << " to " << delimiter_join (to, ',');
        }
        if (ar.is_nohead()) {
            conf << " nohead";
        }
        conf << generate_style_commands (ar);
        conf << "\n";
    }
}


void plot::gnuplot::draw_datasets (std::ostream &conf, const std::string &data_file_name) {

    conf << "plot ";
    auto x_specifier = tics_specifier (1, x_ax.get_tics_spec ());
    for (int i = 0; i < count_datasets; i++) {
        std::string ax_specifier = tics_specifier (i + 2, y_ax.get_tics_spec ());

        conf << "\"" << data_file_name << "\" using " << x_specifier << ":" << ax_specifier
                  << " title " << "\"" << legends.at (i) << "\"";

        if (linestyles.find (i) != linestyles.end ()) {
            conf << generate_style_commands (linestyles.at (i));
        }

        if (i != count_datasets - 1) {
            conf << ", \\\n\t";
        }
    }
    conf << "\n";

}

void plot::gnuplot::config_legends (std::ostream &conf) {
    if (legends.empty())
        return;

    if (!legend_config_.is_enabled()) {
        conf << "set key off\n";
        return;
    }

    conf << "set key";

    if (legend_config_.is_outside()) {
        conf << " outside";
    }
    if (const auto &coordinates = legend_config_.get_coordinates(); !coordinates.empty()) {
        conf << " at " << delimiter_join (coordinates, ',');
    }
    if (const auto &position = legend_config_.get_position(); !position.empty()) {
        conf << " " << position;
    }
    if (legend_config_.is_vertical()) {
        conf << " vertical";
    }
    else {
        conf << " horizontal";
    }
    if (const auto &legend_title = legend_config_.get_legend_title (); !legend_title.empty()) {
        conf << " title " << legend_title;
    }
    if (const auto &font = legend_config_.get_font(); !font.empty()) {
        conf << " font \"" << font << "\"";
    }
    if (legend_config_.has_box()) {
        conf << " box";
    }

    conf << "\n";
}


std::tuple <double, double, double>
plot::gnuplot::define_min_max_step (const plot::axis &ax, const std::string &axis_name) const {

    double step = ax.get_tics_step ();
    double min_val = ax.get_tics_min ();
    double max_val = ax.get_tics_max ();
    if (step == 0 && axis_name == "x") {
        step = (x_max - x_min) / 4.0;
    }
    else if (step == 0 && axis_name == "y") {
        step = (y_max - y_min) / 4.0;
    }
    if (!range_has_max (ax.get_range(), ':')) {
        if (axis_name == "x") {
            max_val = x_max;
        }
        else if (axis_name == "y") {
            max_val = y_max;
        }
    }
    if (!range_has_min (ax.get_range(), ':')) {
        if (axis_name == "x" && x_min < 0) {
            min_val = x_min;
        }
        else if (axis_name == "y" && y_min < 0) {
            min_val = y_min;
        }
    }

    return {min_val, max_val, step};
}

plot::axis &plot::gnuplot::x_axis () {
    return x_ax;
}

plot::axis &plot::gnuplot::y_axis () {
    return y_ax;
}

plot::linestyle_t &plot::gnuplot::linestyle (int line_id) {
    if (linestyles.find (line_id) == linestyles.end()) {
        linestyles.insert ({line_id, {}});
    }
    return linestyles.at (line_id);
}

plot::title_t &plot::gnuplot::title (const std::string &text) {
    plot_title.text (text);
    return plot_title;
}

plot::legend_t &plot::gnuplot::legends_config () {
    return legend_config_;
}


void plot::gnuplot::add_grid () {
    grid = true;
}

void plot::gnuplot::add_label (const plot::label_t &l) {
    labels.emplace_back (l);
}

void plot::gnuplot::add_arrow (const plot::arrow_t &a) {
    arrows.emplace_back (a);
}

void plot::gnuplot::plot_y_x_ratio (double ratio) {
    y_x_ratio = ratio;
}

void plot::gnuplot::plot_scale (double x_scale, double y_scale) {
    x_scale_ = x_scale;
    y_scale_ = y_scale;
}



