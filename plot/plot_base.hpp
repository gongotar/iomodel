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

#ifndef EVALUATION_PLOT_BASE_HPP
#define EVALUATION_PLOT_BASE_HPP

#include <string>
#include <fstream>
#include <utility>
#include <tuple>

#include <cassert>

#include "title_t.hpp"


namespace plot {
    class plot_base {
    private:

        static std::string specify_terminal (const std::string &plot_output);

    protected:

        using margin_t = std::tuple <double, double, double, double>; // top, bottom, left, right
        static constexpr auto top = [] (margin_t &m) -> double & {return std::get <0> (m);};
        static constexpr auto bottom = [] (margin_t &m) -> double & {return std::get <1> (m);};
        static constexpr auto left = [] (margin_t &m) -> double & {return std::get <2> (m);};
        static constexpr auto right = [] (margin_t &m) -> double & {return std::get <3> (m);};
        static constexpr auto ctop = [] (const margin_t &m) -> double {return std::get <0> (m);};
        static constexpr auto cbottom = [] (const margin_t &m) -> double {return std::get <1> (m);};
        static constexpr auto cleft = [] (const margin_t &m) -> double {return std::get <2> (m);};
        static constexpr auto cright = [] (const margin_t &m) -> double {return std::get <3> (m);};


        int output_width_ {}, output_height_ {};
        std::string qualified_name;
        std::string conf_file_name;
        std::ofstream conf_file;
        std::string output;
        std::string terminal;
        margin_t plot_margins;

        static std::string get_qualified_name (std::string plot_output);

        plot_base () = default;

        explicit plot_base (const std::string &plot_output, int output_width, int output_height) :
                output_width_ {output_width}, output_height_ {output_height},
                qualified_name {get_qualified_name (plot_output)},
                conf_file_name {qualified_name + ".cfg"},
                conf_file {conf_file_name},
                output {plot_output},
                terminal {specify_terminal (plot_output)} {

            if (!conf_file.is_open()) {
                perror ("Could not open the config file!");
                exit (1);
            }
        }

        void config_terminal ();

        void exec_gnuplot ();

    public:
        void set_margins (const margin_t &margin);
        void top_margin (double margin);
        void left_margin (double margin);
        void right_margin (double margin);
        void bottom_margin (double margin);
        void set_terminal (const std::string &term);
    };

}


#endif //EVALUATION_PLOT_BASE_HPP
