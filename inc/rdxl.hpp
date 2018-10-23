
#pragma once

#include "relation.h"

struct RDXList
{
    struct RDXNode
    {
        class Buffer
        {
        public:
            uint32_t index;
        
        public:

            static const uint32_t CAPACITY = 1024UL * 1024UL / sizeof(Result);

            Result data[CAPACITY];

            Buffer();

            bool full() const;
            void append(const Result&);
        } buffer;

        RDXNode * next;

        RDXNode();
        ~RDXNode();
    } * head, * tail;

    RDXList();
    ~RDXList();

    void append(const Result&);
};
