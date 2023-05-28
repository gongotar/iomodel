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
// Created by Masoud Gholami on 07.02.22.
//

#ifndef EVALUATION_ALIGNED_ALLOCATOR_HPP
#define EVALUATION_ALIGNED_ALLOCATOR_HPP

#include <cstdlib>
#include <new>
#include <limits>

namespace io_access {

    template <class T>
    struct aligned_allocator {
        typedef T value_type;

        aligned_allocator () = default;

        template <class U>
        constexpr explicit aligned_allocator (const aligned_allocator <U> &) noexcept {}

        [[nodiscard]] T *allocate (std::size_t n) {

            if (n > std::numeric_limits <std::size_t>::max () / sizeof (T))
                throw std::bad_array_new_length ();

            if (auto p = static_cast <T *> (std::aligned_alloc (512, n * sizeof (T)))) {
                std::memset (p, '\0', n * sizeof (T));
                return p;
            }

            throw std::bad_alloc ();
        }

        void deallocate (T *p, std::size_t n) noexcept {
            std::free (p);
        }

    };
}

template <class T, class U>
bool operator== (const io_access::aligned_allocator <T>&, const io_access::aligned_allocator <U>&) {return true;}
template <class T, class U>
bool operator!= (const io_access::aligned_allocator <T>&, const io_access::aligned_allocator <U>&) {return false;}


#endif //EVALUATION_ALIGNED_ALLOCATOR_HPP
