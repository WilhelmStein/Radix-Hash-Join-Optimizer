
#pragma once

#include <types.hpp>
#include <iosfwd>       // std::ostream

namespace RHJ
{
    class Query
    {
        std::size_t relCount, preCount, cheCount;

        std::size_t * relations;

        struct Predicate
        {
            enum Type { join_t, filter_eq_t, filter_lt_t, filter_gt_t } type;

            struct Operand
            {
                std::size_t rel, col;

                Operand& operator=(char *);
            } left;

            union
            {
                Operand operand;
                tuple_key_t constraint;
            } right;

            Predicate& operator=(char *);

        } * predicates;

        using Checksum = Predicate::Operand;

        Checksum * checksums;

        friend std::ostream& operator<<(std::ostream&, const Predicate::Operand&);
        friend std::ostream& operator<<(std::ostream&, const Predicate&);
        friend std::ostream& operator<<(std::ostream&, const Query&);

    public:

        Query(char *);
        ~Query() { delete[] relations; delete[] predicates; delete[] checksums; }
    };

    std::ostream& operator<<(std::ostream&, const Query::Predicate::Operand&);
    std::ostream& operator<<(std::ostream&, const Query::Predicate&);
    std::ostream& operator<<(std::ostream&, const Query&);
}