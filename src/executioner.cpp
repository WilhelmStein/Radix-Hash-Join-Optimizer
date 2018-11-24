#include <executioner.hpp>
#include <cstdlib>

RHJ::Executioner::IntermediateResults::~IntermediateResults() {
    if (head)
        delete head;
}

RHJ::Executioner::Executioner() {

}

RHJ::Executioner::~Executioner() {

}

RHJ::Executioner::IntermediateResults::Node::Node() : next(nullptr) {
    this->content.array = nullptr;
    this->content.columnNum = 0;
    this->content.columnSize = 0;
    this->content.relationNames = nullptr;
}

void RHJ::Executioner::IntermediateResults::search(std::size_t Rel_1, std::size_t Rel_2, Node *& node_1, Node *& node_2, std::size_t *index_1, std::size_t *index_2) {

    bool found_1 = false;
    node_1 = nullptr;

    bool found_2 = false;
    node_2 = nullptr;

    Node * current = head;
    if (!current) return;
    
    do {
        for (int i = 0; i < current->content.columnNum; i++) {
            if (current->content.relationNames[i] == Rel_1) {
                found_1 = true;
                node_1 = current;
                *index_1 = i;
            }
            if (current->content.relationNames[i] == Rel_1) {
                found_2 = true;
                node_2 = current;
                *index_2 = i;
            }
        }

        if (current->next)
            current = current->next;
        else break;
    
    } while (!found_1 || !found_2);
}

void RHJ::Executioner::IntermediateResults::search(std::size_t Rel, Node *& node, std::size_t *index) {

    bool found = false;
    node = nullptr;
    *index = -1;

    Node * current = head;
    if (!current) return;

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
                executeJoin(query, query.predicates[i]);
                break;
            default:
                executeFilter(query, query.predicates[i]);
                break;
        }
    }
}

void RHJ::Executioner::executeFilter(const Query& query, Query::Predicate pred) {
    IntermediateResults::Node * node;
    std::size_t index;

    inteResults.search(pred.left.rel, node, &index);

    // SUBJECT TO CHANGE //
    int relation = pred.left.rel;
    int column = pred.left.col;
    int immediate = pred.right.constraint;
    Query::Predicate::Type op = pred.type;
    // SUBJECT TO CHANGE //

    if (node) {
        // Our array of rowids exists in
        // content.array[content.columnSize * index] - content.array[content.columnSize * (index + 1)]

        // Create new array
        tuple_key_t *filteredArray = new  tuple_key_t[node->content.columnNum * node->content.columnSize];
        std::size_t curIndex = 0;

        // Iterate over all rows in array
        for 
        (   
            std::size_t i = node->content.columnSize * index; 
            i < node->content.columnSize * (index + 1); 
            i++
        ) 
        {
            // For each one get its value from its appropriate relation and check if filter is true
            int rowID = node->content.array[i];

            // So we want something like:
            // value = *Relations*[ query.relations[relation] ].column(column)[rowID]

            /* placeholder */ int value = 1; /* placeholder */

            if (compare(value, immediate, op)) {
                // Copy whole "row" into filteredArray
                for 
                (
                    std::size_t j = curIndex; 
                    j <= (node->content.columnNum * node->content.columnSize) - (node->content.columnSize - curIndex); 
                    j += node->content.columnSize
                ) 
                {
                    filteredArray[j] = rowID;
                }
                curIndex++;
            }
        }

        // Replace array with new filtered array
        node->content.array = filteredArray;
        node->content.columnSize = curIndex;    // curIndex can function as array size
    }
    else {
        // construct new node

        int columnSize = 100;

        tuple_key_t *filteredArray = new tuple_key_t[columnSize];
        std::size_t curIndex = 0;

        std::size_t relationNames[] = { pred.left.rel };

        // So we want something like:
        // value = *Relations*[ query.relations[relation] ].column(column)[rowID]

        /* placeholder */ int value = 1; /* placeholder */

        // Iterate over whole column
        for (std::size_t i = 0; i < columnSize; i++) {

            if (compare(value, immediate, op)) {
                
                filteredArray[curIndex] = i;
                curIndex++;
            }
        }

        this->inteResults.append(relationNames, curIndex, 1, filteredArray);
    }
}

void RHJ::Executioner::executeJoin(const Query& query, Query::Predicate pred) {

}

void RHJ::Executioner::executeSelfJoin(const Query& query, Query::Predicate pred) {

}

bool RHJ::Executioner::compare(tuple_payload_t u, tuple_key_t v, Query::Predicate::Type op) {
    switch (op) {
        case Query::Predicate::Type::filter_eq_t:
            return u == v;
            break;
        case Query::Predicate::Type::filter_gt_t:
            return u > v;
            break;
        case Query::Predicate::Type::filter_lt_t:
            return u < v;
            break;
        default:
            break;
    }
}

