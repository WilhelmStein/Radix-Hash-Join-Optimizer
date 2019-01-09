
#pragma once

#include <types.hpp>
#include <query.hpp>

#include <iosfwd>
#include <utility>

namespace RHJ
{
    extern class Statistics
    {
        float l, u, f, d;

    public:

        Statistics()
        :
        l(0.0f), u(0.0f), f(0.0f), d(0.0f) {}

        Statistics(float l, float u, float f, float d)
        :
        l(l), u(u), f(f), d(d) {}

        Statistics(const Statistics& other)
        :
        l(other.l), u(other.u), f(other.f), d(other.f) {}

        Statistics(Statistics&& other) noexcept
        :
        l(std::move(other.l)), u(std::move(other.u)),
        f(std::move(other.f)), d(std::move(other.d)) {}

        Statistics& operator=(const Statistics& other)
        {
            l = other.l; u = other.u; f = other.f; d = other.d;

            return *this;
        }

        Statistics& operator=(Statistics&& other) noexcept
        {
            l = std::move(other.l); u = std::move(other.u);

            f = std::move(other.f); d = std::move(other.d);

            return *this;
        }

        friend std::ostream& operator<<(std::ostream&, const Statistics&);

        static void load();
        static void dump();

        static float parse
        (
            const std::size_t *, std::size_t,
            const Query::Predicate *, std::size_t
        );

    } ** statistics;

    std::ostream& operator<<(std::ostream&, const Statistics&);
}
