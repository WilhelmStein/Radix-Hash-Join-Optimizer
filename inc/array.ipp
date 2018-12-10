
#include <utility>  // std::move

// iterator<T> implementation
template <typename T, std::size_t N>
inline utility::array<T, N>::iterator::iterator(T * ptr)
:
ptr(ptr)
{
}

template <typename T, std::size_t N>
inline utility::array<T, N>::iterator::iterator()
:
ptr(nullptr)
{
}

template <typename T, std::size_t N>
inline utility::array<T, N>::iterator::iterator(const iterator& other)
:
ptr(other.ptr)
{
}

template <typename T, std::size_t N>
inline utility::array<T, N>::iterator::iterator(iterator&& other) noexcept
:
ptr(other.ptr)
{
    other.ptr = nullptr;
}

template <typename T, std::size_t N>
inline typename utility::array<T, N>::iterator& utility::array<T, N>::iterator::operator=(const iterator& other)
{
    ptr = other.ptr; return *this;
}

template <typename T, std::size_t N>
inline typename utility::array<T, N>::iterator& utility::array<T, N>::iterator::operator=(iterator&& other) noexcept
{
    ptr = other.ptr; other.ptr = nullptr;

    return *this;
}

template <typename T, std::size_t N>
inline typename utility::array<T, N>::iterator& utility::array<T, N>::iterator::operator++()
{
    ++ptr; return *this;
}

template <typename T, std::size_t N>
inline T& utility::array<T, N>::iterator::operator*() const
{
    return *ptr;
}

// array<T> implementation
template <typename T, std::size_t N>
inline utility::array<T, N>::array()
{
}

template <typename T, std::size_t N>
inline utility::array<T, N>::array(const array& other)
{
    for (std::size_t i = 0UL; i < N; i++)
        data[i] = other.data[i];
}

template <typename T, std::size_t N>
inline T& utility::array<T, N>::operator[](std::size_t index)
{
    return data[index];
}

template <typename T, std::size_t N>
inline std::size_t utility::array<T, N>::size() const
{
    return N;
}

template <typename T, std::size_t N>
inline T& utility::array<T, N>::front()
{
    return data[0UL];
}

template <typename T, std::size_t N>
inline T& utility::array<T, N>::back()
{
    return data[N - 1UL];
}

template <typename T, std::size_t N>
inline typename utility::array<T, N>::iterator utility::array<T, N>::begin()
{
    return iterator(data);
}

template <typename T, std::size_t N>
inline typename utility::array<T, N>::iterator utility::array<T, N>::end()
{
    return iterator(data + N);
}
