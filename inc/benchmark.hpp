
#pragma once

#include <utility>          // std::forward
#include <type_traits>      // std::result_of, std::is_void, ...
#include <chrono>           // std::chrono
#include <tuple>            // std::tuple

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
    auto benchmark(Task&& task, Args&&... args) -> std::tuple<double, std::clock_t>
    {
        auto tbeg = std::chrono::high_resolution_clock::now();
        auto cbeg = std::clock();

        task(std::forward<Args>(args)...);

        auto tend = std::chrono::high_resolution_clock::now();
        auto cend = std::clock();

        double ms = static_cast<double>
        (
            std::chrono::duration_cast<std::chrono::microseconds>(tend - tbeg).count()
        ) / 1000.0;

        std::clock_t ticks = cend - cbeg;

        return std::make_tuple(ms, ticks);
    }

    template
    <
        typename Task,
        typename ...Args,
        typename = detail::enable_if_t<!detail::is_void<Task, Args...>::value>
    >
    auto benchmark(Task&& task, Args&&... args) -> std::tuple<double, std::clock_t, detail::result_of_t<Task, Args...>>
    {
        auto tbeg = std::chrono::high_resolution_clock::now();
        auto cbeg = std::clock();

        auto rv = task(std::forward<Args>(args)...);

        auto tend = std::chrono::high_resolution_clock::now();
        auto cend = std::clock();

        double ms = static_cast<double>
        (
            std::chrono::duration_cast<std::chrono::microseconds>(tend - tbeg).count()
        ) / 1000.0;

        std::clock_t ticks = cend - cbeg;

        return std::make_tuple(ms, ticks, std::move(rv));
    }
}
