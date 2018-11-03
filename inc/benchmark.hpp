
#pragma once

#include <utility>
#include <chrono>
#include <iostream>

namespace utility
{
    template <typename Task, typename ...Args>
    double benchmark(Task&& task, Args&&... args)
    {
        auto beg = std::chrono::high_resolution_clock::now();

        task(std::forward<Args>(args)...);

        auto end = std::chrono::high_resolution_clock::now();
        
        return static_cast<double>
        (
            std::chrono::duration_cast<std::chrono::microseconds>(end - beg).count()
        ) / 1000.0;
    }
}
