#include <executioner.hpp>
#include <relation.hpp>
#include <cstdlib>
#include <iostream>
#include <unordered_set>


RHJ::Executioner::IntermediateResults::IntermediateResults() : list() { }

RHJ::Executioner::IntermediateResults::~IntermediateResults() { }

RHJ::Executioner::Executioner() : inteResults() { }

RHJ::Executioner::~Executioner() { }


pair<RHJ::Executioner::IntermediateResults::iterator, RHJ::Executioner::IntermediateResults::iterator> 
RHJ::Executioner::IntermediateResults::find(std::size_t Rel_1, std::size_t Rel_2) {

    pair<iterator, iterator> ret;

    bool found_1 = false;

    bool found_2 = false;


    for (iterator it = begin(); it != end(); ++it) {
        
        if ( (*it).map.find(Rel_1) != (*it).map.end() ) {
            found_1 = true;
            ret.first = it;
        }
        if ( (*it).map.find(Rel_2) != (*it).map.end() ) {
            found_2 = true;
            outerIt = it;
        }
    }
}

void RHJ::Executioner::IntermediateResults::find(std::size_t Rel, Node *& node) {

    bool found = false;
    node = nullptr;

    Node * current = head;
    if (!current) return;
    
    do {
        for (int i = 0; i < current->content.columnNum; i++) {
            if (current->content.map.find(Rel) != current->content.map.end()) {
                found = true;
                node = current;
            }
        }

        if (current->next)
            current = current->next;
        else break;
    
    } while (!found);
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

    inteResults.search(pred.left.rel, node);

    // SUBJECT TO CHANGE //
    int relation = pred.left.rel;
    int column = pred.left.col;
    int immediate = pred.right.constraint;
    Query::Predicate::Type op = pred.type;
    // SUBJECT TO CHANGE //

    if (node) {

        // Iterate over all rows in array
        for (int i = 0; i < node->content.columnSize; i++ ) 
        {
            // For each one get its value from its appropriate relation and check if filter is true
            int rowID = node->content.map[relation][i];

            // So we want something like:
            int value = Relations.relations[ query.relations[relation] ].column(column)[rowID].payload;

            if (!compare(value, immediate, op)) {

                // if comparison returns false then remove the value from the vector
                for (auto &vec : node->content.map) {
                    std::vector<tuple_key_t>::iterator ith = vec.second.begin() + i;
                    vec.second.erase(ith);
                }

                // Decreasing iterators because we erased one value
                i--;
                node->content.columnSize--;
            }
        }
    }
    else {
        // construct new node

        int columnSize = Relations.relations[ query.relations[relation] ].columnSize;;

        std::vector<tuple_key_t> filteredVector;

        // Iterate over whole column
        for (std::size_t i = 0; i < columnSize; i++) {

            int value = Relations.relations[ query.relations[relation] ].column(column)[i].payload;

            if (compare(value, immediate, op)) {
                
                filteredVector.push_back(i);
            }
        }

        std::unordered_map<std::size_t, std::vector<tuple_key_t>> map;
        map[relation] = filteredVector;

        this->inteResults.append(map);
    }
}

