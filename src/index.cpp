
#include "index.hpp"
#include "list.h"
#include <cmath>

bool isPrime(relation_size_t);

Index::Index(const PsumTable::Result& _data)
:
_data(_data),
_chain(nullptr),
_bucket(nullptr),
_bucketSize(0U),
_hash([](const Tuple& tuple) { return 0U; })
{
    // Initialize _bucket:
    _bucketSize = _data.second;
    while (!isPrime(++_bucketSize));

    _bucket = new bucket_key_t[_bucketSize];
    for (bucket_size_t b = 0U; b < _bucketSize; b++)
        _bucket[b] = -1;

    _hash = [this](const Tuple& tuple)
    {
        return (tuple.payload > 0 ? tuple.payload : tuple.payload * -1) % _bucketSize;
    };

    // Initialize _chain:
    _chain = new chain_key_t[_data.second];
    for (chain_size_t c = 0U; c < _data.second; c++)
        _chain[c] = -1;

    // A lambda that is used to make a note of the specified tuple
    // It firstly attempts to insert it directly into the buck
    // In case of a collision, the tuple is appended to
    // the chain corresponding to the hash value of the tuple
    auto insert = [this](const Tuple& tuple)
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

    for (relation_size_t t = 0U; t < _data.second; t++)
        insert(_data.first[t]);
}

Index::~Index()
{
    delete[] _bucket;
    delete[] _chain;
}

bool isPrime(relation_size_t n)
{
    if (n % 2 == 0 || n % 3 == 0)
        return false;

    const relation_size_t root = static_cast<relation_size_t>(std::sqrt(n));

    for (relation_size_t i = 5U; i <= root; i += 6)
        if (n % i == 0)
           return false;

    for (relation_size_t i = 7U; i <= root; i += 6)
        if (n % i == 0)
           return false;

    return true;
}

nstd::List<Result> Index::join(const PsumTable::Result& other) const
{
    nstd::List<Result> results;
    for (relation_size_t t = 0U; t < other.second; t++)
    {
        const Tuple& tuple = other.first[t];

        chain_key_t index = _bucket[_hash(tuple)];
        do
        {
            if (tuple.payload == other.first[index].payload)
            {
                Result result(tuple.key, index);
                results.append(&result);
            }

        } while ((index = _chain[index]) >= 0);
    }

    return results;
}
