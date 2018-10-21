
#pragma once

namespace nstd
{
    template <typename First, typename Second>
    struct pair
    {
        First first;
        Second second;

        pair(const First&, const Second&);
        pair(const pair&);

        pair& operator=(const pair&);
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
