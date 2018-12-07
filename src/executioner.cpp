#include <executioner.hpp>
#include <relation.hpp>
#include <cstdlib>
#include <iostream>
#include <algorithm>

RHJ::Executioner::Entity::Entity(std::size_t _columnNum, std::size_t _columnSize, std::unordered_map<std::size_t, std::vector<tuple_key_t>> _map) 
:
columnNum(_columnNum), columnSize(_columnSize), map(_map)
{

}

RHJ::Executioner::Entity::~Entity() {
    
}

RHJ::Executioner::IntermediateResults::IntermediateResults() : list() { }

RHJ::Executioner::IntermediateResults::~IntermediateResults() { 

}

RHJ::Executioner::Executioner() : inteResults() { }

RHJ::Executioner::~Executioner() { 

}


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

    for (iterator it = begin(); it != end(); ++it) {
        
        if ( (*it).map.find(Rel) != (*it).map.end() ) {
            return it;
        }
    }

    return end();
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

std::unordered_map<tuple_key_t, std::vector<std::size_t>> mapIndexes(std::vector<tuple_key_t> vec) {
    std::unordered_map<tuple_key_t, std::vector<std::size_t>> map;
    std::size_t size = vec.size();

    for (std::size_t i = 0; i < size; i++) {
        map[vec[i]].push_back(i);
    }

    return map;
}


std::vector<std::string> RHJ::Executioner::execute(const Query& query) {

    std::sort(query.predicates, query.predicates + query.preCount, [](Query::Predicate a, Query::Predicate b) {
        if ( a.type == Query::Predicate::Type::join_t && b.type == Query::Predicate::Type::join_t ) {
            // check for self joins
            if ( a.left.rel == a.right.operand.rel ) return true;
            if ( b.left.rel == b.right.operand.rel ) return false;
        }

        return a.type < b.type;
    });

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

    while (inteResults.size() > 1) {
        IntermediateResults::iterator left = inteResults.begin();
        IntermediateResults::iterator right = ++inteResults.begin();

        cartesianProduct(left, right);
    }

    return calculateCheckSums(query);
}

void RHJ::Executioner::executeFilter(const Query& query, Query::Predicate pred) {

    // SUBJECT TO CHANGE //
    int relation = pred.left.rel;
    int column = pred.left.col;
    int immediate = pred.right.constraint;
    Query::Predicate::Type op = pred.type;
    // SUBJECT TO CHANGE //

    IntermediateResults::iterator node = inteResults.find(pred.left.rel);
    std::cout << "    Executing Filter..  " << pred << std::endl;

    if (node != inteResults.end() ) {

        std::cout << "        Internal Filter produced: ";

        std::unordered_map<std::size_t, std::vector<tuple_key_t> > map;
        inteResults.emplace_back(-1, -1, map);

        for (auto &vec : (*node).map) {
            std::vector<tuple_key_t> vector;
            map[vec.first] = vector;
        }

        // Iterate over all rows in array
        for (std::size_t i = 0; i < (*node).columnSize; i++ ) 
        {
            // For each one get its value from its appropriate relation and check if filter is true
            tuple_key_t rowID = (*node).map[relation][i];

            // So we want something like:
            tuple_payload_t value = RHJ::meta[query.relations[relation]].columns[column][rowID];
            // int value = Relations.relations[ query.relations[relation] ].column(column)[rowID].payload;

            if (compare(value, immediate, op)) {

                // if comparison returns false then remove the value from the vector
                for (auto &vec : (*node).map) {
                    inteResults.back().map[vec.first].push_back(vec.second[i]);
                }
            }
        }

        inteResults.back().columnNum = inteResults.back().map.size();
        if (inteResults.back().columnNum) {
            inteResults.back().columnSize = inteResults.back().map.begin()->second.size();
        }
        else
            inteResults.back().columnSize = 0;
        inteResults.erase(node);


        // (*node).map = map;
        // (*node).columnSize = map[relation].size();
    }
    else {
        // construct new node
        std::cout << "        External Filter produced: ";
        int columnSize = RHJ::meta[query.relations[relation]].rowSize;

        std::vector<tuple_key_t> filteredVector;

        // Iterate over whole column
        for (std::size_t i = 0; i < columnSize; i++) {

            tuple_payload_t value = RHJ::meta[query.relations[relation]].columns[column][i];

            if (compare(value, immediate, op)) {
                
                filteredVector.push_back(i);
            }
        }

        std::unordered_map<std::size_t, std::vector<tuple_key_t>> map;
        // map[relation] = filteredVector;

        this->inteResults.emplace_back(map.size(), filteredVector.size(), map);
        this->inteResults.back().map[relation] = filteredVector;
    }

    std::cout << inteResults.back().columnSize << " rows" << std::endl  << std::endl;
}

