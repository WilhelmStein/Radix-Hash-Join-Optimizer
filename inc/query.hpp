
#pragma once

#include <types.hpp>
#include <iosfwd>       // std::ostream

namespace RHJ
{
    class Query
    {
        friend class Executioner;

        std::size_t relCount, preCount, cheCount;

        std::size_t * relations;

        struct Predicate
        {
            enum Type { filter_eq_t, filter_lt_t, filter_gt_t, join_t } type;

            struct Operand
            {
                std::size_t rel, col;

                Operand& operator=(char *);
            } left;

            union
            {
                Operand operand;
                tuple_payload_t constraint;
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
