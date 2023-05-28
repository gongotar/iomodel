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
// Created by Masoud Gholami on 31.01.22.
//

#ifndef EVALUATION_IMAGE_HPP
#define EVALUATION_IMAGE_HPP


#include <sstream>
#include <iostream>

#include "file_io.hpp"
#include "aligned_allocator.hpp"
#include "../measurement/system_env.hpp"

namespace io_access {

    class image {
    private:
        file_io fio;
        long header_size {};
        const bool read_only;

        static std::string bytes_as_string (const std::vector <std::byte> &data) {
            return {reinterpret_cast <const char *> (data.data ()),
                    reinterpret_cast <const char *> (data.data () + data.size ())};
        }

#ifdef __unix__
        static std::vector <std::byte, aligned_allocator <std::byte>> string_as_bytes (const std::string &str) {
            return {reinterpret_cast <const std::byte *> (str.data ()),
                    reinterpret_cast <const std::byte *> (str.data () + str.size ())};
        }
#else
        static std::vector <std::byte> string_as_bytes (const std::string &str) {
            return {reinterpret_cast <const std::byte *> (str.data ()),
                    reinterpret_cast <const std::byte *> (str.data () + str.size ())};
        }
#endif

    public:
        long w {}, h {};

        image (file_io &&fileio, long width, long height) :
                fio {std::move (fileio)}, w {width}, h {height}, read_only {false} {
            write_header ();
        }

        explicit image (file_io &&fileio) :
                fio {std::move (fileio)}, read_only {true} {
            read_header ();
        }

        inline std::vector <std::byte> read_data () {
            assert (read_only);
            std::vector <std::byte> data (w * h);
            fio.read_data_at (data.begin (), w * h, header_size);
            return data;
        }

        template <class Iter>
        inline void write_chunk (Iter data_begin, long chunk_size) {
            assert (!read_only);
            assert ((w * h) % chunk_size == 0);
            fio.write_data (data_begin, chunk_size);
        }

        template <class Iter>
        inline void write_chunk_n (Iter data_begin, long chunk_size, long n) {
            assert (!read_only);
            assert ((w * h) % chunk_size == 0);
            long index = n * chunk_size + header_size;
            fio.write_data_at (data_begin, chunk_size, index);
        }

        [[nodiscard]] inline long number_of_chunks (long chunk_size) const {
            assert ((w * h) % chunk_size == 0);
            return (w * h) / chunk_size;
        }

        void write_header ();

        void read_header ();

        image (const image &) = delete;

        image (image &&) = delete;

        image operator= (const image &) = delete;

        image operator= (image &&) = delete;

    };
}


#endif //EVALUATION_IMAGE_HPP
