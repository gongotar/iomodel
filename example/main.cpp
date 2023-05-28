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


 #include <iostream>

#include "io_access/file_io.hpp"
#include "io_access/image.hpp"
#include "monitor/background_monitor.hpp"
#include "model/io_cost.hpp"
#include "model/process.hpp"
#include "plot/gnuplot.hpp"
#include "plot/multiplot.hpp"

int main () {

    int interval = 10;
    long min_size = 32l*1024l*1024l;
    long max_size = 2l*1024l*1024l*1024l;
    long step = min_size;

    long io_count = (max_size - min_size) / step + 1;

    std::fstream out ("gplot_sync_range_1.dat", std::ios::out | std::ios::trunc);
    std::string path = "path_to_dev";
    auto io = io_access::sync_file_io {path + "/tmp.pgm"};
    std::vector <std::byte, io_access::aligned_allocator <std::byte>> data (0);


    measurement::timer_pack timer;

    measurement::system_env sys (path);
    model::io_cost iocost (sys);

    std::cout << sys << std::endl;

    std::vector <long> x;
    std::vector <double> model_values;
    std::vector <double> expr_values;

    model_values.reserve (io_count);
    expr_values.reserve (io_count);
    x.reserve (io_count);

    auto data_size = min_size;
    for (int i = 0; i < io_count; ++i) {

        timer.start (0);
        io.write_data (data.begin(), data_size);
        timer.stop (0);

        double model_cost = iocost.sync_io_cost(data_size, 0);

        model_values.push_back (model_cost);
        expr_values.push_back (timer.duration (0));
        x.push_back (data_size);

	out << data_size << "\t" << timer.duration (0) << "\t" << model_cost << std::endl;
        data_size += step;
    }


    return 0;
}
