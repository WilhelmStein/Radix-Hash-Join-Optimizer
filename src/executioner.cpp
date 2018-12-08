
#include <executioner.hpp>
#include <relation.hpp>
#include <list.hpp>

#include <iostream>
#include <algorithm>

#include <cstring>
#include <climits>
#include <cassert>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <sys/mman.h>

#include <fcntl.h>

// Macro used in order to exit if condition is met
#define exit_if(condition, message)                 \
do                                                  \
{                                                   \
    if (condition)                                  \
    {                                               \
        std::perror(message); std::exit(errno);     \
    }                                               \
} while (false)                                     \

// Macro definition for consistency with above declaration
#define exit_if_not(condition) assert(condition)

static struct Meta
{
    __off_t mappingSize;
    void * mapping;

    tuple_key_t rowSize, columnSize;
    tuple_payload_t ** columns;
} * meta = nullptr;

static std::size_t total = 0UL;

void RHJ::Executioner::createMetadata()
{
    utility::list<char *> paths;

    // Throw exception if any of the following flags are set for std::cin
    std::cin.exceptions(std::ios_base::badbit | std::ios_base::failbit);

    while(true)
    {
        char buff[PATH_MAX + 1UL];
        
        // Atempt to read the current line
        try
        {
            std::cin.getline(buff, sizeof(buff));
        }
        catch (std::ios_base::failure& f)
        {
            std::cerr
            << "iteration: "  << paths.size()
            << " exception: " << f.what()
            << std::endl;

            std::exit(f.code().value());
        }

        if (!std::strcmp(buff, "Done"))
            break;
        
        paths.push_back(new char[std::strlen(buff) + 1UL]);
        
        std::strcpy(paths.back(), buff);
    }
    
    total = paths.size(); exit_if_not(total > 0UL);

    meta = new Meta[total];

    for (std::size_t i = 0UL; i < total; i++)
    {
        // Open file
        int fd = open(paths.front(), O_RDONLY);
        exit_if(fd < 0, paths.front());

        // Retrieve its size
        struct stat st;
        exit_if(fstat(fd, &st) < 0, paths.front());

        // Map file to memory
        meta[i].mapping = mmap(nullptr, (meta[i].mappingSize = st.st_size), PROT_READ, MAP_PRIVATE, fd, 0);
        exit_if(meta[i].mapping == MAP_FAILED, paths.front());

        // Close file
        exit_if(close(fd) < 0, paths.front());

        // Deallocate uneccessary memory
        delete[] paths.front(); paths.erase(paths.begin());

        // Copy Metadata
        void * mapping_clone = meta[i].mapping;

        void * rsptr = reinterpret_cast<void *>(&meta[i].rowSize);
        std::memmove(rsptr, mapping_clone, sizeof(tuple_key_t));
        reinterpret_cast<tuple_key_t *&>(mapping_clone)++;

        void * csptr = reinterpret_cast<void *>(&meta[i].columnSize);
        std::memmove(csptr, mapping_clone, sizeof(tuple_key_t));
        reinterpret_cast<tuple_key_t *&>(mapping_clone)++;
        
        // Create Index
        meta[i].columns = new tuple_payload_t*[meta[i].columnSize]; 
        for(tuple_key_t j = 0UL; j < meta[i].columnSize; j++)
        {
            const tuple_key_t index = j * meta[i].rowSize;

            tuple_payload_t * const mapping = &(reinterpret_cast<tuple_payload_t*>(mapping_clone)[index]);

            meta[i].columns[j] = mapping;
        }
    }
}

void RHJ::Executioner::deleteMetadata()
{
    for(std::size_t i = 0; i < total; i++)
    {
        exit_if(munmap(meta[i].mapping, meta[i].mappingSize) < 0, ("munmap No." + std::to_string(i)).c_str());
        
        delete[] meta[i].columns;
    }

    delete[] meta;
}


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

    const Meta& relation = meta[query.relations[op.rel]];
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

    const Meta& relation = meta[query.relations[op.rel]];
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

    if (!inteResults.empty())
        inteResults.clear();

    std::sort(query.predicates, query.predicates + query.preCount, [](Query::Predicate a, Query::Predicate b) {
        if ( a.type == Query::Predicate::Type::join_t && b.type == Query::Predicate::Type::join_t ) {
            // check for self joins
            if ( a.left.rel == a.right.operand.rel ) return true;
            if ( b.left.rel == b.right.operand.rel ) return false;
        }

        return a.type < b.type;
    });
    
    bool nonzero = true;
    for (std::size_t i = 0; i < query.preCount && nonzero; i++) {

        switch(query.predicates[i].type) {
            case Query::Predicate::Type::join_t:
                if (!executeJoin(query, query.predicates[i]))
                    nonzero = false;
                break;
            default:
                if (!executeFilter(query, query.predicates[i]))
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

            tuple_payload_t value = meta[query.relations[relation]].columns[column][rowID];

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

        std::size_t columnSize = meta[query.relations[relation]].rowSize;

        std::vector<tuple_key_t> filteredVector;

        for (std::size_t i = 0; i < columnSize; i++) {

            tuple_payload_t value = meta[query.relations[relation]].columns[column][i];

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

    std::vector<tuple_key_t> leftVector;
    std::vector<tuple_key_t> rightVector;
    

    for (auto &buffer : results) {
        for (std::size_t i = 0; i < buffer.size(); i++) {

            leftVector.push_back(buffer[i].key1);
            rightVector.push_back(buffer[i].key2);
            
        }
    }

    std::unordered_map<std::size_t, std::vector<tuple_key_t>> map;

    inteResults.emplace_back(map.size(), leftVector.size(), map);
    inteResults.back().map[inner.rel] = leftVector;
    inteResults.back().map[outer.rel] = rightVector;

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

    
    for (auto &buffer : results) {

        for (std::size_t i = 0; i < buffer.size(); i++) {

            for (auto &item : (*innerIt).map) {

                tuple_key_t innerRowID = item.second[buffer[i].key1];
                inteResults.back().map[item.first].push_back( innerRowID );
            }

            tuple_key_t outerRowID = buffer[i].key2;
            inteResults.back().map[outer.rel].push_back( outerRowID );
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


    for (auto &buffer : results) {
        for (std::size_t i = 0; i < buffer.size(); i++) {

            for (auto &item : (*innerIt).map) {
                tuple_key_t innerRowID = item.second[buffer[i].key1];
                inteResults.back().map[item.first].push_back( innerRowID );
            }
            
            for (auto &item : (*outerIt).map) {
                tuple_key_t outerRowID = item.second[buffer[i].key2];
                inteResults.back().map[item.first].push_back( outerRowID );
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

    const Meta& innerRelation = meta[query.relations[inner.rel]];
    const Meta& outerRelation = meta[query.relations[outer.rel]];
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

    const Meta& innerRelation = meta[query.relations[inner.rel]];
    const Meta& outerRelation = meta[query.relations[outer.rel]];
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
            sum += meta[ query.relations[query.checksums[i].rel] ].columns[query.checksums[i].col][rowId];
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
