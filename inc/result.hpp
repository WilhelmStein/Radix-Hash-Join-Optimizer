
#pragma once

#include <types.hpp>

#if defined(__ENABLE_PRINTING_LIST__)    
    #include <iosfwd>
#endif

#define CAPACITY (1024UL * 1024UL / sizeof(Result))

namespace RHJ
{
    class Relation;
    
    struct List
    {
        const Relation& left, & right;

        struct Result {
            tuple_key_t key1;
            tuple_key_t key2;
        };

        struct Node
        {
            class Buffer
            {
                friend List;

                std::size_t _size;
                Result _data[CAPACITY];

            public:

                Buffer();

                std::size_t size() const { return _size; }
                const Result& operator[](std::size_t i) const { return _data[i]; }
            } buffer;

            Node * next;

            Node();
            ~Node();
        } * head, * tail;

        List(const Relation&, const Relation&);
        List(List&&) noexcept;

        ~List();

        #if defined(__ENABLE_PRINTING_LIST__)
            friend std::ostream& operator<<(std::ostream&, const List&);
        #endif

        void append(const Result&);
    };

    #if defined(__ENABLE_PRINTING_LIST__)
        std::ostream& operator<<(std::ostream&, const List&);
    #endif
}
