
#pragma once

#include <types.hpp>
#include <histhash.hpp>
#include <result.hpp>

namespace RHJ 
{
    class Index
    {
        const PsumTable::Bucket& _data;

        bucket_key_t * _bucket;
        std::size_t _bucketSize;

        chain_key_t * _chain;

    public:

        Index(const PsumTable::Bucket&);
        ~Index();

        void join(const PsumTable::Bucket&, RHJ::List&) const;
    };
}
