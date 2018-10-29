
#include <result.hpp>
#include <utility>

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

void RHJ::List::append(const RHJ::List::Result& pair)
{
    if (tail->buffer._size == CAPACITY)
        tail = tail->next = new Node;

    tail->buffer._data[tail->buffer._size++] = pair;
}
