
#include <query.hpp>
#include <iostream>

int main()
{
    char q[] = "0 2 4|0.1=1.2&1.0=2.1&0.1>3000|2.2 1.1";

    std::cout << q << std::endl << RHJ::Query(q) << std::endl;

    return 0;
}