void RHJ::Executioner::executeJoin(const Query& query, Query::Predicate pred) {
    std::cout << "    Executing Join..  " << pred << std::endl;

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
    std::cout << "        External Join produced: ";

    int leftColumn = inner.col;
    int rightColumn = outer.col;

    RHJ::Relation left;
    left.size = RHJ::meta[query.relations[inner.rel]].rowSize;
    left.tuples = new Relation::Tuple[ left.size ];

    for (tuple_key_t i = 0; i < left.size; i++)
    {
        left.tuples[i].key = i;
        left.tuples[i].payload = RHJ::meta[query.relations[inner.rel]].columns[leftColumn][i];
    }

    RHJ::Relation right;
    right.size = RHJ::meta[query.relations[outer.rel]].rowSize;
    right.tuples = new Relation::Tuple[ right.size ];

    for (tuple_key_t i = 0; i < right.size; i++)  
    {
        right.tuples[i].key = i;
        right.tuples[i].payload = RHJ::meta[query.relations[outer.rel]].columns[rightColumn][i];
    }

    
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

    inteResults.emplace_back(map.size(), leftVector.size(), map);
    inteResults.back().map[inner.rel] = leftVector;
    inteResults.back().map[outer.rel] = rightVector;

    std::cout << inteResults.back().columnSize << " rows" << std::endl  << std::endl;

    // delete[] left.tuples;
    // delete[] right.tuples;
}


void RHJ::Executioner::semiInternalJoin(const Query& query, Query::Predicate::Operand inner, IntermediateResults::iterator innerIt,
                                        Query::Predicate::Operand outer) 
{
    std::cout << "        Semi-Internal Join produced: ";
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
        left.tuples[i].payload = RHJ::meta[query.relations[inner.rel]].columns[leftColumn][rowId];
        i++;
    }

    RHJ::Relation right;
    right.size = RHJ::meta[query.relations[outer.rel]].rowSize;
    right.tuples = new Relation::Tuple[ RHJ::meta[query.relations[outer.rel]].rowSize ];

    for (tuple_key_t i = 0; i < RHJ::meta[query.relations[outer.rel]].rowSize; i++)  
    {
        right.tuples[i].key = i;
        right.tuples[i].payload = RHJ::meta[query.relations[outer.rel]].columns[rightColumn][i];
    }

    
    RHJ::Results results = RHJ::Relation::RadixHashJoin(left, right);

    std::unordered_map<std::size_t, std::vector<tuple_key_t>> map;
    std::unordered_map<tuple_key_t, std::vector<std::size_t>> indexes = mapIndexes( (*innerIt).map[inner.rel] );

    this->inteResults.emplace_back(-1, -1, map);
    

    for (auto &buffer : results) {
        for (std::size_t i = 0; i < buffer.size(); i++) {
            // Does sorting really matter? (i dont think so)

            for (int j = 0; j < indexes[buffer[i].key1].size(); j++) {

                for (auto &item : (*innerIt).map) {
                    inteResults.back().map[item.first].push_back( item.second[indexes[buffer[i].key1][j]] );
                }
                inteResults.back().map[outer.rel].push_back(buffer[i].key2);
            }
        }
    }

    inteResults.back().columnNum = inteResults.back().map.size();
    if (inteResults.back().columnNum)
        inteResults.back().columnSize =  inteResults.back().map.begin()->second.size();
    else
        inteResults.back().columnSize = 0;
    inteResults.erase(innerIt);
    std::cout << inteResults.back().columnSize << " rows" << std::endl  << std::endl;
    // (*innerIt).map = map;
    // (*innerIt).columnNum = map.size();
    // if ((*innerIt).columnNum)
    //     (*innerIt).columnSize = map.begin()->second.size();
    // else (*innerIt).columnSize = 0;
    // (*innerIt).columnNum = map.size();

    // delete[] left.tuples;
    // delete[] right.tuples;
}

