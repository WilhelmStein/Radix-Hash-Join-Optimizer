
#pragma once

#include "relation.hpp"

namespace RHJ
{
    struct List
    {
        struct Node
        {
            class Buffer
            {
            public:
                std::size_t index;
            
            public:

                static const std::size_t CAPACITY = 1024UL * 1024UL / sizeof(RHJ::Result);

                RHJ::Result data[CAPACITY];

                Buffer();

                bool full() const;
                void append(const RHJ::Result&);
            } buffer;

            Node * next;

            Node();
            ~Node();
        } * head, * tail;

        List();
        ~List();

        void append(const RHJ::Result&);
    };
}
