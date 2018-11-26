
#include <types.hpp>
#include <query.hpp>
#include <relation.hpp>


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

        struct IntermediateResults {

            struct Node {

                struct Content {
                    
                    std::size_t *relationNames; // Relation indexes existing in this ContentS
                    std::size_t columnSize;     // Number of Values in each Column
                    std::size_t columnNum;      // Number of Columns
                    tuple_key_t *array;         // Array of Columns

                } content;

                Node * next;

                Node();
                ~Node() { if (next) delete next; }
            };

            Node * head;
            Node * tail;

            IntermediateResults() : head(nullptr), tail(nullptr) { }
            ~IntermediateResults();

            void search(std::size_t Rel_1, std::size_t Rel_2, Node *& node_1, Node *& node_2, std::size_t *index_1, std::size_t *index_2);
            void search(std::size_t Rel, Node *& node, std::size_t *index);

            void append(std::size_t *relationNames, std::size_t columnSize, std::size_t columnNum, tuple_key_t *array);

        } inteResults;

        Test Relations;

        void executeFilter(const Query& query, Query::Predicate pred);

        void executeJoin(const Query& query, Query::Predicate pred);
        void externalJoin(const Query& query, Query::Predicate::Operand inner, Query::Predicate::Operand outer);
        void internalJoin(const Query& query, Query::Predicate::Operand inner, IntermediateResults::Node *innerNode, 
                            std::size_t innerIndex, Query::Predicate::Operand outer);
        void internalSelfJoin();

        void executeSelfJoin(const Query& query, Query::Predicate pred);


        bool compare(tuple_payload_t u, tuple_key_t v, Query::Predicate::Type op);

    public:

        Executioner();
        ~Executioner();

        void execute(const Query& query);
    };
}