void RHJ::Executioner::internalJoin(const Query& query, Query::Predicate::Operand inner, IntermediateResults::iterator innerIt,
                                                        Query::Predicate::Operand outer, IntermediateResults::iterator outerIt) 
{
    std::cout << "        Internal Join produced: ";
    RHJ::Relation left;
    left.size = (*innerIt).columnSize;
    left.tuples = new Relation::Tuple[(*innerIt).columnSize];

    std::size_t i = 0;
    // Iterate over all rows in array
    for (auto &rowId : (*innerIt).map[inner.rel])  
    {
        left.tuples[i].key = rowId;
        left.tuples[i].payload = RHJ::meta[query.relations[inner.rel]].columns[inner.col][rowId];
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
        right.tuples[i].payload = RHJ::meta[query.relations[outer.rel]].columns[outer.col][rowId];
        i++;
    }

    RHJ::Results results = RHJ::Relation::RadixHashJoin(left, right);

    std::unordered_map<std::size_t, std::vector<tuple_key_t>> map;
    this->inteResults.emplace_back(-1, -1, map);

    
    for (auto &buffer : results) {
        for (std::size_t i = 0; i < buffer.size(); i++) {

            // Does sorting really matter? (i dont think so)

            std::unordered_map<tuple_key_t, std::vector<std::size_t>> innerIndexes = mapIndexes( (*innerIt).map[inner.rel] );
            std::unordered_map<tuple_key_t, std::vector<std::size_t>> outerIndexes = mapIndexes( (*outerIt).map[outer.rel] );


            for (int j = 0; j < innerIndexes[buffer[i].key1].size(); j++) {

                for (int k = 0; k < outerIndexes[buffer[i].key2].size(); k++) {

                    for (auto &item : (*innerIt).map) {
                        inteResults.back().map[item.first].push_back( item.second[innerIndexes[buffer[i].key1][j]] );
                    }
                    
                    for (auto &item : (*outerIt).map) {
                        inteResults.back().map[item.first].push_back( item.second[outerIndexes[buffer[i].key2][k]] );
                    }  
                }
            
            }
        }
    }

    inteResults.back().columnNum = inteResults.back().map.size();
    if (inteResults.back().columnNum)
        inteResults.back().columnSize =  inteResults.back().map.begin()->second.size();
    else
        inteResults.back().columnSize = 0;
    inteResults.erase(innerIt);
    std::cout << inteResults.back().columnSize << " rows" << std::endl  << std::endl;

    // (*innerIt).map = map;
    // (*innerIt).columnNum = map.size();

    // if ((*innerIt).columnNum)
    //     (*innerIt).columnSize = map.begin()->second.size();
    // else 
    //     (*innerIt).columnSize = 0;

    this->inteResults.erase(outerIt);
}


