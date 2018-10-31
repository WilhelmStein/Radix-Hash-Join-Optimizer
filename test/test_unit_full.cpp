/*
    Full Test
*/

//              Debugging Options

#define RANDOM_VALUES   ////////////////////////
                            // Choose Either //
#define SAME_VALUES    ///////////////////////

#define ODD_NUMBERS     ////////////////////////
                            // Choose Either //
//#define EVEN_NUMBERS   ///////////////////////

#define SIZE_R  (16UL)
#define SIZE_S  (8UL)

//

#include <histhash.hpp>
#include <cstdlib>
#include <ctime>
#include <iostream>


using std::cout;
using std::endl;

#define MIN   (0UL)
#define MAX   (100UL)

#ifdef ODD_NUMBERS
    #define RAND ( 2 * (MIN + std::rand() % (MAX - MIN + 1)) + 1 )
#elif defined EVEN_NUMBERS
    #define RAND ( 2 * (MIN + std::rand() % (MAX - MIN + 1)) )
#else
    #define RAND (MIN + std::rand() % (MAX - MIN + 1))
#endif


int main(void)
{
    #ifdef RANDOM_VALUES
        std::srand(static_cast<unsigned>(std::time(nullptr)));
    #elif defined SAME_VALUES
        std::srand(NULL);
    #endif

    // Create Relation Tables
    RHJ::Relation R(SIZE_R);
    RHJ::Relation S(SIZE_S);

    for(unsigned long i = 0; i < SIZE_R; i++) {
        R.tuples[i].key = i;
        R.tuples[i].payload = RAND;
    }

    for(unsigned long i = 0; i < SIZE_S; i++) {
        S.tuples[i].key = i;
        S.tuples[i].payload = RAND;
    }
    
    RHJ::List results( RHJ::Relation::RadixHashJoin(R, S) );
    
    return 0;
}