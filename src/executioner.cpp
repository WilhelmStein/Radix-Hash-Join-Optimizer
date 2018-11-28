#include <executioner.hpp>
#include <relation.hpp>
#include <cstdlib>
#include <iostream>
#include <unordered_set>

RHJ::Executioner::Entity::Entity(std::size_t _columnNum, std::size_t _columnSize, std::unordered_map<std::size_t, std::vector<tuple_key_t>> _map) 
:
columnNum(_columnNum), columnSize(_columnSize), map(_map)
{

}

RHJ::Executioner::Entity::~Entity() {
    
}

RHJ::Executioner::IntermediateResults::IntermediateResults() : list() { }

RHJ::Executioner::IntermediateResults::~IntermediateResults() { }

RHJ::Executioner::Executioner() : inteResults() { }

RHJ::Executioner::~Executioner() { }


utility::pair<RHJ::Executioner::IntermediateResults::iterator, RHJ::Executioner::IntermediateResults::iterator> 
RHJ::Executioner::IntermediateResults::find(std::size_t Rel_1, std::size_t Rel_2) {

    utility::pair<iterator, iterator> ret(end(), end());

    bool found_1 = false;

    bool found_2 = false;


    for (iterator it = begin(); it != end(); ++it) {
        
        if ( (*it).map.find(Rel_1) != (*it).map.end() ) {
            found_1 = true;
            ret.first = it;
        }
        if ( (*it).map.find(Rel_2) != (*it).map.end() ) {
            found_2 = true;
            ret.second = it;
        }

        if (found_1 && found_2) return ret;
    }

    return ret;
}