void RHJ::Executioner::executeJoin(const Query& query, Query::Predicate pred) {

    if (pred.left.rel == pred.right.operand.rel) {
        executeSelfJoin(query, pred);
    }

    IntermediateResults::Node * leftNode;
    IntermediateResults::Node * rightNode;

    inteResults.search(pred.left.rel, pred.right.operand.rel, leftNode, rightNode);

    if (leftNode || rightNode) {
        if (leftNode && !rightNode) 
            semiInternalJoin(query, pred.left, leftNode, pred.right.operand);

        else if (rightNode && !leftNode) 
            semiInternalJoin(query, pred.right.operand, rightNode, pred.left);
            
        else {
            if (leftNode == rightNode) {
                // internalSelfJoin(query, pred.left, leftNode, pred.right.operand);
            }
            else {
                internalJoin(query, pred.left, leftNode, pred.right.operand, rightNode);
            }
        }
        
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


    std::vector<tuple_key_t> leftVector;
    std::vector<tuple_key_t> rightVector;
    
    RHJ::Results::Node *current = results.head;
    
    do {

        for (std::size_t i = 0; i < current->buffer.size(); i++) {
            leftVector.push_back(current->buffer[i].key1);
            rightVector.push_back(current->buffer[i].key2);
        }

        if (current->next)
            current = current->next;
        else break;

    } while (true);

    std::unordered_map<std::size_t, std::vector<tuple_key_t>> map;
    map[inner.rel] = leftVector;
    map[outer.rel] = rightVector;

    inteResults.append(map);

    left.tuples = nullptr;
    right.tuples = nullptr;

}

void RHJ::Executioner::semiInternalJoin(const Query& query, Query::Predicate::Operand inner, IntermediateResults::Node *innerNode, Query::Predicate::Operand outer) 
{

    int leftColumn = inner.col;
    int rightColumn = outer.col;

    RHJ::Relation left;
    left.size = innerNode->content.columnSize;
    left.tuples = new Relation::Tuple[innerNode->content.columnSize];

    std::size_t i = 0;
    // Iterate over all rows in array
    for (auto &rowId : innerNode->content.map[inner.rel])  
    {
        left.tuples[i].key = rowId;
        left.tuples[i].payload = Relations.relations[ query.relations[inner.rel] ].column(leftColumn)[rowId].payload;
        i++;
    }

    RHJ::Relation right;
    right.size = Relations.relations[ query.relations[outer.rel] ].columnSize;
    right.tuples = Relations.relations[ query.relations[outer.rel] ].column(rightColumn);

    
    RHJ::Results results = RHJ::Relation::RadixHashJoin(left, right);

    std::unordered_map<std::size_t, std::vector<tuple_key_t>> map;

    RHJ::Results::Node *current = results.head;
    
    do {
        for (std::size_t i = 0; i < current->buffer.size(); i++) {

            // Does sorting really matter? (i dont think so)

            std::vector<std::size_t> indexes = findIndexes(innerNode->content.map[inner.rel], current->buffer[i].key1);

            for (int j = 0; j < indexes.size(); j++) {

                for (auto &item : innerNode->content.map) {
                    map[item.first].push_back( item.second[indexes[j]] );
                }
                map[outer.rel].push_back(current->buffer[i].key2);
            }
        }

        if (current->next)
            current = current->next;
        else break;

    } while (true);

    innerNode->content.map = map;
    innerNode->content.columnSize = map.begin()->second.size();
    innerNode->content.columnNum = map.size();

    right.tuples = nullptr;
}

std::vector<std::size_t> findIndexes(std::vector<tuple_key_t> vec, tuple_key_t val) {
    std::vector<std::size_t> found;

    for (int i = 0; i < vec.size(); i++) {
        if (vec[i] == val) {
            found.push_back(i);
        }
    }

    return found;
}

void RHJ::Executioner::internalJoin(const Query& query, Query::Predicate::Operand inner, IntermediateResults::Node *innerNode, 
                                                        Query::Predicate::Operand outer, IntermediateResults::Node *outerNode) 
{

    RHJ::Relation left;
    left.size = innerNode->content.columnSize;
    left.tuples = new Relation::Tuple[innerNode->content.columnSize];

    std::size_t i = 0;
    // Iterate over all rows in array
    for (auto &rowId : innerNode->content.map[inner.rel])  
    {
        left.tuples[i].key = rowId;
        left.tuples[i].payload = Relations.relations[ query.relations[inner.rel] ].column(inner.col)[rowId].payload;
        i++;
    }

    RHJ::Relation right;
    right.size = outerNode->content.columnSize;
    right.tuples = new Relation::Tuple[outerNode->content.columnSize];

    i = 0;
    // Iterate over all rows in array
    for (auto &rowId : outerNode->content.map[outer.rel])  
    {
        right.tuples[i].key = rowId;
        right.tuples[i].payload = Relations.relations[ query.relations[outer.rel] ].column(outer.col)[rowId].payload;
        i++;
    }

    RHJ::Results results = RHJ::Relation::RadixHashJoin(left, right);

    std::unordered_map<std::size_t, std::vector<tuple_key_t>> map;

    RHJ::Results::Node *current = results.head;
    
    do {
        for (std::size_t i = 0; i < current->buffer.size(); i++) {

            // Does sorting really matter? (i dont think so)

            std::vector<std::size_t> innerIndexes = findIndexes(innerNode->content.map[inner.rel], current->buffer[i].key1);
            std::vector<std::size_t> outerIndexes = findIndexes(outerNode->content.map[outer.rel], current->buffer[i].key2);

            for (int j = 0; j < innerIndexes.size(); j++) {

                for (int k = 0; k < outerIndexes.size(); k++) {

                    for (auto &item : innerNode->content.map) {
                        map[item.first].push_back( item.second[innerIndexes[j]] );
                    }
                    
                    for (auto &item : outerNode->content.map) {
                        map[item.first].push_back( item.second[outerIndexes[k]] );
                    }  
                }
            
            }
        }

        if (current->next)
            current = current->next;
        else break;

    } while (true);

    innerNode->content.map = map;
    innerNode->content.columnSize = map.begin()->second.size();
    innerNode->content.columnNum = map.size();

    this->inteResults.erase(outerNode);

    right.tuples = nullptr;
    left.tuples = nullptr;
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
    // rel 0
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

    // rel 1
    this->relations[1].columnSize = 10;
    this->relations[1].columnNum = 1;
    this->relations[1].array = new RHJ::Relation::Tuple[10];
    this->relations[1].array[0].key = 0; 
    this->relations[1].array[0].payload = 4;
    this->relations[1].array[1].key = 1; 
    this->relations[1].array[1].payload = 4; 
    this->relations[1].array[2].key = 2; 
    this->relations[1].array[2].payload = 1; 
    this->relations[1].array[3].key = 3; 
    this->relations[1].array[3].payload = 2; 
    this->relations[1].array[4].key = 4; 
    this->relations[1].array[4].payload = 3; 
    this->relations[1].array[5].key = 5; 
    this->relations[1].array[5].payload = 5; 
    this->relations[1].array[6].key = 6; 
    this->relations[1].array[6].payload = 9; 
    this->relations[1].array[7].key = 7; 
    this->relations[1].array[7].payload = 9; 
    this->relations[1].array[8].key = 8; 
    this->relations[1].array[8].payload = 10; 
    this->relations[1].array[9].key = 9; 
    this->relations[1].array[9].payload = 3; 

    // rel 2
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

    // rel 4
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