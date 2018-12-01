
#pragma once

#include <types.hpp>
#include <list.hpp>
#include <iosfwd>           // std::ostream

#define CAPACITY (1024UL * 1024UL / sizeof(RHJ::Result))

namespace RHJ
{
    class Relation;
    
    struct Result
    {
        tuple_key_t key1;
        tuple_key_t key2;
    };

    class Buffer
    {
        friend struct Results;

        std::size_t _size;

        Result _data[CAPACITY];

    public:

        Buffer() : _size(0UL) {}

        std::size_t size() const { return _size; }
        const Result& operator[](std::size_t i) const { return _data[i]; }

    };

    struct Results : public utility::list<Buffer>
    {
        #if defined (__VERBOSE__)
            const Relation * left, * right;

            Results(const Relation&, const Relation&);
        #else
            Results();
        #endif
        
        Results(const Results&) = delete;
        Results(Results&&) noexcept;

        ~Results();

        Results& operator=(const Results&) = delete;
        Results& operator=(Results&&) noexcept;

        friend std::ostream& operator<<(std::ostream&, const Results&);

        void push_back(tuple_key_t, tuple_key_t);
    };

    std::ostream& operator<<(std::ostream&, const Results&);
}
