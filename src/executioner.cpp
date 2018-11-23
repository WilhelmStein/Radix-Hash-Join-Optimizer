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

void RHJ::Executioner::IntermediateResults::contains(std::size_t Rel, Node *& node, std::size_t *index) {

    bool found = false;
    node = nullptr;
    *index = -1;

    Node * current = head;

    do {
        for (std::size_t i = 0; i < current->content.columnNum; i++) {
            if (current->content.relationNames[i] == Rel) {
                found = true;
                node = current;
                *index = i;
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

        switch(query.predicates[i].type) {
            case Query::Predicate::Type::join_t:
                executeJoin(query.predicates[i]);
                break;
            default:
                executeFilter(query.predicates[i]);
                break;
        }
    }
}

void RHJ::Executioner::executeFilter(Query::Predicate pred) {
    IntermediateResults::Node * node;
    std::size_t index;

    inteResults.contains(pred.left.rel, node, &index);

    if (node) {
        // Our array of rowids exists in
        // content.array[content.columnSize * index] - content.array[content.columnSize * (index + 1)]

        for (   
                std::size_t i = node->content.columnSize * index; 
                i < node->content.columnSize * (index + 1); 
                i++
            ) 
        {
            
        }
    }
    else {

    }
}

void RHJ::Executioner::executeJoin(Query::Predicate pred) {

}