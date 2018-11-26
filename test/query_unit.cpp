
#include <query.hpp>
#include <iostream>
#include <executioner.hpp>

int main()
{
    char q[] = "0 2 4|0.1=1.2&1.0=2.1&0.1>3000&0.0<6000|2.2 1.1";

    // std::cout << q << std::endl << RHJ::Query(q) << std::endl;

    RHJ::Query query = RHJ::Query(q);

    RHJ::Executioner *demios = new RHJ::Executioner();

    demios->execute(query);

    return 0;
}
