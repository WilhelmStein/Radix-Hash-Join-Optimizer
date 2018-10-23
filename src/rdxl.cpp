
#include "../inc/rdxl.hpp"
#include <iostream>

RDXList::RDXNode::Buffer::Buffer()
:
index(0U)
{
}

bool RDXList::RDXNode::Buffer::full() const
{
    return index == CAPACITY;
}

void RDXList::RDXNode::Buffer::append(const Result& result)
{
    data[index++] = result;
}

RDXList::RDXNode::RDXNode()
:
next(nullptr)
{
}

RDXList::RDXNode::~RDXNode()
{
    if (next)
        delete next;
}

RDXList::RDXList()
:
head(new RDXList::RDXNode()), tail(head)
{
}

RDXList::~RDXList()
{
    delete head;
}

void RDXList::RDXList::append(const Result& result)
{
    if (tail->buffer.full())
    {
        tail = tail->next = new RDXNode;   
    }
    tail->buffer.append(result);
}
