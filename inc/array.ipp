
#include <utility>  // std::move

// iterator<T> implementation
template <typename T>
inline utility::array<T>::iterator::iterator(T * ptr)
:
ptr(ptr)
{
}

template <typename T>
inline utility::array<T>::iterator::iterator()
:
ptr(nullptr)
{
}

template <typename T>
inline utility::array<T>::iterator::iterator(iterator&& other) noexcept
:
ptr(other.ptr)
{
    other.ptr = nullptr;
}

template <typename T>
inline typename utility::array<T>::iterator& utility::array<T>::iterator::operator=(iterator&& other) noexcept
{
    ptr = other.ptr; other.ptr = nullptr;

    return *this;
}

template <typename T>
inline typename utility::array<T>::iterator& utility::array<T>::iterator::operator++()
{
    ++ptr; return *this;
}

template <typename T>
inline T& utility::array<T>::iterator::operator*() const
{
    return *ptr;
}

// array<T> implementation
template <typename T>
inline utility::array<T>::array(std::size_t s)
:
data(new T[s]), s(s)
{
}

// template <typename T>
// inline utility::array<T>::array(array&& other) noexcept
// :
// data(other.data), s(std::move(other.s))
// {
//     other.data = nullptr;
// }

template <typename T>
inline utility::array<T>::~array()
{
    delete[] data;
}

// template <typename T>
// inline utility::array<T>& utility::array<T>::operator=(array&& other) noexcept
// {
//     data = other.data; other.data = nullptr;

//     s = std::move(other.s);

//     return *this;
// }

template <typename T>
inline T& utility::array<T>::operator[](std::size_t index)
{
    return data[index];
}

template <typename T>
inline std::size_t utility::array<T>::size() const
{
    return s;
}

template <typename T>
inline typename utility::array<T>::iterator utility::array<T>::begin() const
{
    return iterator(data);
}

template <typename T>
inline typename utility::array<T>::iterator utility::array<T>::end() const
{
    return iterator(data + s);
}
