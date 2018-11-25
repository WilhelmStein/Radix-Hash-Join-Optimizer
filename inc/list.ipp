
#pragma once

#include <fstream>          // std::ostream
#include <utility>          // std::move, std::forward

// node<T> implementation
template <typename T>
inline utility::list<T>::node::node(node * prev, T&& data)
:
data(std::forward<T>(data)), prev(prev), next(nullptr)
{
}

template <typename T>
template <typename ...Args>
inline utility::list<T>::node::node(node * prev, Args&&... args)
:
data(std::forward<Args>(args)...), prev(prev), next(nullptr)
{
}

template <typename T>
inline utility::list<T>::node::~node()
{
    if (next)
        delete next;
}

// iterator<T> implementation
template <typename T>
inline utility::list<T>::iterator::iterator()
:
ptr(nullptr)
{
}

template <typename T>
inline utility::list<T>::iterator::iterator(node * ptr)
:
ptr(ptr)
{
}

template <typename T>
inline utility::list<T>::iterator::iterator(iterator&& other) noexcept
:
ptr(other.ptr)
{
    other.ptr = nullptr;
}

template <typename T>
inline typename utility::list<T>::iterator& utility::list<T>::iterator::operator=(iterator&& other) noexcept
{
    ptr = other.ptr; other.ptr = nullptr; return *this;
}

template <typename T>
inline typename utility::list<T>::iterator& utility::list<T>::iterator::operator++()
{
    ptr = ptr->next; return *this;
}

template <typename T>
inline const T& utility::list<T>::iterator::operator*() const
{
    return ptr->data;
}

// list<T> Implementation
template <typename T>
inline utility::list<T>::list()
:
head(nullptr), tail(nullptr), s(0UL)
{
}

template <typename T>
inline utility::list<T>::list(list<T>&& other) noexcept
:
head(other.head), tail(other.tail), s(std::move(other.s))
{
    other.head = other.tail = nullptr;
}

template <typename T>
inline utility::list<T>::~list()
{
    if (head)
        delete head;
}

template <typename T>
inline utility::list<T>& utility::list<T>::operator=(list<T>&& other) noexcept
{
    head = other.head; other.head = nullptr;
    tail = other.tail; other.tail = nullptr;

    s = std::move(other.s);

    return *this;
}

template <typename T>
inline void utility::list<T>::push_back(T&& data)
{
    if (!head)
    {
        head = tail = new node(nullptr, std::forward<T>(data));
    }
    else
    {
        tail = tail->next = new node(tail, std::forward<T>(data));
    }

    s++;
}

template <typename T>
template <typename ...Args>
inline void utility::list<T>::emplace_back(Args&&... args)
{
    if (!head)
    {
        head = tail = new node(nullptr, std::forward<Args>(args)...);
    }
    else
    {
        tail = tail->next = new node(tail, std::forward<Args>(args)...);
    }

    s++;
}

template <typename T>
inline typename utility::list<T>::iterator utility::list<T>::find(const T& data)
{
    for (node * current = head; current; current = current->next)
        if (current->data == data)
            return iterator(current);

    return end();
}

template <typename T>
inline typename utility::list<T>::iterator utility::list<T>::erase(const iterator& cit)
{
    node ** prevptr = &(cit.ptr->prev), ** nextptr = &(cit.ptr->next);

    if (!*prevptr && !*nextptr)
    {
        head = tail = nullptr;
    }
    else if (!*prevptr)
    {
        (*nextptr)->prev = nullptr; head = *nextptr;
    }
    else if (!*nextptr)
    {
        (*prevptr)->next = nullptr; tail = *prevptr;
    }
    else
    {
        (*prevptr)->next = *nextptr;
        (*nextptr)->prev = *prevptr;
    }

    iterator it(*nextptr);

    *prevptr = *nextptr = nullptr; delete cit.ptr; s--;

    return it;
}

template <typename T>
inline T& utility::list<T>::front()
{
    return head->data;
}

template <typename T>
inline T& utility::list<T>::back()
{
    return tail->data;
}

template <typename T>
inline typename utility::list<T>::iterator utility::list<T>::begin() const
{
    return iterator(head);
}

template <typename T>
inline typename utility::list<T>::iterator utility::list<T>::end() const
{
    return iterator(nullptr);
}

template <typename T>
inline bool utility::list<T>::empty() const
{
    return !head;
}

template <typename T>
inline std::size_t utility::list<T>::size() const
{
    return s;
}
