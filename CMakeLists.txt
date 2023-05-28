cmake_minimum_required(VERSION 3.16)
project(evaluation)

set(CMAKE_CXX_STANDARD 20)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

add_executable(evaluation example/main.cpp io_access/file_io.hpp io_access/image.cpp io_access/image.hpp io_access/file_io.cpp measurement/timer_pack.hpp monitor/background_monitor.hpp model/io_cost.hpp model/io_cost.cpp measurement/system_env.cpp measurement/system_env.hpp monitor/perf_event_monitor.hpp monitor/meminfo_monitor.hpp model/process.hpp plot/gnuplot.hpp measurement/config.hpp plot/style.hpp plot/gnuplot.cpp plot/axis.hpp plot/label_t.hpp plot/plot_utility.hpp plot/plot_utility.cpp plot/arrow_t.hpp plot/linestyle_t.hpp io_access/aligned_allocator.hpp plot/multiplot.hpp plot/plot_base.hpp plot/plot_base.cpp plot/multiplot.cpp plot/title_t.hpp plot/legend_t.hpp measurement/utils.hpp)
target_link_libraries(evaluation Threads::Threads)
configure_file(${PROJECT_SOURCE_DIR}/pictures/posterized_pic.pgm posterized_pic.pgm COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/python_scripts/regression.py regression.py COPYONLY)


configure_file(${PROJECT_SOURCE_DIR}/config.io config.io COPYONLY)

