
#include <result.hpp>
#include <utility>

#ifdef __DEBUG_RESULT__
    #include <relation.hpp>
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

RHJ::List::List()
:
head(new RHJ::List::Node()), tail(head)
{
}

RHJ::List::List(List&& other) noexcept
:
head(std::move(other.head)), tail(std::move(other.tail))
{
}

RHJ::List::~List()
{
    delete head;
}

RHJ::List& RHJ::List::operator=(List&& other) noexcept
{
    head = std::move(other.head);
    tail = std::move(other.tail);

    return *this;
}

void RHJ::List::append(const RHJ::List::Result& result)
{
    if (tail->buffer._size == CAPACITY)
        tail = tail->next = new Node;

    tail->buffer._data[tail->buffer._size++] = result;
}

#ifdef __DEBUG_RESULT__
    #include <iostream>

    void RHJ::List::print(const RHJ::Relation& R, const RHJ::Relation& S)
    {
        std::cerr << R << std::endl << S << std::endl;

        for
        (
            const RHJ::List::Node * current = head;
            current != nullptr;
            current = current->next
        )
        {
            std::cerr << "+----------+----------+----------+----------+" << std::endl;
            std::cerr << "|Key R     |Value R   |Key S     |Value S   |" << std::endl;
            std::cerr << "+----------+----------+----------+----------+" << std::endl;
            for (std::size_t i = 0UL; i < current->buffer.size(); i++)
            {
                const std::size_t rowR = current->buffer[i].key1;
                const std::size_t rowS = current->buffer[i].key2;

                std::cerr << "|" << R.tuples[rowR]  << "|" << S.tuples[rowS] << "|" << std::endl;
            }
            std::cerr << "+----------+----------+----------+----------+" << std::endl;
        }
    }
#endif
