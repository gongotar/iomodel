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

#include "image.hpp"

void io_access::image::read_header () {
    assert (read_only);

    auto line1 = fio.read_line();
    auto line2 = fio.read_line();
    auto line3 = fio.read_line();

    std::string magic_identifier = bytes_as_string (line1);
    std::stringstream dimensions {bytes_as_string (line2)};
    dimensions >> w >> h;
    std::stringstream maxval_ss {bytes_as_string (line3)};
    int maxval;
    maxval_ss >> maxval;

    assert (magic_identifier == "P5");
    assert (w > 0 && h > 0 && maxval == 255);

    header_size = line1.size() + line2.size() + line3.size() + 3; // three times \n

}

void io_access::image::write_header () {
    assert (!read_only);
    std::stringstream ss;
    ss << "P5" << "\n" <<  w << "\t" << h << "\n" << 255 << std::endl;
    auto str = ss.str();
    auto header = string_as_bytes (str);
    header_size = static_cast <long> (header.size());

    if (fio.is_direct_io()) {
        auto bs = measurement::system_env::fetch_logical_block_size();
        while (header.size() % bs != 0) {
            header.emplace_back ();
        }
    }
    fio.write_data (header.begin(), header.size());
}
