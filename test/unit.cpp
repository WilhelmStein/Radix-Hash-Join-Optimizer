
#include <histhash.hpp>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <benchmark.hpp>

#if defined(__VERBOSE__)
    #if defined(__SILENCE__)
        #undef __SILENCE__
    #endif
#endif

#if defined(__LARGE__) || defined(__MEDIUM__)
    #if defined(__SAME__)
        #undef __SAME__
    #endif
#endif

#if defined(__LARGE__)
    #define SIZE_R (1024UL * 1024UL * 1024UL / sizeof(RHJ::PsumTable::Bucket))
    #define SIZE_S (1024UL * 1024UL * 1024UL / sizeof(RHJ::PsumTable::Bucket))

    #if !defined(__DIFFERENT__)
        #define __DIFFERENT__
    #endif
#elif defined(__MEDIUM__)
    #define SIZE_R (64UL  * 1024UL * 1024UL / sizeof(RHJ::PsumTable::Bucket))
    #define SIZE_S (256UL * 1024UL * 1024UL / sizeof(RHJ::PsumTable::Bucket))
#else
    #define SIZE_R (16UL)
    #define SIZE_S (8UL)
#endif

#if defined(__DIFFERENT__)
    #define PAYLOAD_R (1)
    #define PAYLOAD_S (PAYLOAD_R + 1)
#elif defined(__SAME__)
    #define PAYLOAD_R (1)
    #define PAYLOAD_S PAYLOAD_R
#else
    #if defined(__MEDIUM__)
        #define MIN (0)
        #define MAX (256 * 1024)
    #else
        #define MIN (0)
        #define MAX (5)
    #endif

    #define PAYLOAD_R (MIN + std::rand() % (MAX - MIN + 1))
    #define PAYLOAD_S PAYLOAD_R
#endif

int main()
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
    
    #if defined(__BENCHMARK__)
        #if defined(__LARGE__)
            std::cout << "\ncase: __LARGE__"  << std::endl;
        #elif defined(__MEDIUM__)
            std::cout << "\ncase: __MEDIUM__" << std::endl;
        #else
            std::cout << "\ncase: __SMALL__" << std::endl;
        #endif
        
        std::cout << "cache-kb: " << __CACHE_SIZE__ / 1024UL << std::endl;

        RHJ::List results(utility::benchmark(RHJ::Relation::RadixHashJoin, R, S));
    #else
        RHJ::List results(RHJ::Relation::RadixHashJoin(R, S));
    #endif

    #if !defined(__SILENCE__)
        std::cout << R << std::endl;
        std::cout << S << std::endl;
        
        std::cout << results << std::endl;
    #endif

    return 0;
}
