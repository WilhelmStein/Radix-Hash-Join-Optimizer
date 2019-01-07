
#pragma once

#include <utility>

class Statistics
{
    float min, max, size, unique; // l, u, f, d accordingly

public:

    Statistics()
    :
    min(0.0f), max(0.0f), size(0.0f), unique(0.0f) {}

    Statistics(float min, float max, float size, float unique)
    :
    min(min), max(max), size(size), unique(unique) {}

    Statistics(const Statistics& other)
    :
    min(other.min), max(other.max), size(other.size), unique(other.size) {}

    Statistics(Statistics&& other) noexcept
    :
    min(std::move(other.min)), max(std::move(other.max)),
    size(std::move(other.size)), unique(std::move(other.unique)) {}

    Statistics& operator=(const Statistics& other)
    {
        min = other.min; max = other.max; size = other.size; unique = other.unique;

        return *this;
    }

    Statistics& operator=(Statistics&& other) noexcept
    {
        min = std::move(other.min); max = std::move(other.max);

        size = std::move(other.size); unique = std::move(other.unique);

        return *this;
    }
};
