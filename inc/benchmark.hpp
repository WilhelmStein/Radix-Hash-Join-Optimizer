
#pragma once

#include <utility>
#include <type_traits>
#include <chrono>
#include <iostream>

namespace utility
{
    namespace detail
    {
        template <typename Task, typename ...Args>
        using result_of_t = typename std::result_of<Task(Args...)>::type;

        template <typename Task, typename ...Args>
        using is_void = std::is_void<result_of_t<Task, Args...>>;

        template <bool condition>
        using enable_if_t = typename std::enable_if<condition>::type;
    }

    template
    <
        typename Task,
        typename ...Args,
        typename = detail::enable_if_t<detail::is_void<Task, Args...>::value>
    >
    void benchmark(Task&& task, Args&&... args)
    {
        auto tbeg = std::chrono::high_resolution_clock::now();
        auto cbeg = std::clock();

        task(std::forward<Args>(args)...);

        auto tend = std::chrono::high_resolution_clock::now();
        auto cend = std::clock();

        const double tdiff = static_cast<double>
        (
            std::chrono::duration_cast<std::chrono::microseconds>(tend - tbeg).count()
        ) / 1000.0;

        const double cdiff = cend - cbeg;

        std::cout << "milliseconds: " << tdiff << std::endl;
        std::cout << "clock ticks: "  << cdiff << std::endl;
    }

    template
    <
        typename Task,
        typename ...Args,
        typename = detail::enable_if_t<!detail::is_void<Task, Args...>::value>
    >
    auto benchmark(Task&& task, Args&&... args) -> detail::result_of_t<Task, Args...> 
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
