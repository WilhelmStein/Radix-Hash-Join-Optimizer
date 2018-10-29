
#include <result.hpp>

RHJ::List::Node::Buffer::Buffer()
:
index(0UL)
{
}

bool RHJ::List::Node::Buffer::full() const
{
    return index == CAPACITY;
}

void RHJ::List::Node::Buffer::append(const RHJ::Pair& pair)
{
    data[index++] = pair;
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

RHJ::List::~List()
{
    delete head;
}

void RHJ::List::append(const RHJ::Pair& pair)
{
    if (tail->buffer.full())
        tail = tail->next = new Node;

    tail->buffer.append(pair);
}
