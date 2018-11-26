#include <executioner.hpp>
#include <relation.hpp>
#include <cstdlib>
#include <iostream>

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
    if (!head) {
        head = new Node;
        tail = head;
    }
    else
        tail = tail->next = new Node;

    if (columnSize == 0) {
        tail->content.array = nullptr;
    }
    else {
        tail->content.array = array;
    }
    
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
            int value = Relations.relations[ query.relations[relation] ].column(column)[rowID].payload;

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

        int columnSize = Relations.relations[ query.relations[relation] ].columnSize;;

        tuple_key_t *filteredArray = new tuple_key_t[columnSize];
        std::size_t curIndex = 0;

        std::size_t relationNames[] = { pred.left.rel };


        // Iterate over whole column
        for (std::size_t i = 0; i < columnSize; i++) {

            int value = Relations.relations[ query.relations[relation] ].column(column)[i].payload;

            if (compare(value, immediate, op)) {
                
                filteredArray[curIndex] = i;
                curIndex++;
            }
        }

        this->inteResults.append(relationNames, curIndex, 1, filteredArray);
    }
}

void RHJ::Executioner::executeJoin(const Query& query, Query::Predicate pred) {

    if (pred.left.rel == pred.right.operand.rel) {
        executeSelfJoin(query, pred);
    }

    IntermediateResults::Node * leftNode;
    IntermediateResults::Node * rightNode;
    std::size_t leftIndex;
    std::size_t rightIndex;

    inteResults.search(pred.left.rel, pred.right.operand.rel, leftNode, rightNode, &leftIndex, &rightIndex);

    if (leftNode || rightNode) {
        if (leftNode && !rightNode) internalJoin(query, pred.left, leftNode, leftIndex, pred.right.operand);
        else if (rightNode && !leftNode) internalJoin(query, pred.right.operand, rightNode, rightIndex, pred.left);
        else ;
        
    }
    else {
        externalJoin(query, pred.left, pred.right.operand);
    }
}

void RHJ::Executioner::externalJoin(const Query& query, Query::Predicate::Operand inner, Query::Predicate::Operand outer) {
    int leftColumn = inner.col;
    int rightColumn = outer.col;

    RHJ::Relation left;
    left.size = Relations.relations[ query.relations[inner.rel] ].columnSize;
    left.tuples = Relations.relations[ query.relations[inner.rel] ].column(leftColumn);

    RHJ::Relation right;
    right.size = Relations.relations[ query.relations[outer.rel] ].columnSize;
    right.tuples = Relations.relations[ query.relations[outer.rel] ].column(rightColumn);

    
    RHJ::Results results = RHJ::Relation::RadixHashJoin(left, right);


    tuple_key_t *joinedArray = new tuple_key_t[2 * results.count];
    std::size_t curIndex = 0;

    std::size_t relationNames[] = { inner.rel, outer.rel };
    
    RHJ::Results::Node *current = results.head;
    
    while(curIndex < results.count) {
        for (std::size_t i = 0; i < current->buffer.size(); i++) {
            joinedArray[curIndex + i] = current->buffer[i].key1;
            joinedArray[curIndex + i + results.count] = current->buffer[i].key1;
            curIndex++;
        }
        current = current->next;
    }

    inteResults.append(relationNames, 2 * results.count, 2, joinedArray);

}

