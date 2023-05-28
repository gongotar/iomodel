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

#ifdef __unix__
#ifndef _GNU_SOURCE
    #define _GNU_SOURCE
#endif
#endif

#ifndef EVALUATION_FILE_IO_HPP
#define EVALUATION_FILE_IO_HPP

#include <string>
#include <cstring>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdio>
#include <cassert>
#include <vector>

namespace io_access {
    class file_io {
    private:
        const bool syscall;
        int fd {};
        bool direct {};
        long seek {};
        FILE *fp;

    protected:

        file_io (const std::string &path, int flags) : syscall {true}, fp {nullptr} {
            fd = open (path.c_str (), flags, S_IRWXU);
#ifdef _GNU_SOURCE
            direct = (flags & O_DIRECT) > 0;
#else
            direct = false;
#endif
            if (fd < 0) {
                perror ("Could not open the file");
            }
        }

        file_io (const std::string &path, const std::string &mode) : syscall {false}, fd {-1} {
            fp = fopen (path.c_str (), mode.c_str ());
            if (!fp) {
                perror ("Could not open the file");
            }
        }


    public:

        std::vector <std::byte> read_line ();

        [[nodiscard]] bool is_direct_io () const noexcept;

        template <class Iter>
        void write_data (Iter data_begin, long size) {
            if (syscall) {
                ssize_t rc = write (fd, static_cast <std::byte *> (&(*data_begin)), size);
                if (rc != size) {
                    perror ("Unsuccessful write!");
                }
            } else {
                size_t rc = fwrite (static_cast <std::byte *> (&(*data_begin)), sizeof (std::byte), size, fp);
                if (rc != size) {
                    perror ("Unsuccessful write!");
                }
            }
            seek += size;
        }

        template <class Iter>
        void read_data (Iter data_begin, long size) {
            if (syscall) {
                ssize_t rc = read (fd, static_cast <std::byte *> (&(*data_begin)), size);
                assert(rc == size);
            } else {
                size_t rc = fread (static_cast <std::byte *> (&(*data_begin)), sizeof (std::byte), size, fp);
                assert(rc == size);
            }
            seek += size;
        }

        template <class Iter>
        void write_data_at (Iter data_begin, long size, long offset) {
            seek_file (offset);
            write_data (data_begin, size);
        }

        template <class Iter>
        void read_data_at (Iter data_begin, long size, long offset) {
            seek_file (offset);
            read_data (data_begin, size);
        }

        void seek_file (long offset);

        file_io (const file_io &) = delete;

        file_io operator= (const file_io &) = delete;

        file_io operator= (file_io &&) = delete;

        file_io (file_io &&fio) noexcept: syscall {fio.syscall} {

            fp = fio.fp;
            fd = fio.fd;
            seek = fio.seek;
            direct = fio.direct;
            fio.fp = nullptr;
            fio.fd = -1;
            fio.seek = 0;
        }

        ~file_io () {
            if (syscall && fd > 0) {
                close (fd);
            } else if (fp != nullptr) {
                fclose (fp);
                fp = nullptr;
            }
        }

    };

    class lib_file_io : public file_io {
    public:
        explicit lib_file_io (const std::string &path) : file_io (path, "w+") {}
    };

    class nonsync_file_io : public file_io {
    public:
        explicit nonsync_file_io (const std::string &path) : file_io (path, O_RDWR | O_CREAT | O_TRUNC) {}
    };

    class sync_file_io : public file_io {
    public:
        explicit sync_file_io (const std::string &path) : file_io (path, O_RDWR | O_SYNC | O_CREAT | O_TRUNC) {}
    };

    class readonly_file_io : public file_io {
    public:
        explicit readonly_file_io (const std::string &path) : file_io (path, O_RDONLY) {}
    };

#ifdef _GNU_SOURCE
    class direct_file_io : public file_io {
    public:
        explicit direct_file_io (const std::string &path) : file_io (path,
                                                                     O_RDWR | O_SYNC | O_DIRECT | O_CREAT | O_TRUNC) {}
    };
#endif
}

#endif //EVALUATION_FILE_IO_HPP