RHJ::Executioner::IntermediateResults::iterator RHJ::Executioner::IntermediateResults::find(std::size_t Rel) {

    bool found = false;

    for (iterator it = begin(); it != end(); ++it) {
        
        if ( (*it).map.find(Rel) != (*it).map.end() ) {
            found = true;
            return it;
        }
    }
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

    // SUBJECT TO CHANGE //
    int relation = pred.left.rel;
    int column = pred.left.col;
    int immediate = pred.right.constraint;
    Query::Predicate::Type op = pred.type;
    // SUBJECT TO CHANGE //

    IntermediateResults::iterator node = inteResults.find(pred.left.rel);

    if (node != inteResults.end() ) {

        // Iterate over all rows in array
        for (int i = 0; i < (*node).columnSize; i++ ) 
        {
            // For each one get its value from its appropriate relation and check if filter is true
            int rowID = (*node).map[relation][i];

            // So we want something like:
            int value = Relations.relations[ query.relations[relation] ].column(column)[rowID].payload;

            if (!compare(value, immediate, op)) {

                // if comparison returns false then remove the value from the vector
                for (auto &vec : (*node).map) {
                    std::vector<tuple_key_t>::iterator ith = vec.second.begin() + i;
                    vec.second.erase(ith);
                }

                // Decreasing iterators because we erased one value
                i--;
                (*node).columnSize--;
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

        this->inteResults.emplace_back(map.size(), filteredVector.size(), map);
    }
}

void RHJ::Executioner::executeJoin(const Query& query, Query::Predicate pred) {

    

    utility::pair<RHJ::Executioner::IntermediateResults::iterator, RHJ::Executioner::IntermediateResults::iterator> nodes;
    nodes = inteResults.find(pred.left.rel, pred.right.operand.rel);

    bool left = nodes.first != inteResults.end();
    bool right = nodes.second != inteResults.end();

    if (left || right) {
        if (left && !right) 
            semiInternalJoin(query, pred.left, nodes.first, pred.right.operand);

        else if (right && !left) 
            semiInternalJoin(query, pred.right.operand, nodes.second, pred.left);
            
        else {
            if (nodes.first == nodes.second) {
                internalSelfJoin(query, pred.left, nodes.first, pred.right.operand);
            }
            else {
                internalJoin(query, pred.left, nodes.first, pred.right.operand, nodes.second);
            }
        } 
    }
    else {
        if (pred.left.rel == pred.right.operand.rel) {
            externalSelfJoin(query, pred.left, pred.right.operand);
        }
        else
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
    

    for (auto &buffer : results) {
        for (std::size_t i = 0; i < buffer.size(); i++) {
            // Does sorting really matter? (i dont think so)
            leftVector.push_back(buffer[i].key1);
            rightVector.push_back(buffer[i].key2);
            
        }
    }

    std::unordered_map<std::size_t, std::vector<tuple_key_t>> map;
    map[inner.rel] = leftVector;
    map[outer.rel] = rightVector;

    inteResults.emplace_back(map.size(), leftVector.size(), map);

    left.tuples = nullptr;
    right.tuples = nullptr;

}


void RHJ::Executioner::semiInternalJoin(const Query& query, Query::Predicate::Operand inner, IntermediateResults::iterator innerIt,
                                        Query::Predicate::Operand outer) 
{

    int leftColumn = inner.col;
    int rightColumn = outer.col;

    RHJ::Relation left;
    left.size = (*innerIt).columnSize;
    left.tuples = new Relation::Tuple[(*innerIt).columnSize];

    std::size_t i = 0;
    // Iterate over all rows in array
    for (auto &rowId : (*innerIt).map[inner.rel])  
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


    for (auto &buffer : results) {
        for (std::size_t i = 0; i < buffer.size(); i++) {
            // Does sorting really matter? (i dont think so)

            std::vector<std::size_t> indexes = findIndexes((*innerIt).map[inner.rel], buffer[i].key1);

            for (int j = 0; j < indexes.size(); j++) {

                for (auto &item : (*innerIt).map) {
                    map[item.first].push_back( item.second[indexes[j]] );
                }
                map[outer.rel].push_back(buffer[i].key2);
            }
        }
    }


    (*innerIt).map = map;
    (*innerIt).columnSize = map.begin()->second.size();
    (*innerIt).columnNum = map.size();

    right.tuples = nullptr;
}

void RHJ::Executioner::internalJoin(const Query& query, Query::Predicate::Operand inner, IntermediateResults::iterator innerIt,
                                                        Query::Predicate::Operand outer, IntermediateResults::iterator outerIt) 
{

    RHJ::Relation left;
    left.size = (*innerIt).columnSize;
    left.tuples = new Relation::Tuple[(*innerIt).columnSize];

    std::size_t i = 0;
    // Iterate over all rows in array
    for (auto &rowId : (*innerIt).map[inner.rel])  
    {
        left.tuples[i].key = rowId;
        left.tuples[i].payload = Relations.relations[ query.relations[inner.rel] ].column(inner.col)[rowId].payload;
        i++;
    }

    RHJ::Relation right;
    right.size = (*outerIt).columnSize;
    right.tuples = new Relation::Tuple[(*outerIt).columnSize];

    i = 0;
    // Iterate over all rows in array
    for (auto &rowId : (*outerIt).map[outer.rel])  
    {
        right.tuples[i].key = rowId;
        right.tuples[i].payload = Relations.relations[ query.relations[outer.rel] ].column(outer.col)[rowId].payload;
        i++;
    }

    RHJ::Results results = RHJ::Relation::RadixHashJoin(left, right);

    std::unordered_map<std::size_t, std::vector<tuple_key_t>> map;

    
    for (auto &buffer : results) {
        for (std::size_t i = 0; i < buffer.size(); i++) {

            // Does sorting really matter? (i dont think so)

            std::vector<std::size_t> innerIndexes = findIndexes((*innerIt).map[inner.rel], buffer[i].key1);
            std::vector<std::size_t> outerIndexes = findIndexes((*outerIt).map[outer.rel], buffer[i].key2);

            for (int j = 0; j < innerIndexes.size(); j++) {

                for (int k = 0; k < outerIndexes.size(); k++) {

                    for (auto &item : (*innerIt).map) {
                        map[item.first].push_back( item.second[innerIndexes[j]] );
                    }
                    
                    for (auto &item : (*outerIt).map) {
                        map[item.first].push_back( item.second[outerIndexes[k]] );
                    }  
                }
            
            }
        }
    }

    (*innerIt).map = map;
    (*innerIt).columnNum = map.size();
    if ((*innerIt).columnNum)
        (*innerIt).columnSize = map.begin()->second.size();
    else (*innerIt).columnSize = 0;

    this->inteResults.erase(outerIt);

    right.tuples = nullptr;
    left.tuples = nullptr;
}


void RHJ::Executioner::internalSelfJoin(const Query& query, Query::Predicate::Operand inner, IntermediateResults::iterator innerIt, 
                                                            Query::Predicate::Operand outer) 
{
    // Iterate over all rows in array
    for (int i = 0; i < (*innerIt).columnSize; i++ ) 
    {
        // For each one get its value from its appropriate relation and check if filter is true
        int rowID_1 = (*innerIt).map[inner.rel][i];
        int rowID_2 = (*innerIt).map[outer.rel][i];

        // So we want something like:
        tuple_payload_t value_1 = Relations.relations[ query.relations[inner.rel] ].column(inner.col)[rowID_1].payload;
        tuple_payload_t value_2 = Relations.relations[ query.relations[outer.rel] ].column(outer.col)[rowID_2].payload;

        if (!compare(value_1, value_2, Query::Predicate::Type::filter_eq_t)) {

            // if comparison returns false then remove the value from the vector
            for (auto &vec : (*innerIt).map) {
                std::vector<tuple_key_t>::iterator ith = vec.second.begin() + i;
                vec.second.erase(ith);
            }

            // Decreasing iterators because we erased one value
            i--;
            (*innerIt).columnSize--;
        }
    }
}

void RHJ::Executioner::externalSelfJoin(const Query& query, Query::Predicate::Operand inner, Query::Predicate::Operand outer) {

    int columnSize = Relations.relations[ query.relations[inner.rel] ].columnSize;;

    std::vector<tuple_key_t> filteredVector;

    // Iterate over whole column
    for (std::size_t i = 0; i < columnSize; i++) {

        int value_1 = Relations.relations[ query.relations[inner.rel] ].column(inner.col)[i].payload;
        int value_2 = Relations.relations[ query.relations[outer.rel] ].column(outer.col)[i].payload;

        if (compare(value_1, value_2, Query::Predicate::Type::filter_eq_t)) {
            
            filteredVector.push_back(i);
        }
    }

    std::unordered_map<std::size_t, std::vector<tuple_key_t>> map;
    map[inner.rel] = filteredVector;

    this->inteResults.emplace_back(map.size(), filteredVector.size(), map);
}


bool RHJ::Executioner::compare(tuple_payload_t u, tuple_payload_t v, Query::Predicate::Type op) {
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