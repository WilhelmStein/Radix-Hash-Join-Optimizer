#include <executioner.hpp>


RHJ::Executioner::IntermediateResults::~IntermediateResults() {
    if (head)
        delete head;
}

void RHJ::Executioner::IntermediateResults::contains(std::size_t Rel_1, std::size_t Rel_2, Node * node_1, Node * node_2) {

    bool found_1 = false;
    node_1 = nullptr;

    bool found_2 = false;
    node_2 = nullptr;

    Node * current = head;

    do {
        for (int i = 0; i < current->content.columnNum; i++) {
            if (current->content.relationNames[i] == Rel_1) {
                found_1 = true;
                node_1 = current;
            }
            if (current->content.relationNames[i] == Rel_1) {
                found_2 = true;
                node_2 = current;
            }
        }

        if (current->next)
            current = current->next;
        else break;
    
    } while (!found_1 || !found_2);
}

void RHJ::Executioner::IntermediateResults::contains(std::size_t Rel, Node * node) {

    bool found = false;
    node = nullptr;

    Node * current = head;

    do {
        for (int i = 0; i < current->content.columnNum; i++) {
            if (current->content.relationNames[i] == Rel) {
                found = true;
                node = current;
                break;
            }
        }

        if (found) break;

        if (current->next)
            current = current->next;
        else break;
    
    } while (!found);
}

void RHJ::Executioner::IntermediateResults::append(std::size_t *relationNames, std::size_t columnSize, std::size_t columnNum, tuple_key_t *array)
{
    tail = tail->next = new Node;

    tail->content.array = array;
    tail->content.columnNum = columnNum;
    tail->content.columnSize = columnSize;
    tail->content.relationNames = relationNames;
}


void RHJ::Executioner::execute(const Query& query) {

    

    for (int i = 0; i < query.preCount; i++) {

    }
}