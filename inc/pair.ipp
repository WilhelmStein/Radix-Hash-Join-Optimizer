
template <typename First, typename Second>
nstd::pair<First, Second>::pair(const First& first, const Second& second)
:
first(first), second(second)
{
}

template <typename First, typename Second>
nstd::pair<First, Second>::pair(const nstd::pair<First, Second>& other)
:
first(other.first), second(other.second)
{
}

template <typename First, typename Second>
nstd::pair<First, Second>& nstd::pair<First, Second>::operator=(const nstd::pair<First, Second>& other)
{
    first  = other.first;
    second = other.second;

    return *this;
}

template <typename First, typename Second>
inline bool operator<(const nstd::pair<First, Second>& A, const nstd::pair<First, Second>& B)
{
    return A.first < B.first || (A.first == B.first && A.second < B.second);
}

template <typename First, typename Second>
inline bool operator>(const nstd::pair<First, Second>& A, const nstd::pair<First, Second>& B)
{
    return B < A;
}

template <typename First, typename Second>
inline bool operator==(const nstd::pair<First, Second>& A, const nstd::pair<First, Second>& B)
{
    return !(A < B) && !(B < A);
}

template <typename First, typename Second>
inline bool operator!=(const nstd::pair<First, Second>& A, const nstd::pair<First, Second>& B)
{
    return !(A == B);
}
