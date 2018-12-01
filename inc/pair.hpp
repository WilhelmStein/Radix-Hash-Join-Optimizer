
#pragma once

#include <iosfwd>

namespace utility
{
    template <typename First, typename Second> struct pair;
    template <typename First, typename Second>
    std::ostream& operator<<(std::ostream&, const pair<First, Second>&);

    template <typename First, typename Second>
    struct pair
    {
        First first;
        Second second;

        pair();
        pair(const First&, const Second&);
        pair(const pair&);
        pair(pair&&) noexcept;

        pair& operator=(const pair&);
        pair& operator=(pair&&) noexcept;

        friend std::ostream& operator<< <First, Second>(std::ostream&, const pair<First, Second>&);
    };

    template <typename First, typename Second>
    bool operator<(const pair<First, Second>&, const pair<First, Second>&);

    template <typename First, typename Second>
    bool operator>(const pair<First, Second>&, const pair<First, Second>&);

    template <typename First, typename Second>
    bool operator==(const pair<First, Second>&, const pair<First, Second>&);

    template <typename First, typename Second>
    bool operator!=(const pair<First, Second>&, const pair<First, Second>&);
}

#include "pair.ipp"
