
#include <histhash.hpp>
#include <cstdlib>
#include <ctime>
#include <iostream>

#define SIZE_R  (1024UL * 1024UL / sizeof(RHJ::PsumTable::Bucket))
#define SIZE_S  (1024UL * 1024UL / sizeof(RHJ::PsumTable::Bucket))

#if defined(__RANDOM_VALUES__)
    #define MIN   (0)
    #define MAX   (1000)
    #define VALUE (MIN + std::rand() % (MAX - MIN + 1))
#else
    #define VALUE (1)
#endif

#if defined(__ODD_NUMBERS__)
    #define PAYLOAD (2 * VALUE + 1)
#elif defined(__EVEN_NNUMBERS__)
    #define PAYLOAD (2 * VALUE)
#else
    #define PAYLOAD VALUE
#endif

int main()
{
    #if defined(__RANDOM_VALUES__)
        std::srand(static_cast<unsigned>(std::time(nullptr)));
    #endif

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
        R.tuples[i].payload = PAYLOAD;
    }

    for(std::size_t i = 0; i < SIZE_S; i++)
    {
        S.tuples[i].key = i;
        S.tuples[i].payload = PAYLOAD;
    }
    
    RHJ::List results(RHJ::Relation::RadixHashJoin(R, S));

    return 0;
}
