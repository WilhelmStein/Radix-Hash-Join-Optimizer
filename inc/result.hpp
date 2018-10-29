
#pragma once

#include <types.hpp>

#define CAPACITY (1024UL * 1024UL / sizeof(RHJ::Pair))

namespace RHJ
{
    struct Pair {
        tuple_key_t key1;
        tuple_key_t key2;
    };

    struct List
    {
        struct Node
        {
            class Buffer
            {
                std::size_t _size;
                RHJ::Pair _data[CAPACITY];
                
            public:

                Buffer();

                std::size_t size() const { return _size; }
                const RHJ::Pair& operator[](std::size_t i) const { return _data[i]; }

                bool full() const;
                void append(const RHJ::Pair&);
            } buffer;

            Node * next;

            Node();
            ~Node();
        } * head, * tail;

        List();
        ~List();

        void append(const RHJ::Pair&);
    };
}
