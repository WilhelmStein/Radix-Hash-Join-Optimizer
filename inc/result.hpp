
#pragma once

#include <types.hpp>
#include <iosfwd>

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

        friend std::ostream& operator<<(std::ostream&, const List&);

        void append(const Result&);
    };

    std::ostream& operator<<(std::ostream&, const List&);
}
