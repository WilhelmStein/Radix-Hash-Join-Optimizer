
#include "index.hpp"

#define SIZE 10

int main()
{
    Bucket bucket;

    Tuple * tuples[SIZE] = { nullptr };
    for (tuple_key_t i = 0; i < SIZE; i++)
    {
        tuples[i] = new Tuple(i, i * 2);
        bucket.insert(tuples[i]);
    }

    Index index(bucket);

    index.join(bucket);
}
