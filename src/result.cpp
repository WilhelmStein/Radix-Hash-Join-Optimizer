
#include <result.hpp>
#include <relation.hpp>
#include <utility>          // std::move
#include <fstream>          // std::ostream

#if defined (__VERBOSE__)
    RHJ::Results::Results(const RHJ::Relation& left, const RHJ::Relation& right)
    :
    list(), left(&left), right(&right)
    {
    }

    RHJ::Results::Results(Results&& other) noexcept
    :
    list(std::move(other)), left(other.left), right(other.right)
    {
    }

    RHJ::Results& RHJ::Results::operator=(Results&& other) noexcept
    {
        static_cast<list<Buffer>&>(*this) = std::move(other);
        
        left  = other.left;
        right = other.right;

        return *this;
    }
#else
    RHJ::Results::Results()
    :
    list()
    {
    }

    RHJ::Results::Results(Results&& other) noexcept
    :
    list(std::move(other))
    {
    }

    RHJ::Results& RHJ::Results::operator=(Results&& other) noexcept
    {
        static_cast<list<Buffer>&>(*this) = std::move(other);

        return *this;
    }
#endif

RHJ::Results::~Results()
{
}

void RHJ::Results::push_back(tuple_key_t key1, tuple_key_t key2)
{
    if (!tail || back()._size == CAPACITY)
        emplace_back();

    back()._data[back()._size++] = { key1, key2 };
}

// void RHJ::Results::merge(Results&&)
// {
    
// }

std::ostream& RHJ::operator<<(std::ostream& os, const RHJ::Results& results)
{
    #if defined (__VERBOSE__)
        os << "+----------+----------+----------+----------+" << std::endl;
        os << "|Key L     |Value L   |Key R     |Value R   |" << std::endl;
        os << "+----------+----------+----------+----------+" << std::endl;
    #endif

    std::size_t count = 0UL;
    for (const auto& buffer : results)
    {
        #if defined (__VERBOSE__)
            for (std::size_t i = 0UL; i < buffer.size(); i++)
            {
                const std::size_t rowR = buffer[i].key1;
                const std::size_t rowS = buffer[i].key2;

                os << "|" << results.left->tuples[rowR]  << "|" << results.right->tuples[rowS] << "|" << std::endl;
            }
        #endif

        count += buffer.size();
    }

    #if defined (__VERBOSE__)
        os << "+----------+----------+----------+----------+" << std::endl;

        os
        << "\n[L=\""    << results.left->name
        << "\", R=\"" << results.right->name
        << "\"]: ";
    #endif

    os << count << " results...";

    return os;
}
