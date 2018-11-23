
#include <types.hpp>
#include <query.hpp>

namespace RHJ
{
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

                Node() : next(nullptr) {}
                ~Node() { if (next) delete next; }
            };

            Node * head;
            Node * tail;

            IntermediateResults() : head(nullptr), tail(nullptr) { }
            ~IntermediateResults();

            void contains(std::size_t Rel_1, std::size_t Rel_2, Node * node_1, Node * node_2);
            void contains(std::size_t Rel, Node *& node, std::size_t *index);

            void append(std::size_t *relationNames, std::size_t columnSize, std::size_t columnNum, tuple_key_t *array);

        } inteResults;

        void executeFilter(Query::Predicate pred);
        void executeJoin(Query::Predicate pred);

    public:

        Executioner();
        ~Executioner();

        void execute(const Query& query);
    };
}



