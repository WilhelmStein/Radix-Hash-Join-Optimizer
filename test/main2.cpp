
#include <relation.hpp>
#include <cstdlib>
#include <iostream>
#include <iomanip>

#define SIZE  (1024UL * 1024UL / sizeof(RHJ::List::Result) * 16UL)

#define MIN   (0UL)
#define MAX   (100UL)
#define WIDTH (3)

#define RAND(MIN, MAX) (MIN + std::rand() % (MAX - MIN + 1))

int main()
{
    RHJ::Relation R(new RHJ::Relation::Tuple[SIZE], SIZE);
    RHJ::Relation S(new RHJ::Relation::Tuple[SIZE], SIZE);
    
    for (std::size_t i = 0UL; i < SIZE; i++)
    {
        R.tuples[i].key = i; R.tuples[i].payload = RAND(MIN, MAX);
        S.tuples[i].key = i; S.tuples[i].payload = RAND(MIN, MAX);
    }

    RHJ::List results = RHJ::Relation::RadixHashJoin(R, S);

    for
    (
        const RHJ::List::Node * current = results.head;
        current != nullptr;
        current = current->next
    )
    {

        for (std::size_t i = 0UL; i < current->buffer.size(); i++)
            std::cout
            << std::setw(WIDTH) << std::setfill('0') << current->buffer[i].key1
            << ' '
            << std::setw(WIDTH) << std::setfill('0') << current->buffer[i].key2
            << std::endl;
    }

    return 0;
}
