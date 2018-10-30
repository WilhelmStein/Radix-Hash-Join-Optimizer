
#include <index.hpp>
#include <cmath>
#include <functional>

#define HASH(tuple, _bucketSize) (std::hash<tuple_payload_t>{}(tuple.payload) % _bucketSize)

bool isPrime(std::size_t);

RHJ::Index::Index(const RHJ::PsumTable::Bucket& _data)
:
_data(_data),
_bucket(nullptr),
_bucketSize(0U),
_chain(nullptr)
{
    // Initialize _bucket:
    _bucketSize = _data.size;
    while (!isPrime(++_bucketSize));

    _bucket = new bucket_key_t[_bucketSize];
    for (std::size_t b = 0UL; b < _bucketSize; b++)
        _bucket[b] = -1;

    // Initialize _chain:
    _chain = new chain_key_t[_data.size];
    for (std::size_t c = 0UL; c < _data.size; c++)
        _chain[c] = -1;

    for (std::size_t row = 0UL; row < _data.size; row++)
    {
        const Relation::Tuple& tuple = _data.tuples[row];

        bucket_key_t index = HASH(tuple, _bucketSize);

        if (_bucket[index] < 0)
        {
            _bucket[index] = row; return;
        }

        index = _bucket[index];
        while (_chain[index] >= 0) index = _chain[index];

        _chain[index] = row;
    }
}

RHJ::Index::~Index()
{
    delete[] _bucket;
    delete[] _chain;
}

bool isPrime(std::size_t candidate)
{
    if (candidate < 2UL)
        return false;

    if (candidate == 2UL || candidate == 3UL)
        return true;

    if (candidate % 2UL == 0UL || candidate % 3UL == 0UL)
        return false;

    const std::size_t root = static_cast<std::size_t>(std::sqrt(candidate));

    for (std::size_t i = 5UL; i <= root; i += 6UL)
        if (candidate % i == 0UL)
           return false;

    for (std::size_t i = 7UL; i <= root; i += 6UL)
        if (candidate % i == 0UL)
           return false;

    return true;
}

void RHJ::Index::join(const RHJ::PsumTable::Bucket& bucket, RHJ::List& results) const
{
    for (std::size_t row = 0UL; row < bucket.size; row++)
    {
        const Relation::Tuple& tuple = bucket.tuples[row];

        for
        (
            bucket_key_t index = _bucket[HASH(tuple, _bucketSize)];
            index >= 0;
            index = _chain[index]
        )
        {
            if (tuple.payload == _data.tuples[index].payload)
            {
                List::Result pair = { tuple.key, _data.tuples[index].key };
                results.append(pair);
            }
        }
    }
}
