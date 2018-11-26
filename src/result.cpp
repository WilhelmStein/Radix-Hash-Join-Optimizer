
#include <result.hpp>
#include <relation.hpp>
#include <utility>          // std::move
#include <fstream>          // std::ostream

#if defined (__VERBOSE__)
    RHJ::Results::Results(const RHJ::Relation& left, const RHJ::Relation& right)
    :
    head(new Node), tail(head), left(&left), right(&right)
    {
    }

    RHJ::Results::Results(Results&& other) noexcept
    :
    head(std::move(other.head)), tail(std::move(other.tail)), left(std::move(other.left)), right(std::move(other.right))
    {
        other.head = other.tail = nullptr;
    }

    RHJ::Results& RHJ::Results::operator=(Results&& other) noexcept
    {
        head  = std::move(other.head);
        tail  = std::move(other.tail);
        left  = std::move(other.left);
        right = std::move(other.right);

        other.head = other.tail = nullptr;

        return *this;
    }
#else
    RHJ::Results::Results()
    :
    head(new Node), tail(head), count(0)
    {
    }

    RHJ::Results::Results(Results&& other) noexcept
    :
    head(std::move(other.head)), tail(std::move(other.tail))
    {
        other.head = other.tail = nullptr;
    }

    RHJ::Results& RHJ::Results::operator=(Results&& other) noexcept
    {
        head = std::move(other.head);
        tail = std::move(other.tail);

        other.head = other.tail = nullptr;

        return *this;
    }
#endif

RHJ::Results::~Results()
{
    if (head)
        delete head;
}

void RHJ::Results::push_back(tuple_key_t key1, tuple_key_t key2)
{
    if (tail->buffer._size == CAPACITY)
        tail = tail->next = new Node;

    tail->buffer._data[tail->buffer._size++] = { key1, key2 };
    count++;
}

std::ostream& RHJ::operator<<(std::ostream& os, const RHJ::Results& results)
{
    std::size_t count = 0UL;
    for
    (
        const RHJ::Results::Node * current = results.head;
        current != nullptr;
        current = current->next
    )
    {
        #if defined (__VERBOSE__)
            os << "+----------+----------+----------+----------+" << std::endl;
            os << "|Key L     |Value L   |Key R     |Value R   |" << std::endl;
            os << "+----------+----------+----------+----------+" << std::endl;
            for (std::size_t i = 0UL; i < current->buffer.size(); i++, count++)
            {
                const std::size_t rowR = current->buffer[i].key1;
                const std::size_t rowS = current->buffer[i].key2;

                os << "|" << results.left->tuples[rowR]  << "|" << results.right->tuples[rowS] << "|" << std::endl;
            }
            os << "+----------+----------+----------+----------+" << std::endl;

            os
            << "\n[L=\""    << results.left->name
            << "\", R=\"" << results.right->name
            << "\"]: ";
        #else
            count += current->buffer.size();
        #endif
    }

    os << count << " results...";

    return os;
}
