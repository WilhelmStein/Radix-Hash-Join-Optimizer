
#include <meta.hpp>
#include <executioner.hpp>
#include <relation.hpp>
#include <list.hpp>
#include <statistics.hpp>
#include <join_enumerator.hpp>

#include <iostream>
#include <algorithm>
#include <unordered_set>

#define PUSH_RESULTS(self, results, pusher_1, arg_1, pusher_2, arg_2)       \
do {                                                                        \
    for (const auto &buffer : results) {                                    \
        for (std::size_t i = 0; i < buffer.size(); i++) {                   \
            pusher_1(self, arg_1, buffer[i].key1)                           \
            pusher_2(self, arg_2, buffer[i].key2)                           \
        }                                                                   \
    }                                                                       \
} while (false)                                                             \

#define PUSH_RESULT(self, relation, value) self->inteResults.back().map[relation].push_back(value);

#define PUSH_INTERNAL_RESULTS(self, iterator, value)            \
for (const auto &item : (*iterator).map) {                      \
    PUSH_RESULT(self, item.first, item.second[value])           \
}                                                               \

#define N 10000000L

RHJ::Executioner::Entity::Entity(std::size_t _columnNum, std::size_t _columnSize, std::unordered_map<std::size_t, std::vector<tuple_key_t>> _map) 
:
columnSize(_columnSize), columnNum(_columnNum), map(_map) { }

RHJ::Executioner::Entity::~Entity() { }

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

    for (iterator it = begin(); it != end(); ++it) {
        
        if ( (*it).map.find(Rel) != (*it).map.end() ) {
            return it;
        }
    }

    return end();
}


RHJ::Relation RHJ::Executioner::getInternalData(const Query& query, Query::Predicate::Operand op, IntermediateResults::iterator it) {

    const Meta& relation = RHJ::meta[query.relations[op.rel]];
    tuple_payload_t * column = relation.columns[op.col];

    RHJ::Relation ret;
    ret.size = (*it).columnSize;
    ret.tuples = new Relation::Tuple[ ret.size ];

    tuple_key_t i = 0;
    for (auto &rowId : (*it).map[op.rel])  
    {
        ret.tuples[i].key = i;
        ret.tuples[i].payload = column[rowId];
        i++;
    }

    return ret;
}

RHJ::Relation RHJ::Executioner::getExternalData(const Query& query, Query::Predicate::Operand op) {

    const Meta& relation = RHJ::meta[query.relations[op.rel]];
    tuple_payload_t * column = relation.columns[op.col];

    RHJ::Relation ret;

    ret.size = relation.rowSize;
    ret.tuples = new Relation::Tuple[ ret.size ];

    for (tuple_key_t i = 0; i < ret.size; i++)
    {
        ret.tuples[i].key = i;
        ret.tuples[i].payload = column[i];
    }

    return ret;
}


std::vector<std::string> RHJ::Executioner::execute(const Query& query) {

    inteResults.clear();

    // std::sort(query.predicates, query.predicates + query.preCount, [](Query::Predicate a, Query::Predicate b) {
    //     if ( a.type == Query::Predicate::Type::join_t && b.type == Query::Predicate::Type::join_t ) {
    //         // check for self joins
    //         if ( a.left.rel == a.right.operand.rel ) return true;
    //         if ( b.left.rel == b.right.operand.rel ) return false;
    //     }

    //     return a.type < b.type;
    // });

    RHJ::JoinEnumerator enumerator(query);
    
    std::deque<RHJ::Query::Predicate> bestFormat = enumerator.generateBestCombination();

    for(std::size_t i = 0; i < query.preCount; i++)
        if( query.predicates[i].type != RHJ::Query::Predicate::Type::join_t ||
            query.predicates[i].right.operand.rel == query.predicates[i].left.rel)
            bestFormat.push_front(query.predicates[i]);


    bool nonzero = true;
    for (std::size_t i = 0; i < bestFormat.size() && nonzero; i++) {

        switch(bestFormat[i].type) {
            case Query::Predicate::Type::join_t:
                if (!executeJoin(query, bestFormat[i]))
                    nonzero = false;
                break;
            default:
                if (!executeFilter(query, bestFormat[i]))
                    nonzero = false;
                break;
        }
    }

    while (inteResults.size() > 1 && nonzero) {
        IntermediateResults::iterator left = inteResults.begin();
        IntermediateResults::iterator right = ++inteResults.begin();

        cartesianProduct(left, right);
    }

    return calculateCheckSums(query);
}


