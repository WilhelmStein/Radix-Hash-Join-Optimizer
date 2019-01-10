
#pragma once

#include <cstddef>       // std::size_t

namespace utility
{
    template <typename T, std::size_t N>
    class array
    {
        T data[N];

    public:

        class iterator
        {
            friend class array;

            T * ptr;

            iterator(T *);

        public:

            iterator();

            iterator(const iterator&);
            iterator(iterator&&) noexcept;

            iterator& operator=(const iterator&);
            iterator& operator=(iterator&&) noexcept;

            iterator& operator++();

            T& operator*() const;

            friend bool operator!=(const iterator& lhs, const iterator& rhs)
            {
                return lhs.ptr != rhs.ptr;
            }
        };

        array();
        
        array(const array&);

        array& operator=(const array&);

        T& operator[](std::size_t);

        std::size_t size() const;

        T& front();
        T& back();

        iterator begin();
        iterator end();
    };
}

#include <array.ipp>
