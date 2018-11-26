
#pragma once

#include <cstddef>       // std::size_t

namespace utility
{
    template <typename T>
    class array
    {
        T * data;

        std::size_t s;

    public:

        class iterator
        {
            friend class array;

            T * ptr;

            iterator(T *);

        public:

            iterator();

            iterator(const iterator&) = delete;
            iterator(iterator&&) noexcept;

            iterator& operator=(const iterator&) = delete;
            iterator& operator=(iterator&&) noexcept;

            iterator& operator++();

            T& operator*() const;

            friend bool operator!=(const iterator& lhs, const iterator& rhs)
            {
                return lhs.ptr != rhs.ptr;
            }
        };

        array(std::size_t);
        
        array(const array&) = delete;
        array(array&&) noexcept = delete;

        ~array();

        array& operator=(const array&) = delete;
        array& operator=(array&&) noexcept = delete;

        T& operator[](std::size_t);

        std::size_t size() const;

        iterator begin() const;
        iterator end() const;
    };
}

#include <array.ipp>
