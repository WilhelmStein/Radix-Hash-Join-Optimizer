
/*
    Testing RHJ::PsumTable
*/

#include <histhash.hpp>
#include <cstdlib>
#include <ctime>

#define MIN (0)
#define MAX (100)
#define RAND (MIN + std::rand() % (MAX - MIN + 1))

#define SIZE (10UL)

int main()
{
    RHJ::Relation R(SIZE);
    for (std::size_t row = 0UL; row < R.size; row++)
    {
        R.tuples[row].key     = row + 1UL;
        R.tuples[row].payload = RAND;
    }

    RHJ::PsumTable table(R, 3UL);

    return 0;
}
