
#include <index.hpp>
#include <cmath>                        // std::sqrt

#define HASH(key)                       \
do {                                    \
    key = ~key + (key << 15);           \
    key = key ^ (key >> 12);            \
    key = key + (key << 2);             \
    key = key ^ (key >> 4);             \
    key = key * 2057;                   \
    key = key ^ (key >> 16);            \
} while(false)                          \

#define BOUNDED_HASH(key, denominator)  \
do {                                    \
    HASH(key);                          \
    key = key % denominator;            \
} while(false)                          \

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

        bucket_key_t index = tuple.payload;
        BOUNDED_HASH(index, _bucketSize);

        if (_bucket[index] < 0)
        {
            _bucket[index] = row; continue;
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

void RHJ::Index::join(const RHJ::PsumTable::Bucket& bucket, RHJ::Results& results, const RHJ::PsumTable& lpsum) const
{
    std::uintptr_t min = reinterpret_cast<std::uintptr_t>(lpsum.table.tuples);
    std::uintptr_t max = reinterpret_cast<std::uintptr_t>(&lpsum.table.tuples[lpsum.table.size - 1UL]);

    for (std::size_t row = 0UL; row < bucket.size; row++)
    {
        const Relation::Tuple& tuple = bucket.tuples[row];

        bucket_key_t index = tuple.payload;
        BOUNDED_HASH(index, _bucketSize);

        for (index = _bucket[index]; index >= 0; index = _chain[index])
        {
            if (tuple.payload == _data.tuples[index].payload)
            {
                std::uintptr_t offset = reinterpret_cast<std::uintptr_t>(&tuple);

                if (min <= offset && offset <= max)
                    results.push_back(tuple.key, _data.tuples[index].key);
                else
                    results.push_back(_data.tuples[index].key, tuple.key);
            }
        }
    }
}