void RHJ::Executioner::internalSelfJoin(const Query& query, Query::Predicate::Operand inner, IntermediateResults::iterator innerIt, 
                                                            Query::Predicate::Operand outer) 
{
    std::cout << "        Internal Self-Join produced: ";
    // Iterate over all rows in array
    std::unordered_map<std::size_t, std::vector<tuple_key_t> > map;
    this->inteResults.emplace_back(-1, -1, map);

    for (auto &vec : (*innerIt).map) {
        std::vector<tuple_key_t> vector;
        map[vec.first] = vector;
    }

    for (std::size_t i = 0; i < (*innerIt).columnSize; i++ ) 
    { 
        // For each one get its value from its appropriate relation and check if filter is true
        tuple_key_t rowID_1 = (*innerIt).map[inner.rel][i];
        tuple_key_t rowID_2 = (*innerIt).map[outer.rel][i];

        // So we want something like:
        tuple_payload_t value_1 = RHJ::meta[ query.relations[inner.rel] ].columns[inner.col][rowID_1];
        tuple_payload_t value_2 = RHJ::meta[ query.relations[outer.rel] ].columns[outer.col][rowID_2];

        if (value_1 == value_2) {

            // if comparison returns false then remove the value from the vector
            for (auto &vec : (*innerIt).map) {
                inteResults.back().map[vec.first].push_back(vec.second[i]);
            }
            // Decreasing iterators because we erased one value
        }
    }

    inteResults.back().columnNum = inteResults.back().map.size();
    if (inteResults.back().columnNum)
        inteResults.back().columnSize =  inteResults.back().map.begin()->second.size();
    else
        inteResults.back().columnSize = 0;
    inteResults.erase(innerIt);
    std::cout << inteResults.back().columnSize << " rows" << std::endl  << std::endl;


    // (*innerIt).map = map;
    // (*innerIt).columnSize = map[inner.rel].size();
}

void RHJ::Executioner::externalSelfJoin(const Query& query, Query::Predicate::Operand inner, Query::Predicate::Operand outer) {
    std::cout << "        External Self-Join produced: ";
    int columnSize = RHJ::meta[ query.relations[inner.rel] ].rowSize;

    std::vector<tuple_key_t> filteredVector;

    // Iterate over whole column
    for (std::size_t i = 0; i < columnSize; i++) {

        int value_1 = RHJ::meta[ query.relations[inner.rel] ].columns[inner.col][i];
        int value_2 = RHJ::meta[ query.relations[outer.rel] ].columns[outer.col][i];

        if (compare(value_1, value_2, Query::Predicate::Type::filter_eq_t)) {
            
            filteredVector.push_back(i);
        }
    }

    std::unordered_map<std::size_t, std::vector<tuple_key_t>> map;
    // map[inner.rel] = filteredVector;

    this->inteResults.emplace_back(map.size(), filteredVector.size(), map);
    inteResults.back().map[inner.rel] = filteredVector;
    std::cout << inteResults.back().columnSize << " rows" << std::endl  << std::endl;
}

void RHJ::Executioner::cartesianProduct(IntermediateResults::iterator left, IntermediateResults::iterator right) {

    std::cout << "    Cartesian Product produced: ";
    std::unordered_map<std::size_t, std::vector<tuple_key_t>> map;

    for (std::size_t i = 0; i < (*left).columnSize; i++) {

        for (std::size_t j = 0; j < (*right).columnSize; j++) {

            for (auto &item : (*left).map) {
                map[item.first].push_back(item.second[i]);
            }

            for (auto &item : (*right).map) {
                map[item.first].push_back(item.second[j]);   
            }
        }
    }

    (*left).map = map;
    (*left).columnNum = map.size();

    if ((*left).columnNum)
        (*left).columnSize = map.begin()->second.size();
    else 
        (*left).columnSize = 0;

    this->inteResults.erase(right);
    std::cout << inteResults.back().columnSize << " rows" << std::endl  << std::endl;
}

std::vector<std::string> RHJ::Executioner::calculateCheckSums(const Query& query) {
    std::vector<std::string> ret;

    for (int i = 0; i < query.cheCount; i++) {

        if ( (*inteResults.begin()).columnSize == 0 ) {
            ret.push_back("NULL");
            continue;
        }
        
        tuple_payload_t sum = 0;
        for (auto &rowId : (*inteResults.begin()).map[query.checksums[i].rel] ) {
            sum += RHJ::meta[ query.relations[query.checksums[i].rel] ].columns[query.checksums[i].col][rowId];
        }

        ret.push_back(std::to_string(sum));

    }

    return ret;
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