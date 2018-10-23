
#include <fstream>

template <typename First, typename Second>
pair<First, Second>::pair()
:
first(), second()
{
}

template <typename First, typename Second>
pair<First, Second>::pair(const First& first, const Second& second)
:
first(first), second(second)
{
}

template <typename First, typename Second>
pair<First, Second>::pair(const pair<First, Second>& other)
:
first(other.first), second(other.second)
{
}

template <typename First, typename Second>
pair<First, Second>& pair<First, Second>::operator=(const pair<First, Second>& other)
{
    first  = other.first;
    second = other.second;

    return *this;
}

template <typename First, typename Second>
bool operator<(const pair<First, Second>& A, const pair<First, Second>& B)
{
    return A.first < B.first || (A.first == B.first && A.second < B.second);
}

template <typename First, typename Second>
bool operator>(const pair<First, Second>& A, const pair<First, Second>& B)
{
    return B < A;
}

template <typename First, typename Second>
bool operator==(const pair<First, Second>& A, const pair<First, Second>& B)
{
    return !(A < B) && !(B < A);
}

template <typename First, typename Second>
bool operator!=(const pair<First, Second>& A, const pair<First, Second>& B)
{
    return !(A == B);
}

template <typename First, typename Second>
std::ostream& operator<<(std::ostream& os, const pair<First, Second>& p)
{
    os << p.first << ' ' << p.second;

    return os;
}
