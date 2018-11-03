
#include <histhash.hpp>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <benchmark.hpp>

#if !defined(__VERBOSE__)
    #if !defined(__DIFFERENT__)
        #define __DIFFERENT__
    #endif

    #define SIZE_R (512UL  * 1024UL * 1024UL / sizeof(RHJ::PsumTable::Bucket))
    #define SIZE_S (1024UL * 1024UL * 1024UL / sizeof(RHJ::PsumTable::Bucket))
#else
    #define SIZE_R (16UL)
    #define SIZE_S (8UL)
#endif

#if defined(__RANDOM__)
    #define MIN (0)
    #define MAX (5)
    #define VALUE (MIN + std::rand() % (MAX - MIN + 1))
#else
    #define VALUE (1)
#endif

#if defined(__DIFFERENT__)
    #define PAYLOAD_R (2 * VALUE + 1)
    #define PAYLOAD_S (2 * VALUE)
#else
    #define PAYLOAD_R VALUE
    #define PAYLOAD_S VALUE
#endif

int main()
{
    double diff = utility::benchmark([]()
    {
        #if defined(__RANDOM__)
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
            R.tuples[i].payload = PAYLOAD_R;
        }

        for(std::size_t i = 0; i < SIZE_S; i++)
        {
            S.tuples[i].key = i;
            S.tuples[i].payload = PAYLOAD_S;
        }
        
        RHJ::List results(RHJ::Relation::RadixHashJoin(R, S));

        std::cout << R << std::endl;
        std::cout << S << std::endl;
        
        std::cout << results << std::endl;
    });

    std::cout << "<MSG>: " << diff << " milliseconds elapsed" << std::endl;

    return 0;
}
