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
// Created by Masoud Gholami on 01.02.22.
//

#include "file_io.hpp"

std::vector <std::byte> io_access::file_io::read_line () {

    if (syscall) {
        constexpr size_t bufsize = 32;
        char buf [bufsize];
        std::vector <std::byte> data;
        bool newline = false;
        while (read (fd, buf, bufsize) == bufsize) {
            for (const char c: buf) {
                if (c == '\n') {
                    newline = true;
                    break;
                }
                data.push_back ((std::byte) (c));
            }
            if (newline) {
                seek_file (seek + data.size() + 1);
                break;
            }
        }
        return data;
    }
    else {
        char *data;
        size_t size;
        assert (getline (&data, &size, fp) > 0);
        seek += size;
        return std::vector <std::byte> (reinterpret_cast <std::byte *> (data),
                                        reinterpret_cast <std::byte *> (data + size));
    }
}

void io_access::file_io::seek_file (long offset) {
    if (syscall) {
        assert (offset == lseek (fd, offset, SEEK_SET));
    }
    else {
        assert (0 == fseek (fp, offset, SEEK_SET));
    }
    seek = offset;
}

bool io_access::file_io::is_direct_io () const noexcept {
    return direct;
}
