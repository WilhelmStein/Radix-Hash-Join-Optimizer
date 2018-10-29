
#pragma once

#include <types.hpp>

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
                std::size_t index;
            
            public:

                static const std::size_t CAPACITY = 1024UL * 1024UL / sizeof(RHJ::Pair);

                RHJ::Pair data[CAPACITY];

                Buffer();

                std::size_t size() const { return index; }
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