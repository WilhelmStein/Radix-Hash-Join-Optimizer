
#pragma once

namespace utility
{
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
    };
}

#include <pair.ipp>
