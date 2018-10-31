
#include <result.hpp>
#include <relation.hpp>
#include <utility>

#if defined(__ENABLE_PRINTING_LIST__)
    #include <fstream>
#endif

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

RHJ::List::List(const RHJ::Relation& left, const RHJ::Relation& right)
:
left(left), right(right), head(new RHJ::List::Node()), tail(head)
{
}

RHJ::List::List(List&& other) noexcept
:
left(std::move(other.left)), right(std::move(other.right)), head(std::move(other.head)), tail(std::move(other.tail))
{
}

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

#if defined(__ENABLE_PRINTING_LIST__)
    std::ostream& RHJ::operator<<(std::ostream& os, const RHJ::List& results)
    {
        for
        (
            const RHJ::List::Node * current = results.head;
            current != nullptr;
            current = current->next
        )
        {
            os << "+----------+----------+----------+----------+" << std::endl;
            os << "|Key L     |Value L   |Key R     |Value R   |" << std::endl;
            os << "+----------+----------+----------+----------+" << std::endl;
            for (std::size_t i = 0UL; i < current->buffer.size(); i++)
            {
                const std::size_t rowR = current->buffer[i].key1;
                const std::size_t rowS = current->buffer[i].key2;

                os << "|" << results.left.tuples[rowR]  << "|" << results.right.tuples[rowS] << "|" << std::endl;
            }
            os << "+----------+----------+----------+----------+" << std::endl;
        }

        return os;
    }
#endif
