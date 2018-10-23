
#pragma once

#include "histhash.hpp"
#include "types.hpp"
#include "pair.hpp"
#include "rdxl.hpp"
#include <functional>

namespace RHJ 
{
    class Index
    {
        const PsumTable::Bucket& _data;
        
        chain_key_t * _chain;

        bucket_key_t * _bucket;
        std::size_t _bucketSize;

        std::function<bucket_key_t(const Tuple&)> _hash;

    public:

        Index(const PsumTable::Bucket&);

        ~Index();

        const PsumTable::Bucket& data() const { return _data; }

        const chain_key_t * chain() const  { return _chain; }

        const bucket_key_t * bucket() const { return _bucket; }
        std::size_t bucketSize() const { return _bucketSize; }

        const std::function<bucket_key_t(const Tuple&)>& hash() const { return _hash; }

        void join(const PsumTable::Bucket&, RHJ::List&) const;
    };
}
