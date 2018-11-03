
#include <histhash.hpp>
#include <iostream>
#include <cstdlib>
#include <ctime>

#define MIN (0)
#define MAX (10)
#define RAND (MIN + std::rand() % (MAX - MIN + 1))

#define SIZE_R (16UL)
#define SIZE_S (8UL)

int main()
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    #if defined(__VERBOSE__)
        RHJ::Relation R(SIZE_R, "Slim");
        RHJ::Relation S(SIZE_S, "Shady");
    #else
        RHJ::Relation R(SIZE_R);
        RHJ::Relation S(SIZE_S);
    #endif

    for(std::size_t i = 0; i < SIZE_R; i++)
    {
        R.tuples[i].key = i;
        R.tuples[i].payload = RAND;
    }

    for(std::size_t i = 0; i < SIZE_S; i++)
    {
        S.tuples[i].key = i;
        S.tuples[i].payload = RAND;
    }
    
    RHJ::List results(RHJ::Relation::RadixHashJoin(R, S));
    
    std::cout << R << std::endl;
    std::cout << S << std::endl;

    std::cout << results << std::endl;

    return 0;
}
