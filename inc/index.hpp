
#pragma once

#include "histhash.h"
#include "types.hpp"
#include "pair.hpp"
#include <functional>

class Index
{
    const Bucket& _data;
    
    chain_key_t * _chain;

    bucket_key_t * _bucket;
    bucket_size_t _bucketSize;

    std::function<bucket_key_t(const Tuple&)> _hash;

public:

    using Result = ::pair<relation_size_t, relation_size_t>;

    Index(const Bucket&);
    ~Index();

    const Bucket& data() const { return _data; }

    const chain_key_t * chain() const  { return _chain; }

    const bucket_key_t * bucket() const { return _bucket; }
    bucket_size_t bucketSize() const { return _bucketSize; }

    const std::function<bucket_key_t(const Tuple&)>& hash() const { return _hash; }

    nstd::List<Index::Result> join(const Bucket&) const;
};
