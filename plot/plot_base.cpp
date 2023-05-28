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
// Created by Masoud Gholami on 08.02.22.
//

#include <unistd.h>
#include <sys/wait.h>
#include "plot_base.hpp"


std::string plot::plot_base::get_qualified_name (std::string plot_output) {
    auto slash_pos = plot_output.find_last_of ('/');
    if (slash_pos != std::string::npos) {
        plot_output = plot_output.substr (slash_pos);
    }
    assert(!plot_output.empty ());

    auto dot_pos = plot_output.find_last_of ('.');
    if (dot_pos != std::string::npos) {
        plot_output.replace (dot_pos, 1, "_");
    }
    return plot_output;
}

std::string plot::plot_base::specify_terminal (const std::string &plot_output) {
    std::string term = "svg";
    auto dot_pos = plot_output.find_last_of ('.');
    auto ext = plot_output.substr (dot_pos + 1);
    if (ext == "tex") {
        term = "pstex";
    } else if (ext == "svg") {
        term = "svg";
    } else if (ext == "pdf") {
        term = "pdf";

    }
    return term;
}

void plot::plot_base::exec_gnuplot () {
    auto pid = fork ();
    assert (pid >= 0);
    if (pid) {
        int status;
        wait (&status);
        assert (WIFEXITED (status));
        assert (WEXITSTATUS (status) == EXIT_SUCCESS);
    } else {
        execlp ("gnuplot", "gnuplot", conf_file_name.c_str (), nullptr);
        perror ("Exec error");
        exit (EXIT_FAILURE);
    }
}
 
void plot::plot_base::config_terminal () {
    conf_file << "set term " << terminal;

    if (output_width_ * output_height_ > 0) {
        conf_file << " size " << output_width_ << ", " << output_height_;
    }

    conf_file << "\n";

}

void plot::plot_base::set_terminal (const std::string &term) {
    terminal = term;
}


void plot::plot_base::set_margins (const plot::plot_base::margin_t &margin) {
    plot_margins = margin;
}

void plot::plot_base::top_margin (double margin) {
    top (plot_margins) = margin;
}

void plot::plot_base::left_margin (double margin) {
    left (plot_margins) = margin;
}

void plot::plot_base::bottom_margin (double margin) {
    bottom (plot_margins) = margin;
}

void plot::plot_base::right_margin (double margin) {
    right (plot_margins) = margin;
}