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
// Created by Masoud Gholami on 02.02.22.
//

#include <climits>
#include "system_env.hpp"
#include "utils.hpp"


void measurement::system_env::blocking_sync () {
    syscall (SYS_sync);
    auto property = "Dirty";
    monitor::meminfo_monitor mm;
    auto dirty = mm.get_property (property);
    int count = 0;
    int max_count = 10;
    long max_dirty = 256;
    while (dirty > max_dirty) {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        auto last_dirty = dirty;
        dirty = mm.get_property (property);
        syscall (SYS_sync);

        if (dirty == last_dirty) {
            count++;

            if (count >= max_count) {
                count = 0;
                max_dirty *= 2;
            }
        }
        else {
            count = 0;
        }

    }
}

double measurement::system_env::measure_write_syscall_cost (int flags) const {
//    char buf [1] {};
//    auto syscall_fn = [fd, buf] () {return syscall (SYS_write, fd, buf, 0);};
//    auto dummycall_fn = [fd, buf] () {return dummycall (SYS_write, fd, buf, 0);};
//    double cost = measure_syscall_cost (syscall_fn, dummycall_fn);
    assert (bs > 0);
    long syscall_measure_data_size = 64*1024l*1024l;
    long syscall_measure_chunk_size = 1024l;

    long nchunks = syscall_measure_data_size / syscall_measure_chunk_size;

    blocking_sync ();
    auto buf = (unsigned char *) mmap (nullptr, syscall_measure_data_size, PROT_READ|PROT_WRITE,
                                       MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    int fd = open (dummyfile.c_str(), flags, S_IRWXU);
    assert (write (fd, buf, bs) == bs);
    timer_pack <2> timers;
    timers.start (0);
    assert (write (fd, buf, syscall_measure_data_size) == syscall_measure_data_size);
    timers.stop (0);

    timers.start (1);
    for (long i = 0; i < nchunks; i++) {
        assert (write (fd, buf, syscall_measure_chunk_size) == syscall_measure_chunk_size);
    }
    timers.stop (1);
    close (fd);

    double cost = (timers.duration (1) - timers.duration (0)) / static_cast <double> (nchunks);
    return cost;
}

double measurement::system_env::measure_seek_syscall_cost () const {
    int fd = open (dummyfile.c_str(), O_RDWR | O_CREAT | O_TRUNC | O_SYNC, S_IRWXU);

    std::vector <long> range (syscall_measure_repeats);
    for (int i = 0; i < syscall_measure_repeats; i++) {
        range.at (i) = i * syscall_measure_seek_chunk_size;
    }
    std::shuffle (range.begin (), range.end (), std::default_random_engine(random_seek));

    auto syscall_fn = [fd, &range, i = 0] () mutable {return syscall (SYS_lseek, fd, range[i++], SEEK_SET);};
    auto dummycall_fn = [fd, &range, i = 0] () mutable {return dummycall (SYS_lseek, fd, range[i++], SEEK_SET);};
    double cost = measure_syscall_cost (syscall_fn, dummycall_fn);
    close (fd);
    return cost;
}

long measurement::system_env::fetch_logical_block_size () {
    long logical_block_size;
    std::ifstream ifs ("/sys/block/sda/queue/logical_block_size");
    ifs >> logical_block_size;
    return logical_block_size;
}


std::string measurement::system_env::get_hostname () {
#ifdef LOCAL_MAC
    char hostname [100];
    gethostname(hostname, 100);
#else
    char hostname [HOST_NAME_MAX];
    gethostname(hostname, HOST_NAME_MAX);
#endif
    return std::string (hostname);
}

/**
 *
 * @param min_size
 * @param max_size
 * @param step
 * @return  <write syscall cost, write bw, read syscall cost, read bw>
 */
std::tuple <double, double, double, double>
measurement::system_env::perform_regression_experiment (long min_size, long max_size, long step, long repeats) const {

    timer_pack <2> timers;
    size_t rc;

    auto buf = (unsigned char *) mmap (nullptr, max_size, PROT_READ|PROT_WRITE,
                                       MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
/*
    blocking_sync ();
#ifdef _GNU_SOURCE
    int fd = open (dummyfile.c_str(), O_WRONLY | O_DSYNC | O_CREAT | O_TRUNC | O_DIRECT, S_IRWXU);
#else
    int fd = open (dummyfile.c_str(), O_WRONLY | O_DSYNC | O_CREAT | O_TRUNC, S_IRWXU);
#endif
    assert (write (fd, buf, bs) == bs);
*/
    long io_count = (max_size - min_size) / step + 1;


    std::vector <double> x;
    std::vector <double> expr_values;

    expr_values.reserve (io_count);
    x.reserve (io_count);

    auto data_size = min_size;
    for (int i = 0; i < io_count; ++i) {
    
blocking_sync ();
int fd = open (dummyfile.c_str(), O_WRONLY | O_DSYNC | O_TRUNC | O_DIRECT, S_IRWXU);
    assert (write (fd, buf, bs) == bs);

        timers.start (0);
        for (long j = 0; j < repeats; ++j) {
            rc = write (fd, buf, data_size);
        }
        timers.stop (0);

        if (rc != data_size) {
            perror ("Could not perform write operation");
        }

        expr_values.push_back (timers.duration (0) / (1.0 * repeats));
        x.push_back (data_size);

        std::cout << data_size << " " << timers.duration (0) / (1.0 * repeats) << std::endl;

        data_size += step;
    
	
close (fd);
    }

    auto write_regr = utils::linear_regression (x, expr_values);

    std::cout << "regression intercept " << write_regr.first << ", slope " << write_regr.second << std::endl;
 //   close (fd);

    blocking_sync ();
#ifdef _GNU_SOURCE
    int fd = open (dummyfile.c_str(), O_RDONLY | O_SYNC | O_DIRECT, S_IRWXU);
    std::cout << "gnu source" << std::endl;
#else
    fd = open (dummyfile.c_str(), O_RDONLY | O_SYNC, S_IRWXU);
#endif

    std::vector <double> read_expr_values;
    read_expr_values.reserve (io_count);

    data_size = min_size;
    for (int i = 0; i < io_count; ++i) {
lseek (fd, 0, SEEK_SET);
        timers.start (1);
        for (long j = 0; j < repeats; ++j) {
            rc = read (fd, buf, data_size);
        }
        timers.stop (1);

        if (rc != data_size) {
            perror ("Could not perform write operation");
        }

        read_expr_values.push_back (timers.duration (1) / (1.0 * repeats));

        data_size += step;
    }

    auto read_regr = utils::linear_regression (x, read_expr_values);

    munmap (buf, max_size);
    close (fd);

    return {write_regr.first, 1.0 / write_regr.second, read_regr.first, 1.0 / read_regr.second};
}



std::pair <double, double> measurement::system_env::measure_device_bandwidth () {

    assert(bs > 0 && (device_bandwidth_measure_data_size % bs == 0));

//    timer_pack <2> timers;
//    size_t rc;
//    auto buf = (unsigned char *) mmap (nullptr, device_bandwidth_measure_data_size, PROT_READ|PROT_WRITE,
//                               MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);



    long min_nano_size = bs;
    long max_nano_size = 128 * bs;
    long nano_step = bs;

    long min_normal_size = 128l*1024l*1024l;
    long max_normal_size =2l*1024l*1024l*1024l;
    long normal_step = 128l*1024l*1024l;

    auto nano_performance = perform_regression_experiment (min_nano_size, max_nano_size, nano_step, 128);
    sc_sw = std::max (0.0, std::get <0> (nano_performance));

    auto normal_performance = perform_regression_experiment (min_normal_size, max_normal_size, normal_step, 1);

    auto wbw = std::get <1> (normal_performance);
    auto rbw = std::get <3> (normal_performance);

    return {rbw, wbw};
}

std::pair <long, long> measurement::system_env::fetch_dirty_limits () {
    long pagesize = getpagesize ();

#ifdef LOCAL_MAC
    return {4796170l*pagesize, 9592338l*pagesize};
#else
    long nr_dirty_threshold = 0, nr_dirty_background_threshold = 0;
    std::ifstream ifs ("/proc/vmstat");
    std::string word;
    while (ifs >> word) {
        if (word == "nr_dirty_threshold") {
            ifs >> nr_dirty_threshold;
        }
        else if (word == "nr_dirty_background_threshold") {
            ifs >> nr_dirty_background_threshold;
        }
        else if (nr_dirty_background_threshold > 0 && nr_dirty_threshold > 0) {
            break;
        }
    }

    return {nr_dirty_background_threshold * pagesize, nr_dirty_threshold * pagesize};
#endif
}

int measurement::system_env::fetch_dirty_expire_centisecs () {
    int dirty_expire_centisecs;
    std::ifstream ifs ("/proc/sys/vm/dirty_expire_centisecs");
    ifs >> dirty_expire_centisecs;
    return dirty_expire_centisecs / 100;
}

double measurement::system_env::measure_memory_write_bandwidth () {

    auto p1 = (unsigned char *) mmap (nullptr, memory_bandwidth_measure_data_size, PROT_READ|PROT_WRITE,
                               MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    auto p2 = (unsigned char *) mmap (nullptr, memory_bandwidth_measure_data_size, PROT_READ|PROT_WRITE,
                               MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    int rc = mlock (p1, memory_bandwidth_measure_data_size);
    if (rc < 0) {
        perror ("Too large: memory_bandwidth_measure_data_size");
    }
    rc = mlock (p2, memory_bandwidth_measure_data_size);
    if (rc < 0) {
        perror ("Too large: memory_bandwidth_measure_data_size");
    }

    timer_pack <2> timers;
    timers.start (0);
    memcpy (p1, p2, memory_bandwidth_measure_data_size);
    timers.stop (0);

    timers.start (1);
    dummycall (p1, p2, memory_bandwidth_measure_data_size);
    timers.stop (1);

    munlock (p1, memory_bandwidth_measure_data_size);
    munlock (p2, memory_bandwidth_measure_data_size);
    munmap (p1, memory_bandwidth_measure_data_size);
    munmap (p2, memory_bandwidth_measure_data_size);

    return static_cast <double> (memory_bandwidth_measure_data_size) / (timers.duration (0) - timers.duration (1));
}

double measurement::system_env::measure_clib_latency () const {
    assert(bw_mem > 0);
    blocking_sync ();

    char *buf = (char *) malloc (bs);
    memset (buf, 0, bs);
    FILE *fd = fopen (dummyfile.c_str(), "w");
    fwrite (buf, 1, bs, fd);
    long total_size {bs};
    double total_time_diff {};
    int count {};
    timer_pack timer;
    for (int i = 0; i < 128; ++i) {
        timer.start (0);
        fwrite (buf, 1, bs, fd);
        timer.stop (0);
        total_size += bs;
        if (total_size % bf != 0) {
            total_time_diff += timer.duration (0) - bs/bw_mem;
            count ++;
        }
    }
    free(buf);
    fclose (fd);

    return total_time_diff / count;
}

long measurement::system_env::fetch_clib_buffer_size () const {
    char buf[1] {};
    FILE *fd = fopen (dummyfile.c_str(), "w");
    fwrite (buf, 1, 1, fd);
#ifdef LOCAL_MAC
    long buffer_size = 4096;
#else
    long buffer_size =  __fbufsize (fd);
#endif
    fclose (fd);
    return buffer_size;
}

std::tuple <double, double, double> measurement::system_env::measure_ramdisk_bandwidths () const {

    assert (limit_bg > 0 && limit_hard > 0 && sc_w > 0);

    std::vector <std::byte> buf (ramdisk_bandwidth_measure_data_size);

    long setpoint = (limit_bg + limit_hard) / 2;
    long writes_to_bg = limit_bg / ramdisk_bandwidth_measure_data_size + 1;
    long writes_before_sp = setpoint / ramdisk_bandwidth_measure_data_size - 1;
    long writes_to_sync = limit_hard * 2;
    long page_nr_interval = 300;
    long max_sync_samples = 6;
//    long page_nr_interval = 800;
//    long max_sync_samples = 2;

    blocking_sync ();
    int fd = open (dummyfile.c_str(), O_CREAT | O_RDWR | O_TRUNC, S_IRWXU);

    timer_pack <3> timer;
    monitor::meminfo_monitor mm;
    auto dirty_keyword = "Dirty";
    bool sync_reached = false;
    long dirty, last_dirty;

    for (long i = 0; i < writes_to_sync; i++) {
        if (i < writes_to_bg) {
            timer.start (0);
        }
        else if (i < writes_before_sp)  {
            timer.start (1);
        }
        else {
            dirty = mm.get_property (dirty_keyword);
            if (mm.get_property (dirty_keyword) < last_dirty + page_nr_interval) {
                sync_reached = true;
                timer.start (2);
            }
            else if (mm.get_property (dirty_keyword) < last_dirty - page_nr_interval) {
                break;
            }
        }

        int rc = write (fd, buf.data(), ramdisk_bandwidth_measure_data_size);
        if (i < writes_to_bg) {
            timer.stop (0);
        }
        else if (i < writes_before_sp) {
            timer.stop (1);
        }
        else {
            last_dirty = dirty;
            if (sync_reached) {
                timer.stop (2);
                sync_reached = false;
                if (timer.count (2) > max_sync_samples) {
                    break;
                }
            }
            else {
                timer.reset (2);
            }
        }

        assert (rc == ramdisk_bandwidth_measure_data_size);
    }

    close (fd);

    double freerun_bw = static_cast <double> (ramdisk_bandwidth_measure_data_size) / (timer.avg_duration (0) - sc_w);
    double async_bw = static_cast <double> (ramdisk_bandwidth_measure_data_size) / (timer.avg_duration (1) - sc_w);
    double sync_bw = static_cast <double> (ramdisk_bandwidth_measure_data_size) / (timer.avg_duration (2) - sc_w);

    std::tuple bandwidths = {freerun_bw, async_bw, sync_bw};
    return bandwidths;
}

void measurement::system_env::measure_host () {

    blocking_sync();
	
    std::cout << "starting with measurements" << std::endl;
    bs = fetch_logical_block_size ();
    pagesize = fetch_pagesize ();
    std::cout << "fetched pagesize and blocksize" << std::endl;
    sc_sw = measure_write_syscall_cost (O_RDWR | O_CREAT | O_TRUNC | O_SYNC);
    std::cout << "measured sync write syscall cost" << std::endl;
    sc_w = measure_write_syscall_cost (O_RDWR | O_CREAT | O_TRUNC);
    std::cout << "measured write syscall cost" << std::endl;
    sc_sk = measure_seek_syscall_cost ();
    std::cout << "measured seek system call cost" << std::endl;
    dirty_expire = fetch_dirty_expire_centisecs ();
    std::cout << "fetched dirty expire" << std::endl;
    bw_mem = measure_memory_write_bandwidth ();
    std::cout << "measured memory write bandwidth" << std::endl;
    bf = fetch_clib_buffer_size ();
    std::cout << "fetched C library buffer size" << std::endl;
    lib_metacost = measure_clib_latency ();
    std::cout << "measured C library latency" << std::endl;

    const auto &[rbw, wbw] = measure_device_bandwidth ();
    bw_rdev = rbw;
    bw_dev = wbw;
    std::cout << "measured device bandwidths" << std::endl;
    const auto &[freerun, asnyc, sync] = measure_ramdisk_bandwidths ();
    bw_ramdisk = freerun;
    coeff_bg = asnyc / freerun;
    bw_sync = sync;
    std::cout << "measured ramdisk bandwidths" << std::endl;

    remove (dummyfile.c_str());
}

bool measurement::system_env::load_from_config () {
    std::string section = get_config_section ();
    bool success = conf.go_to_section (section);
    if (!success) {
        return false;
    }

    bool config_load = true;

    auto get_val_from_ptr = [&config_load] (auto &val, const auto &ptr) {
        if (ptr) {
            val = *ptr;
        }
        else {
            config_load = false;
        }
    };

    get_val_from_ptr (sc_w, conf.get_property <double> ("write_syscall_cost"));
    get_val_from_ptr (sc_sw, conf.get_property <double> ("sync_write_syscall_cost"));
    get_val_from_ptr (pagesize, conf.get_property <double> ("page_size"));
    get_val_from_ptr (sc_sk, conf.get_property <double> ("seek_syscall_cost"));
    get_val_from_ptr (bs, conf.get_property <long> ("logical_block_size"));
    get_val_from_ptr (bw_rdev, conf.get_property <double> ("device_read_bandwidth"));
    get_val_from_ptr (bw_dev, conf.get_property <double> ("device_write_bandwidth"));
    get_val_from_ptr (bw_sync, conf.get_property <double> ("OS_sync_bandwidth"));
    get_val_from_ptr (bw_ramdisk, conf.get_property <double> ("ramdisk_write_bandwidth"));
    get_val_from_ptr (dirty_expire, conf.get_property <int> ("dirty_expire_seconds"));
    get_val_from_ptr (coeff_bg, conf.get_property <double> ("OS_background_sync_coefficient"));
    get_val_from_ptr (bw_mem, conf.get_property <double> ("memory_write_bandwidth"));
    get_val_from_ptr (bf, conf.get_property <long> ("C_library_buffer_size"));
    get_val_from_ptr (lib_metacost, conf.get_property <double> ("C_library_latency"));

    return config_load;
}

void measurement::system_env::store_to_config () {

    conf.add_section (get_config_section());
    conf.add_property ("write_syscall_cost", sc_w);
    conf.add_property ("page_size", pagesize);
    conf.add_property ("sync_write_syscall_cost", sc_sw);
    conf.add_property ("seek_syscall_cost", sc_sk);
    conf.add_property ("logical_block_size", bs);
    conf.add_property ("device_read_bandwidth", bw_rdev);
    conf.add_property ("device_write_bandwidth", bw_dev);
    conf.add_property ("OS_sync_bandwidth", bw_sync);
    conf.add_property ("ramdisk_write_bandwidth", bw_ramdisk);
    conf.add_property ("dirty_expire_seconds", dirty_expire);
    conf.add_property ("OS_background_sync_coefficient", coeff_bg);
    conf.add_property ("memory_write_bandwidth", bw_mem);
    conf.add_property ("C_library_buffer_size", bf);
    conf.add_property ("C_library_latency", lib_metacost);

    conf.flush();

}


std::string measurement::system_env::get_config_section () const {
#ifdef LOCAL_MAC
    return "nvram1.zib.de:/local/bzcghola";
#else
    return get_hostname() + ":" + device;
#endif
}

long measurement::system_env::fetch_pagesize () {
    return sysconf(_SC_PAGE_SIZE);
}