bool RHJ::Executioner::executeFilter(const Query& query, Query::Predicate pred) {

    std::size_t relation = pred.left.rel;
    std::size_t column = pred.left.col;
    tuple_payload_t immediate = pred.right.constraint;
    Query::Predicate::Type op = pred.type;

    IntermediateResults::iterator node = inteResults.find(pred.left.rel);

    #if !defined (__QUIET__)
        std::cerr << "\tExecuting Filter..  " << pred << std::endl;
    #endif

    if (node != inteResults.end() ) {

        #if !defined (__QUIET__)
            std::cerr << "\t\tInternal Filter produced: ";
        #endif

        std::unordered_map<std::size_t, std::vector<tuple_key_t> > map;
        inteResults.emplace_back(-1, -1, map);

        for (auto &vec : (*node).map) {
            std::vector<tuple_key_t> vector;
            map[vec.first] = vector;
        }

        for (std::size_t i = 0; i < (*node).columnSize; i++ ) 
        {
            tuple_key_t rowID = (*node).map[relation][i];

            tuple_payload_t value = RHJ::meta[query.relations[relation]].columns[column][rowID];

            if (compare(value, immediate, op)) {

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

        #if !defined (__QUIET__)
            std::cerr << "\t\tExternal Filter produced: ";
        #endif

        std::size_t columnSize = RHJ::meta[query.relations[relation]].rowSize;

        std::vector<tuple_key_t> filteredVector;

        for (std::size_t i = 0; i < columnSize; i++) {

            tuple_payload_t value = RHJ::meta[query.relations[relation]].columns[column][i];

            if (compare(value, immediate, op)) {
                
                filteredVector.push_back(i);
            }
        }

        std::unordered_map<std::size_t, std::vector<tuple_key_t>> map;
        // map[relation] = filteredVector;

        inteResults.emplace_back(map.size(), filteredVector.size(), map);
        inteResults.back().map[relation] = filteredVector;
    }

    #if !defined (__QUIET__)
        std::cerr << inteResults.back().columnSize << " rows\n" << std::endl;
    #endif

    return inteResults.back().columnSize > 0;
}

bool RHJ::Executioner::executeJoin(const Query& query, Query::Predicate pred) {

    // External Join:       2 relations that do not exist in intermediate results
    // Internal Join:       2 relations that BOTH exist in intermediate results
    // Semi-Internal Join:  1 relation in intermediate results. 1 relation not in intermediate results.
    // Internal Self Join:  1 relation self join that exists in intermediate results
    // External Self Join:  1 relation self join that does not exist in intermediate results
    // 
    // Practically, the only difference here is the source of the data used for joins.

    #if !defined (__QUIET__)
        std::cerr << "\tExecuting Join..  " << pred << std::endl;
    #endif

    utility::pair<RHJ::Executioner::IntermediateResults::iterator, RHJ::Executioner::IntermediateResults::iterator> nodes;
    nodes = inteResults.find(pred.left.rel, pred.right.operand.rel);

    bool left = nodes.first != inteResults.end();
    bool right = nodes.second != inteResults.end();

    if (left || right) {
        if (left && !right) 
            return semiInternalJoin(query, pred.left, nodes.first, pred.right.operand);

        else if (right && !left) 
            return semiInternalJoin(query, pred.right.operand, nodes.second, pred.left);
            
        else {
            if (nodes.first == nodes.second) {
                return internalSelfJoin(query, pred.left, nodes.first, pred.right.operand);
            }
            else {
                return internalJoin(query, pred.left, nodes.first, pred.right.operand, nodes.second);
            }
        } 
    }
    else {
        if (pred.left.rel == pred.right.operand.rel) {
            return externalSelfJoin(query, pred.left, pred.right.operand);
        }
        else
            return externalJoin(query, pred.left, pred.right.operand);
    }
}


bool RHJ::Executioner::externalJoin(const Query& query, Query::Predicate::Operand inner, Query::Predicate::Operand outer) {
    
    #if !defined (__QUIET__)
        std::cerr << "\t\tExternal Join produced: ";
    #endif


    RHJ::Relation left = getExternalData(query, inner);
    RHJ::Relation right = getExternalData(query, outer);


    RHJ::Results results = RHJ::Relation::RadixHashJoin(left, right);

    std::unordered_map<std::size_t, std::vector<tuple_key_t>> map;

    inteResults.emplace_back(-1, -1, map);
    inteResults.back().map[inner.rel] = std::vector<tuple_key_t>();
    inteResults.back().map[outer.rel] = std::vector<tuple_key_t>();

    PUSH_RESULTS(this, results, PUSH_RESULT, inner.rel, PUSH_RESULT, outer.rel);

    inteResults.back().columnNum = inteResults.back().map.size();
    inteResults.back().columnSize = inteResults.back().map.begin()->second.size();

    #if !defined (__QUIET__)
        std::cerr << inteResults.back().columnSize << " rows\n" << std::endl;
    #endif

    return inteResults.back().columnSize > 0;
    // delete[] left.tuples;
    // delete[] right.tuples;
}


bool RHJ::Executioner::semiInternalJoin(const Query& query, Query::Predicate::Operand inner, IntermediateResults::iterator innerIt,
                                        Query::Predicate::Operand outer) 
{
    #if !defined (__QUIET__)
        std::cerr << "\t\tSemi-Internal Join produced: ";
    #endif


    RHJ::Relation left = getInternalData(query, inner, innerIt);
    RHJ::Relation right = getExternalData(query, outer);

    
    RHJ::Results results = RHJ::Relation::RadixHashJoin(left, right);

    std::unordered_map<std::size_t, std::vector<tuple_key_t>> map;
    inteResults.emplace_back(-1, -1, map);


    // Initializing keys in case results are empty
    for (auto &item : (*innerIt).map) {
        inteResults.back().map[item.first] = std::vector<tuple_key_t>();
    }
    inteResults.back().map[outer.rel] = std::vector<tuple_key_t>();

    PUSH_RESULTS(this, results, PUSH_INTERNAL_RESULTS, innerIt, PUSH_RESULT, outer.rel);

    inteResults.back().columnNum = inteResults.back().map.size();
    if (inteResults.back().columnNum)
        inteResults.back().columnSize =  inteResults.back().map.begin()->second.size();
    else
        inteResults.back().columnSize = 0;
    inteResults.erase(innerIt);

    #if !defined (__QUIET__)
        std::cerr << inteResults.back().columnSize << " rows\n" << std::endl;
    #endif

    return inteResults.back().columnSize > 0;
    // (*innerIt).map = map;
    // (*innerIt).columnNum = map.size();
    // if ((*innerIt).columnNum)
    //     (*innerIt).columnSize = map.begin()->second.size();
    // else (*innerIt).columnSize = 0;
    // (*innerIt).columnNum = map.size();

    // delete[] left.tuples;
    // delete[] right.tuples;
}

bool RHJ::Executioner::internalJoin(const Query& query, Query::Predicate::Operand inner, IntermediateResults::iterator innerIt,
                                                        Query::Predicate::Operand outer, IntermediateResults::iterator outerIt) 
{
    #if !defined (__QUIET__)
        std::cerr << "\t\tInternal Join produced: ";
    #endif


    RHJ::Relation left = getInternalData(query, inner, innerIt);
    RHJ::Relation right = getInternalData(query, outer, outerIt);


    RHJ::Results results = RHJ::Relation::RadixHashJoin(left, right);

    std::unordered_map<std::size_t, std::vector<tuple_key_t>> map;
    inteResults.emplace_back(-1, -1, map);


    // Initializing keys in case results are empty
    for (auto &item : (*innerIt).map) {
        inteResults.back().map[item.first] = std::vector<tuple_key_t>();
    }
    for (auto &item : (*outerIt).map) {
        inteResults.back().map[item.first] = std::vector<tuple_key_t>();
    }

    PUSH_RESULTS(this, results, PUSH_INTERNAL_RESULTS, innerIt, PUSH_INTERNAL_RESULTS, outerIt);  

    inteResults.back().columnNum = inteResults.back().map.size();
    if (inteResults.back().columnNum)
        inteResults.back().columnSize =  inteResults.back().map.begin()->second.size();
    else
        inteResults.back().columnSize = 0;
    inteResults.erase(innerIt);
    
    #if !defined (__QUIET__)
        std::cerr << inteResults.back().columnSize << " rows\n" << std::endl;
    #endif
    

    // (*innerIt).map = map;
    // (*innerIt).columnNum = map.size();

    // if ((*innerIt).columnNum)
    //     (*innerIt).columnSize = map.begin()->second.size();
    // else 
    //     (*innerIt).columnSize = 0;

    inteResults.erase(outerIt);
    return inteResults.back().columnSize > 0;
    
}


bool RHJ::Executioner::internalSelfJoin(const Query& query, Query::Predicate::Operand inner, IntermediateResults::iterator innerIt, 
                                                            Query::Predicate::Operand outer) 
{
    #if !defined (__QUIET__)
        std::cerr << "\t\tInternal Self-Join produced: ";
    #endif

    const Meta& innerRelation = RHJ::meta[query.relations[inner.rel]];
    const Meta& outerRelation = RHJ::meta[query.relations[outer.rel]];
    tuple_payload_t * innerColumn = innerRelation.columns[inner.col];
    tuple_payload_t * outerColumn = outerRelation.columns[outer.col];


    std::unordered_map<std::size_t, std::vector<tuple_key_t> > map;
    inteResults.emplace_back(-1, -1, map);

    for (auto &item : (*innerIt).map) {
        map[item.first] = std::vector<tuple_key_t>();
    }

    for (std::size_t i = 0; i < (*innerIt).columnSize; i++ ) 
    { 
        tuple_key_t rowID_1 = (*innerIt).map[inner.rel][i];
        tuple_key_t rowID_2 = (*innerIt).map[outer.rel][i];

        tuple_payload_t value_1 = innerColumn[rowID_1];
        tuple_payload_t value_2 = outerColumn[rowID_2];

        if (value_1 == value_2) {

            for (auto &item : (*innerIt).map) {
                inteResults.back().map[item.first].push_back(item.second[i]);
            }

        }
    }

    inteResults.back().columnNum = inteResults.back().map.size();
    if (inteResults.back().columnNum)
        inteResults.back().columnSize =  inteResults.back().map.begin()->second.size();
    else
        inteResults.back().columnSize = 0;
    inteResults.erase(innerIt);

    #if !defined (__QUIET__)
        std::cerr << inteResults.back().columnSize << " rows\n" << std::endl;
    #endif

    return inteResults.back().columnSize > 0;

    // (*innerIt).map = map;
    // (*innerIt).columnSize = map[inner.rel].size();
}

bool RHJ::Executioner::externalSelfJoin(const Query& query, Query::Predicate::Operand inner, Query::Predicate::Operand outer) {

    #if !defined (__QUIET__)
        std::cerr << "\t\tExternal Self-Join produced: ";
    #endif

    const Meta& innerRelation = RHJ::meta[query.relations[inner.rel]];
    const Meta& outerRelation = RHJ::meta[query.relations[outer.rel]];
    tuple_payload_t * innerColumn = innerRelation.columns[inner.col];
    tuple_payload_t * outerColumn = outerRelation.columns[outer.col];

    std::size_t columnSize = innerRelation.rowSize;
  
    std::vector<tuple_key_t> filteredVector;

    // Iterate over whole column
    for (std::size_t i = 0; i < columnSize; i++) {

        tuple_payload_t value_1 = innerColumn[i];
        tuple_payload_t value_2 = outerColumn[i];

        if (compare(value_1, value_2, Query::Predicate::Type::filter_eq_t)) {
            
            filteredVector.push_back(i);
        }
    }

    std::unordered_map<std::size_t, std::vector<tuple_key_t>> map;
    // map[inner.rel] = filteredVector;

    inteResults.emplace_back(map.size(), filteredVector.size(), map);
    inteResults.back().map[inner.rel] = filteredVector;

    #if !defined (__QUIET__)
        std::cerr << inteResults.back().columnSize << " rows\n" << std::endl;
    #endif

    return inteResults.back().columnSize > 0;
}

bool RHJ::Executioner::cartesianProduct(IntermediateResults::iterator left, IntermediateResults::iterator right) {

    #if !defined (__QUIET__)
        std::cerr << "\tCartesian Product produced: ";
    #endif

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

    inteResults.erase(right);

    #if !defined (__QUIET__)
        std::cerr << inteResults.back().columnSize << " rows\n" << std::endl;
    #endif

    return inteResults.back().columnSize > 0;
}

std::vector<std::string> RHJ::Executioner::calculateCheckSums(const Query& query) {
    std::vector<std::string> ret;

    for (std::size_t i = 0; i < query.cheCount; i++) {

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
        case Query::Predicate::Type::filter_gt_t:
            return u > v;
        case Query::Predicate::Type::filter_lt_t:
            return u < v;
        default:
            return false;
    }
}
