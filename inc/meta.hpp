
#pragma once

#include <types.hpp>

namespace RHJ
{
    extern struct Meta
    {
        static std::size_t total;

        __off_t mappingSize;
        void * mapping;

        tuple_key_t rowSize, columnSize;
        tuple_payload_t ** columns;

        static void load();
        static void dump();

    } * meta;
}
