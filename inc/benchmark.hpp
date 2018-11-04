
#pragma once

#include <utility>
#include <chrono>
#include <iostream>

namespace utility
{
    template <typename Task, typename ...Args>
    auto benchmark(Task&& task, Args&&... args) -> typename std::result_of<Task(Args...)>::type
    {
        auto tbeg = std::chrono::high_resolution_clock::now();
        auto cbeg = std::clock();

        auto rv = task(std::forward<Args>(args)...);

        auto tend = std::chrono::high_resolution_clock::now();
        auto cend = std::clock();

        const double tdiff = static_cast<double>
        (
            std::chrono::duration_cast<std::chrono::microseconds>(tend - tbeg).count()
        ) / 1000.0;

        const double cdiff = cend - cbeg;

        std::cout << "milliseconds: " << tdiff << std::endl;
        std::cout << "clock ticks: "  << cdiff << std::endl;

        return rv;
    }
}
