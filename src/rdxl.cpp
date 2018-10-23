
#include "rdxl.hpp"

inline RDXList::RDXNode::Buffer::Buffer()
:
index(0U)
{
}

inline bool RDXList::RDXNode::Buffer::full() const
{
    return index == CAPACITY;
}

inline void RDXList::RDXNode::Buffer::append(const Result& result)
{
    data[index++] = result;
}

inline RDXList::RDXNode::RDXNode()
:
next(nullptr)
{
}

inline RDXList::RDXNode::~RDXNode()
{
    if (next)
        delete next;
}

inline RDXList::RDXList()
:
head(new RDXList::RDXNode()), tail(head)
{
}

inline RDXList::~RDXList()
{
    delete head;
}

inline void RDXList::RDXList::append(const Result& result)
{
    if (tail->buffer.full())
        tail = tail->next = new RDXNode;

    tail->buffer.append(result);
}
