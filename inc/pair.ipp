
#include <fstream>
#include <utility>

template <typename First, typename Second>
utility::pair<First, Second>::pair()
:
first(), second()
{
}

template <typename First, typename Second>
utility::pair<First, Second>::pair(const First& first, const Second& second)
:
first(first), second(second)
{
}

template <typename First, typename Second>
utility::pair<First, Second>::pair(const utility::pair<First, Second>& other)
:
first(other.first), second(other.second)
{
}

template <typename First, typename Second>
utility::pair<First, Second>::pair(utility::pair<First, Second>&& other) noexcept
:
first(std::move(other.first)), second(std::move(other.second))
{
}

template <typename First, typename Second>
utility::pair<First, Second>& utility::pair<First, Second>::operator=(const utility::pair<First, Second>& other)
{
    first  = other.first;
    second = other.second;

    return *this;
}

template <typename First, typename Second>
utility::pair<First, Second>& utility::pair<First, Second>::operator=(utility::pair<First, Second>&& other) noexcept
{
    first  = std::move(other.first);
    second = std::move(other.second);

    return *this;
}
