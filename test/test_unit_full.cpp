/*
    Full Test
*/

#include <histhash.hpp>
#include <cstdlib>
#include <ctime>

#if defined(__ENABLE_PRINTING_LIST__)
    #include <iostream>
#endif

#define SIZE_R  (64UL)// * 1024UL * 1024UL / sizeof(RHJ::PsumTable::Bucket))
#define SIZE_S  (32UL)// * 1024UL * 1024UL / sizeof(RHJ::PsumTable::Bucket))

#if defined(__RANDOM_VALUES__)
    #define MIN   (0)
    #define MAX   (100)
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

    RHJ::Relation R(SIZE_R);

    for(std::size_t i = 0; i < SIZE_R; i++)
    {
        R.tuples[i].key = i;
        R.tuples[i].payload = PAYLOAD;
    }

    RHJ::Relation S(SIZE_S);

    for(std::size_t i = 0; i < SIZE_S; i++)
    {
        S.tuples[i].key = i;
        S.tuples[i].payload = PAYLOAD;
    }
    
    RHJ::List results(RHJ::Relation::RadixHashJoin(R, S));
    
    #if defined(__ENABLE_PRINTING_LIST__)
        std::cout << "Left Relation\n" << std::endl;
        std::cout << R << std::endl;
        
        std::cout << "Right Relation\n" << std::endl;
        std::cout << S << std::endl;
        std::cout << results << std::endl;
    #endif

    return 0;
}
