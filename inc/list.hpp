
#pragma once

#include <iosfwd>       // std::ostream

namespace utility
{
    template <typename T>
    class list
    {
    protected:
    
        struct node
        {
            T data;

            node * prev, * next;

            node(node *, T&&);

            template <typename ...Args>
            node(node *, Args&&...);
            
            ~node();

        } * head, * tail;

        std::size_t s;

    public:

        class iterator
        {
            friend class list;

            node * ptr;

            iterator(node *);

        public:

            iterator();

            iterator(const iterator&) = delete;
            iterator(iterator&&) noexcept;

            iterator& operator=(const iterator&) = delete;
            iterator& operator=(iterator&&) noexcept;

            iterator& operator++();

            const T& operator*() const;

            friend bool operator!=(const iterator& lhs, const iterator& rhs)
            {
                return lhs.ptr != rhs.ptr;
            }
        };

        list();
        
        list(const list&) = delete;
        list(list&&) noexcept;

        virtual ~list();

        list& operator=(const list&) = delete;
        list& operator=(list&&) noexcept;

        void push_back(T&&);
        void pop_back();

        template <typename ...Args>
        void emplace_back(Args&&...);

        iterator find(const T&);
        iterator erase(const iterator&);

        T& front();
        T& back();

        iterator begin() const;
        iterator end() const;

        bool empty() const;
        std::size_t size() const;
    };
}

#include <list.ipp>
