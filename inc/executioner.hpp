
#include <types.hpp>
#include <query.hpp>
#include <relation.hpp>
#include <unordered_map>
#include <vector>
#include <list.hpp>


namespace RHJ
{
    class Test {

    public:

        struct Relation {

            int columnNum;
            int columnSize;
            RHJ::Relation::Tuple *array;

            RHJ::Relation::Tuple *column(int index);

            void print();
        } relations[5];

        Test();
        void print();
        // ~Test();
    };

    class Executioner {

        struct Entity {

            std::size_t columnSize;     // Number of Values in each Column
            std::size_t columnNum;      // Number of Columns

            std::unordered_map<std::size_t, std::vector<tuple_key_t> > map;

        };

        struct IntermediateResults : public utility::list<Entity> {

            IntermediateResults();
            ~IntermediateResults();

            std::vector<iterator> find(std::size_t Rel_1, std::size_t Rel_2, iterator it_1, iterator it_2);
            iterator find(std::size_t Rel);

        } inteResults;

        Test Relations;

        void executeFilter(const Query& query, Query::Predicate pred);

        void executeJoin(const Query& query, Query::Predicate pred);

        void externalJoin(const Query& query, Query::Predicate::Operand inner, Query::Predicate::Operand outer);

        void semiInternalJoin(const Query& query, Query::Predicate::Operand inner, IntermediateResults::iterator innerIt, Query::Predicate::Operand outer);

        void internalJoin(const Query& query, Query::Predicate::Operand inner, IntermediateResults::iterator innerIt, 
                                              Query::Predicate::Operand outer, IntermediateResults::iterator outerIt);

        void internalSelfJoin(const Query& query, Query::Predicate::Operand inner, IntermediateResults::iterator innerIt, Query::Predicate::Operand outer);
        void executeSelfJoin(const Query& query, Query::Predicate pred);


        bool compare(tuple_payload_t u, tuple_key_t v, Query::Predicate::Type op);

    public:

        Executioner();
        ~Executioner();

        void execute(const Query& query);
    };
}

std::vector<std::size_t> findIndexes(std::vector<tuple_key_t> vec, tuple_key_t val);



