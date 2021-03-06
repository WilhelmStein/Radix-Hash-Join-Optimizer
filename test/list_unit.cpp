
#include <list.hpp>
#include <fstream>
#include <iostream>

struct Point
{
    std::size_t x, y;

    Point(std::size_t x, std::size_t y) : x(x), y(y) {}
    
    Point(const Point&) = delete;
    Point(Point&& other) : x(std::move(other.x)), y(std::move(other.y)) {}

    friend bool operator==(const Point& A, const Point& B)
    {
        return A.x == B.x && A.y == B.y;
    }
};

static std::ostream& operator<<(std::ostream& os, const Point& point)
{
    return os << '[' << point.x << ", " << point.y << ']';
}

int main()
{
    utility::list<Point> points;
    for (std::size_t i = 0UL; i < 5UL; i++)
        points.emplace_back(i, i);

    for (const auto& point : points)
        std::cout << point << std::endl;

    std::cout << std::endl;

    utility::list<Point>::iterator it;
    
    it = points.erase(points.find(Point(4UL, 4UL)));

    for (const auto& point : points)
        std::cout << point << std::endl;

    std::cout << std::endl;

    it = points.erase(points.find(Point(3UL, 3UL)));

    for (const auto& point : points)
        std::cout << point << std::endl;

    std::cout << std::endl;

    it = points.find(Point(5UL, 5UL));
    if (it != points.end())
        points.erase(it);

    for (utility::list<Point>::iterator it = points.begin(); it != points.end(); ++it)
        std::cout << *it << std::endl;

    std::cout << "\n" << points.front() << ' ' << points.back() << std::endl;

    std::cout << std::endl;

    for (utility::list<Point>::iterator it = points.begin(); it != points.end(); it = points.erase(it))
    {
        for (const auto& point : points)
            std::cout << point << std::endl;

        std::cout << std::endl;
    }

    std::cout << std::boolalpha << points.empty() << std::endl;

    for (std::size_t i = 0UL; i < 5UL; i++)
        points.emplace_back(i, i);

    points.clear();

    while (!points.empty())
    {
        std::cout << points.back() << std::endl; points.pop_back();
    }

    std::cout << std::boolalpha << points.empty() << std::endl;
    
    for (std::size_t i = 0UL; i < 5UL; i++)
        points.push_front(Point(i, i));

    for (const auto& point : points)
        std::cout << point << std::endl;

    std::cout << std::endl;

    while (!points.empty())
    {
        std::cout << points.front() << std::endl; points.pop_front();
    }
}
