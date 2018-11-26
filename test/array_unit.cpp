
#include <array.hpp>
#include <fstream>
#include <iostream>

struct Point
{
    std::size_t x, y;

    Point() : x(0UL), y(0UL) {}
    Point(std::size_t x, std::size_t y) : x(x), y(y) {}
    
    Point(const Point&) = delete;
    Point(Point&& other) : x(std::move(other.x)), y(std::move(other.y)) {}

    Point& operator=(const Point&) = delete;
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
    utility::array<Point> points(10UL);
    
    for (std::size_t i = 0UL; i < points.size(); i++)
        points[i] = Point(i, i);

    for (const auto& point : points)
        std::cout << point << std::endl;

    std::cout << std::endl;

    for (utility::array<Point>::iterator it = points.begin(); it != points.end(); ++it)
        *it = Point(0UL, 0UL);

    for (const auto& point : points)
        std::cout << point << std::endl;

    std::cout << std::endl;
}
