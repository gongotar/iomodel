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
// Created by Masoud Gholami on 05.02.22.
//

#ifndef EVALUATION_CONFIG_HPP
#define EVALUATION_CONFIG_HPP

#include <fstream>
#include <string>
#include <memory>

class config {
private:
    std::fstream conf;
    long section_offset {-1};
public:
    explicit config (const std::string &config_file):
    conf (config_file, std::ios_base::in | std::ios_base::out | std::ios_base::app) {
        assert (conf.is_open());
    }

    void add_section (const std::string &section) {
        conf.seekp (0);
        conf << "\n[[" << section << "]]\n";
        section_offset = -1;
    }

    template <typename T>
    void add_property (const std::string &property, const T &value) {
        conf << property << "\t" << value << "\n";
        section_offset = -1;
    }

    bool go_to_section (const std::string &section) {
        conf.seekg (0);
        std::string section_token = "[[" + section + "]]";
        std::string token;
        while (conf >> token) {
            if (token == section_token) {
                section_offset = conf.tellg();
                return true;
            }
        }
        conf.clear ();
        return false;
    }

    template <typename T>
    std::unique_ptr <T> get_property (const std::string &property) {
        assert (section_offset >= 0);
        conf.seekp (section_offset);
        std::string token;
        while (conf >> token) {
            if (token == property) {
                T value;
                conf >> value;
                return std::make_unique <T>(value);
            }
        }
        return nullptr;
    }

    void flush () {
        conf.flush();
    }

};

#endif //EVALUATION_CONFIG_HPP
