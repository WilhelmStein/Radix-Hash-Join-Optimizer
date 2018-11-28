
#include <query.hpp>
#include <iostream>
#include <executioner.hpp>
#include <vector>
#include <string>

int main()
{
    char q[] = "0 1 2 4|0.1=2.2&1.0=3.1|2.2 1.1";

    // std::cout << q << std::endl << RHJ::Query(q) << std::endl;

    RHJ::Query query = RHJ::Query(q);

    RHJ::Executioner demios;


    std::vector<std::string> checkSums = demios.execute(query);

    for (auto &i : checkSums)
        std::cout << i << std::endl;

    return 0;
}
