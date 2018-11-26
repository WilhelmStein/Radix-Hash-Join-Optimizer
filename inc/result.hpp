
#pragma once

#include <types.hpp>
#include <iosfwd>           // std::ostream

#define CAPACITY (1024UL * 1024UL / sizeof(RHJ::Results::Result))

namespace RHJ
{
    class Relation;
    
    struct Results
    {
        friend class Executioner;

        struct Result {
            tuple_key_t key1;
            tuple_key_t key2;
        };

        struct Node
        {
            class Buffer
            {
                friend struct Results;

                std::size_t _size;

                Result _data[CAPACITY];

            public:

                Buffer() : _size(0UL) {}

                std::size_t size() const { return _size; }
                const Result& operator[](std::size_t i) const { return _data[i]; }

            } buffer;

            Node * next;

            Node() : next(nullptr) {}
            ~Node() { if (next) delete next; }

        } * head, * tail;

        std::size_t count;

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
