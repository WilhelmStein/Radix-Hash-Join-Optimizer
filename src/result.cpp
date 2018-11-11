
#include <result.hpp>
#include <relation.hpp>
#include <utility>          // std::move
#include <fstream>          // std::ostream

RHJ::List::Node::Buffer::Buffer()
:
_size(0UL)
{
}

RHJ::List::Node::Node()
:
next(nullptr)
{
}

RHJ::List::Node::~Node()
{
    if (next)
        delete next;
}

#if defined (__VERBOSE__)
    RHJ::List::List(const RHJ::Relation& left, const RHJ::Relation& right)
    :
    head(new RHJ::List::Node()), tail(head), left(left), right(right)
    {
    }
#else
    RHJ::List::List()
    :
    head(new RHJ::List::Node()), tail(head)
    {
    }
#endif

#if defined (__VERBOSE__)
    RHJ::List::List(List&& other) noexcept
    :
    head(std::move(other.head)), tail(std::move(other.tail)), left(std::move(other.left)), right(std::move(other.right))
    {
    }
#else
    RHJ::List::List(List&& other) noexcept
    :
    head(std::move(other.head)), tail(std::move(other.tail))
    {
    }
#endif

RHJ::List::~List()
{
    delete head;
}

void RHJ::List::append(const RHJ::List::Result& result)
{
    if (tail->buffer._size == CAPACITY)
        tail = tail->next = new Node;

    tail->buffer._data[tail->buffer._size++] = result;
}

std::ostream& RHJ::operator<<(std::ostream& os, const RHJ::List& results)
{
    std::size_t count = 0UL;
    for
    (
        const RHJ::List::Node * current = results.head;
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

                os << "|" << results.left.tuples[rowR]  << "|" << results.right.tuples[rowS] << "|" << std::endl;
            }
            os << "+----------+----------+----------+----------+" << std::endl;

            os
            << "\n[L=\""    << results.left.name
            << "\", R=\"" << results.right.name
            << "\"]: ";
        #else
            count += current->buffer.size();
        #endif
    }

    os << count << " results...";

    return os;
}
