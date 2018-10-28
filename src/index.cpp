
#include <index.hpp>
#include <cmath>

bool isPrime(std::size_t);

RHJ::Index::Index(const RHJ::PsumTable::Bucket& _data)
:
_data(_data),
_bucket(nullptr),
_bucketSize(0U),
_chain(nullptr),
_hash()
{
    // Initialize _bucket:
    _bucketSize = _data.size;
    while (!isPrime(++_bucketSize));

    _bucket = new bucket_key_t[_bucketSize];
    for (std::size_t b = 0UL; b < _bucketSize; b++)
        _bucket[b] = -1;

    _hash = [this](const Relation::Tuple& tuple)
    {
        return std::abs(tuple.payload) % _bucketSize;
    };

    // Initialize _chain:
    _chain = new chain_key_t[_data.size];
    for (std::size_t c = 0UL; c < _data.size; c++)
        _chain[c] = -1;

    // A lambda that is used to make a note of the specified tuple
    // It firstly attempts to insert it directly into the bucket
    // In case of a collision, the tuple is appended to
    // the chain corresponding to the hash value of the tuple
    auto insert = [this](const Relation::Tuple& tuple)
    {
        bucket_key_t index = _hash(tuple);

        if (_bucket[index] < 0)
        {
            _bucket[index] = tuple.key;
            _chain[_bucket[index]] = -1;
            
            return;
        }

        index = _bucket[index];
        while (_chain[index] >= 0) index = _chain[index];

        _chain[index] = tuple.key; _chain[_chain[index]] = -1;
    };

    for (std::size_t t = 0UL; t < _data.size; t++)
        insert(_data.tuples[t]);
}

RHJ::Index::~Index()
{
    delete[] _bucket;
    delete[] _chain;
}

bool isPrime(std::size_t candidate)
{
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
    for (std::size_t t = 0UL; t < bucket.size; t++)
    {
        const Relation::Tuple& tuple = bucket.tuples[t];

        chain_key_t index = _bucket[_hash(tuple)];
        do
        {
            if (tuple.payload == _data.tuples[index].payload)
            {
                Pair pair = { tuple.key, index };
                results.append(pair);
            }

        } while ((index = _chain[index]) >= 0);
    }
}
