
#include <array.hpp>
#include <fstream>
#include <iostream>

#include <cstdlib>
#include <ctime>

#define frand01 (static_cast<double>(std::rand()) / static_cast<double>(RAND_MAX))
#define frrange(min, max) ((max - min) * frand01 + min)

struct Point
{
    std::size_t x, y;

    Point() : x(0UL), y(0UL) {}
    Point(std::size_t x, std::size_t y) : x(x), y(y) {}
    
    Point(const Point&) = delete;
    Point(Point&& other) : x(std::move(other.x)), y(std::move(other.y)) {}

    Point& operator=(const Point& other)
    {
        x = other.x;
        y = other.y;

        return *this;
    }

    Point& operator=(Point&& other) noexcept
    {
        x = std::move(other.x);
        y = std::move(other.y);

        return *this;
    }
};

static std::ostream& operator<<(std::ostream& os, const Point& point)
{
    return os << '[' << point.x << ", " << point.y << ']';
}

int main()
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    utility::array<Point, 10UL> points;
    
    for (std::size_t i = 0UL; i < points.size(); i++)
        points[i] = Point(frrange(1UL, 10UL), frrange(1, 10UL));

    for (const auto& point : points)
        std::cout << point << std::endl;

    std::cout << std::endl;

    for (const auto& point : utility::array<Point, 10UL>(points))
        std::cout << "copied " << point << std::endl;

    std::cout << std::endl;

    for (auto it = points.begin(); it != points.end(); ++it)
        *it = Point(frrange(1UL, 10UL), frrange(1, 10UL));

    for (const auto& point : points)
        std::cout << point << std::endl;

    std::cout << std::endl;

    std::cout << points.front() << std::endl;
    std::cout << points.back() << std::endl;
}
