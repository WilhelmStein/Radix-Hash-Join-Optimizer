
#include <types.hpp>
#include <query.hpp>

namespace RHJ
{
    class Executioner {

        struct IntermediateResults {

            struct Node {

                struct Content {
                    
                    std::size_t *relationNames; // 
                    std::size_t columnSize;     // Number of Values in each Column
                    std::size_t columnNum;      // Number of Columns
                    tuple_key_t *array;         // Array of Columns

                } content;

                Node * next;

                Node() : next(nullptr) {}
                ~Node() { if (next) delete next; }
            };

            Node * root;

            void contains(std::size_t Rel_1, std::size_t Rel_2, Node * node_1, Node * node_2);
        };

    public:

        Executioner(const Query& _query);
        ~Executioner();

    };
}



