/*
    Testing RHJ::Relation::RadixHashJoin
*/

#include <relation.hpp>
#include <histhash.hpp>
#include <cstdlib>
#include <ctime>

#if defined(__ENABLE_PRINTING_LIST__) || defined(__ENABLE_PRINTING_RELATION__)
    #include <iostream>
#endif

#define SIZE  (16UL * 1024UL * 1024UL / sizeof(RHJ::PsumTable::Bucket))

#define MIN   (0)
#define MAX   (100)

#define RAND(MIN, MAX) (MIN + std::rand() % (MAX - MIN + 1))

int main()
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    #if defined(__ENABLE_PRINTING_RELATION__)
        RHJ::Relation R(SIZE, "Slim");
        RHJ::Relation S(SIZE, "Shady");
    #else
        RHJ::Relation R(SIZE);
        RHJ::Relation S(SIZE);
    #endif

    for (std::size_t i = 0UL; i < SIZE; i++)
    {
        R.tuples[i].key = i; R.tuples[i].payload = RAND(MIN, MAX);
        S.tuples[i].key = i; S.tuples[i].payload = RAND(MIN, MAX);
    }

    RHJ::List results = RHJ::Relation::RadixHashJoin(R, S);

    #if defined(__ENABLE_PRINTING_RELATION__)
        std::cout << R << std::endl;
        std::cout << S << std::endl;
    #endif

    #if defined(__ENABLE_PRINTING_LIST__)
        std::cout << results << std::endl;
    #endif

    return 0;
}