void RHJ::Executioner::internalJoin(const Query& query, Query::Predicate::Operand inner, IntermediateResults::Node *innerNode, 
                                    std::size_t innerIndex, Query::Predicate::Operand outer) 
{

    int leftColumn = inner.col;
    int rightColumn = outer.col;

    RHJ::Relation left;
    left.size = innerNode->content.columnSize;
    left.tuples = new Relation::Tuple[innerNode->content.columnSize];

    std::size_t j = 0;
    // Iterate over all rows in array
    for 
    (   
        std::size_t i = innerNode->content.columnSize * innerIndex; 
        i < innerNode->content.columnSize * (innerIndex + 1); 
        i++
    )
    {
        left.tuples[j].key = innerNode->content.array[i];
        left.tuples[j].payload = Relations.relations[ query.relations[inner.rel] ].column(leftColumn)[innerNode->content.array[i]].payload;
        j++;
    }

    RHJ::Relation right;
    right.size = Relations.relations[ query.relations[outer.rel] ].columnSize;
    right.tuples = Relations.relations[ query.relations[outer.rel] ].column(rightColumn);

    
    RHJ::Results results = RHJ::Relation::RadixHashJoin(left, right);


    tuple_key_t *joinedArray = new tuple_key_t[(innerNode->content.columnNum + 1) * results.count];
    std::size_t curIndex = 0;

    std::size_t relationNames[] = { inner.rel, outer.rel };
    
    RHJ::Results::Node *current = results.head;
    
    while(curIndex < results.count) {
        for (std::size_t i = 0; i < current->buffer.size(); i++) {
            joinedArray[curIndex + i] = current->buffer[i].key1;
            joinedArray[curIndex + i + results.count] = current->buffer[i].key1;
            curIndex++;
        }
        current = current->next;
    }

    inteResults.append(relationNames, 2 * results.count, 2, joinedArray);
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




RHJ::Test::Test() {
    this->relations[0].columnSize = 5;
    this->relations[0].columnNum = 2;
    this->relations[0].array = new RHJ::Relation::Tuple[10];
    this->relations[0].array[0].key = 0; 
    this->relations[0].array[0].payload = 1050; 
    this->relations[0].array[1].key = 1; 
    this->relations[0].array[1].payload = 3010;
    this->relations[0].array[2].key = 2; 
    this->relations[0].array[2].payload = 15764;
    this->relations[0].array[3].key = 3; 
    this->relations[0].array[3].payload = 5010;
    this->relations[0].array[4].key = 4; 
    this->relations[0].array[4].payload = 2;
    this->relations[0].array[5].key = 0; 
    this->relations[0].array[5].payload = 4052;
    this->relations[0].array[6].key = 1; 
    this->relations[0].array[6].payload = 3040;
    this->relations[0].array[7].key = 2; 
    this->relations[0].array[7].payload = 137;
    this->relations[0].array[8].key = 3; 
    this->relations[0].array[8].payload = 25;
    this->relations[0].array[9].key = 4; 
    this->relations[0].array[9].payload = 1;

    this->relations[2].columnSize = 5;
    this->relations[2].columnNum = 3;
    this->relations[2].array = new RHJ::Relation::Tuple[15];
    this->relations[2].array[0].key = 0; 
    this->relations[2].array[0].payload = 5; 
    this->relations[2].array[1].key = 1; 
    this->relations[2].array[1].payload = 4;
    this->relations[2].array[2].key = 2; 
    this->relations[2].array[2].payload = 3;
    this->relations[2].array[3].key = 3; 
    this->relations[2].array[3].payload = 2;
    this->relations[2].array[4].key = 4; 
    this->relations[2].array[4].payload = 1;
    this->relations[2].array[5].key = 0; 
    this->relations[2].array[5].payload = 20;
    this->relations[2].array[6].key = 1; 
    this->relations[2].array[6].payload = 21;
    this->relations[2].array[7].key = 2; 
    this->relations[2].array[7].payload = 22;
    this->relations[2].array[8].key = 3; 
    this->relations[2].array[8].payload = 23;
    this->relations[2].array[9].key = 4; 
    this->relations[2].array[9].payload = 24;
    this->relations[2].array[10].key = 0; 
    this->relations[2].array[10].payload = 2100;
    this->relations[2].array[11].key = 1; 
    this->relations[2].array[11].payload = 4052;
    this->relations[2].array[12].key = 2; 
    this->relations[2].array[12].payload = 9;
    this->relations[2].array[13].key = 3; 
    this->relations[2].array[13].payload = 3040;
    this->relations[2].array[14].key = 4; 
    this->relations[2].array[14].payload = 137;

    this->relations[4].columnSize = 5;
    this->relations[4].columnNum = 2;
    this->relations[4].array = new RHJ::Relation::Tuple[10];
    this->relations[4].array[0].key = 0; 
    this->relations[4].array[0].payload = 30; 
    this->relations[4].array[1].key = 1; 
    this->relations[4].array[1].payload = 31;
    this->relations[4].array[2].key = 2; 
    this->relations[4].array[2].payload = 32;
    this->relations[4].array[3].key = 3; 
    this->relations[4].array[3].payload = 33;
    this->relations[4].array[4].key = 4; 
    this->relations[4].array[4].payload = 34;
    this->relations[4].array[5].key = 0; 
    this->relations[4].array[5].payload = 10;
    this->relations[4].array[6].key = 1; 
    this->relations[4].array[6].payload = 4;
    this->relations[4].array[7].key = 2; 
    this->relations[4].array[7].payload = 9;
    this->relations[4].array[8].key = 3; 
    this->relations[4].array[8].payload = 2;
    this->relations[4].array[9].key = 4; 
    this->relations[4].array[9].payload = 5;
}

RHJ::Relation::Tuple *RHJ::Test::Relation::column(int index) {
    if (index >= columnNum) return nullptr;

    return &(this->array[index * columnSize]);
}

void RHJ::Test::Relation::print() {
    for (int i = 0; i < this->columnNum * this->columnSize; i++) {
        std::cout << this->array[i].payload << std::endl;
    }
}

void RHJ::Test::print() {
    this->relations[0].print();
    std::cout << std::endl;

    this->relations[2].print();
    std::cout << std::endl;

    this->relations[4].print();
    std::cout << std::endl;
}