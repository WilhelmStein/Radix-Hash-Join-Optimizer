#include <join_enumerator.hpp>
#include <math.h>
#include <iostream>
#include <algorithm>

bool operator==(std::deque<std::size_t>& relA, std::deque<std::size_t>& relB) {
    bool allEqual = true;
    std::deque<std::size_t>::iterator ita, itb;

    if(relA.size() != relB.size())
        return false;

    for(  ita = relA.begin(), itb = relB.begin(); ita != relA.end() && itb != relB.end(); ita++, itb++ )
    {
        if(*ita != *itb)
        {
            allEqual = false;
            break;
        }
    }

    return allEqual;
}

bool findInDeque(const std::deque<std::size_t>& queue, const std::size_t& item)
{
    bool found = false;

    for( auto& element : queue ) {
        if(item == element)
        {
            found = true;
            break;
        }
    }

    return found;
}

#ifdef __REC_COST__

void RHJ::JoinEnumerator::recCost(
std::deque<std::size_t>& rels,
std::vector<float>& costs,
std::deque<std::size_t>& prevRels,
std::vector<RHJ::Query::Predicate>& predList,
std::size_t maxDepth,
std::size_t currDepth)
{
    if(currDepth == maxDepth)
    {

        std::size_t predArrSize = predList.size();

        RHJ::Query::Predicate* predArr = new RHJ::Query::Predicate[predArrSize];


        for( size_t i = 0; i < predArrSize; i++ )
            predArr[i] = predList[i];

        // Calculate the cost of the current predicate permutation
        costs.emplace_back( RHJ::Statistics::expected_cost(predArr, predArrSize) );

        delete[] predArr;
        return;
    }

    prevRels.emplace_back(rels[currDepth]);

    std::vector<RHJ::Query::Predicate> subPreds = connected(rels[currDepth + 1], prevRels);
    std::sort(subPreds.begin(), subPreds.end(),
            [](const RHJ::Query::Predicate &a, const RHJ::Query::Predicate &b) { return (a.left.rel < b.right.operand.rel); });

    do {

        for(std::size_t i = 0; i < subPreds.size(); i++)
        {
            //std::cout<<subPreds[i]<<std::endl;
            predList.emplace_back(subPreds[i]);
        }
        //std::cout<<std::endl;

        recCost(rels, costs, prevRels, predList, maxDepth, currDepth + 1);

        for(std::size_t i = 0; i < subPreds.size(); i++)
            predList.pop_back();
        
    } while (std::next_permutation(subPreds.begin(), subPreds.end(),
            [](const RHJ::Query::Predicate &a, const RHJ::Query::Predicate &b) { return (a.left.rel < b.left.rel); }));
    
    prevRels.pop_back();

}

float RHJ::JoinEnumerator::cost(std::deque<std::size_t> rels)
{

    // Converting the current permutation to specified format

    std::size_t relArrSize = rels.size();

    std::vector<RHJ::Query::Predicate> predList;
    std::deque<std::size_t> prevRels;
    std::vector<float> costs; 

    recCost(rels, costs, prevRels, predList, relArrSize - 1, 0);
    std::sort(costs.begin(), costs.end());

    return costs[0];
}

#else

float RHJ::JoinEnumerator::cost(std::deque<std::size_t> rels)
{

    // Converting the current permutation to specified format

    std::size_t relArrSize = rels.size(), predArrSize;

    std::vector<RHJ::Query::Predicate> predList;
    std::deque<std::size_t> prevRels;

    for( size_t i = 0; i < relArrSize - 1; i++ )
    {
        prevRels.emplace_back(rels[i]);

        std::vector<RHJ::Query::Predicate> subPreds = connected(rels[i + 1], prevRels);

        
        for(const auto& pred : subPreds)
            predList.emplace_back(pred);
        
    }

    predArrSize = predList.size();

    RHJ::Query::Predicate* predArr = new RHJ::Query::Predicate[predArrSize];


    for( size_t i = 0; i < predArrSize; i++ )
        predArr[i] = predList[i];

    // Calculate the cost of the current predicate permutation
    float cost = RHJ::Statistics::expected_cost(predArr, predArrSize);
    delete[] predArr;
    return cost;
}
#endif

RHJ::JoinEnumerator::JoinEnumerator(const RHJ::Query& query)
{
    // Start by applying all filters and self joins first on a copy of the starting statistics
    RHJ::Statistics::preprocess(query);

    for(std::size_t i = 0; i < query.preCount; i++)
    {
        // Disregard anything that is not a join or anything that is a self join
        if( query.predicates[i].type != RHJ::Query::Predicate::Type::join_t || 
            query.predicates[i].right.operand.rel == query.predicates[i].left.rel )
            continue;

        utility::pair<std::size_t, std::size_t> pair;

        // Sort predicate relation values in order to create a key representing a set
        if( query.predicates[i].left.rel < query.predicates[i].right.operand.rel )
        {
            pair.first = query.predicates[i].left.rel;
            pair.second = query.predicates[i].right.operand.rel;
        }
        else
        {
            pair.first = query.predicates[i].right.operand.rel;
            pair.second = query.predicates[i].left.rel;
        }

        // Initialize all predicate connections
        std::string relA = std::to_string(pair.first), relB = std::to_string(pair.second);
        connections[relA + "." + relB].emplace_back(query.predicates[i]);

        // Initialize all bestTrees for each relation
        bestTree[relA] = std::deque<std::size_t>{pair.first};
        bestTree[relB] = std::deque<std::size_t>{pair.second};

        // Initialize an ordered set containing all relations
        if(startSet.find(pair.first) == startSet.end())
            startSet.insert(pair.first);
        
        if(startSet.find(pair.second) == startSet.end())
            startSet.insert(pair.second);
    }
}

RHJ::JoinEnumerator::~JoinEnumerator()
{
    bestTree.clear();
    connections.clear();
}

std::deque<RHJ::Query::Predicate> RHJ::JoinEnumerator::generateBestCombination()
{
    std::vector<std::size_t> input;
    for(const auto& item : startSet)
        input.push_back(item);

    // Generate all possible permutations
    std::vector<std::deque<std::size_t>> possiblePermutations = RHJ::JoinEnumerator::generateSubCombinations(input);

    for (const auto& permutation : possiblePermutations)
    {
        // No need to check permutations the largest size, since we cannot append more elements to them
        if(permutation.size() == possiblePermutations[possiblePermutations.size() - 1].size())
            break;
        
        for(const auto& nextRelation : startSet)
        {
            // If nextRealation exists in the current permutation or 
            if( findInDeque(permutation, nextRelation) || !connectionExists(nextRelation, permutation) )
                continue;
            
            std::deque<std::size_t> currTree = permutation;
            currTree.push_back(nextRelation);

            std::string key = "";
            std::deque<std::size_t> copy = currTree;
            std::size_t treeSize = copy.size();
            std::sort(copy.begin(), copy.end());

            for(std::size_t i = 0; i < treeSize; i++)
            {
                if(i == treeSize - 1)
                    key += std::to_string(copy[i]);
                else
                    key += std::to_string(copy[i]) + ".";
            }
            

            if( bestTree.find(key) == bestTree.end() || cost(bestTree[key]) > cost(currTree) )
            {
                bestTree[key] = currTree;
            }
        }
    }

    std::string key = "";
    std::size_t i = 0;
    

    // Generate the key of the largest possible set
    for( const auto& element : startSet )
    {
        if(i == startSet.size() - 1)
            key += std::to_string(element);
        else
            key += std::to_string(element) + ".";
        i++;
    }

    std::deque<std::size_t> bestChoiceRels = bestTree[key];
    std::deque<std::size_t> prevRels;

    std::deque<RHJ::Query::Predicate> bestChoicePreds;

    
    for(std::size_t i = 0; i < bestChoiceRels.size() - 1; i++) {
        prevRels.emplace_back(bestChoiceRels[i]);
        std::vector<RHJ::Query::Predicate> subPreds = connected(bestChoiceRels[i+ 1], prevRels);
        
        for(const auto& subPred : subPreds)
            bestChoicePreds.emplace_back(subPred);
    }

    return bestChoicePreds;
}

bool RHJ::JoinEnumerator::connectionExists(std::size_t relA, const std::deque<std::size_t>& permutation)
{
    // Check if at least 1 connection exists between relA and permutation
    for( const auto& item : permutation )
    {
        auto it = connected(relA, item);
        if( it != connections.end() )
        {
            return true;
        }
    }

    return false;
}

std::vector<RHJ::Query::Predicate>
RHJ::JoinEnumerator::connected(std::size_t relA, const std::deque<std::size_t>& permutation)
{
    // Return all the connections between relA and permutation
    std::vector<RHJ::Query::Predicate> predList;

    for( const auto& item : permutation )
    {
        auto it = connected(relA, item);
        if( it != connections.end() )
        {
            for( const auto& pred : it->second)
                predList.emplace_back(pred);
        }
    }

    return predList;
}

std::unordered_map< std::string, std::deque<RHJ::Query::Predicate> >::iterator
RHJ::JoinEnumerator::connected(std::size_t relA, std::size_t relB)
{
    // Return all connections between relA and relB
    std::size_t pair[] = { relA, relB };
    std::sort(std::begin(pair), std::end(pair), [](std::size_t a, std::size_t b) { return (a < b); });

    std::string pairStr =  std::to_string(pair[0]) + "." + std::to_string(pair[1]);

    auto it = connections.find(pairStr);
    
    return ( (it != connections.end()) ? (it) : (connections.end()) );
}

std::vector<std::deque<std::size_t>> RHJ::JoinEnumerator::generateSubCombinations(const std::vector<std::size_t>& input)
{
    std::size_t n = input.size(), count = pow(2, n);

    std::vector<std::deque<std::size_t>> sets;

    for (std::size_t i = 1; i < count; i++)
    {
        std::deque<std::size_t> set;
        for (std::size_t j = 0; j < n; j++)
        {
            // i works as a bit mask
            if ((i & (1 << j)) != 0)
            {
                set.push_back(input[j]);
            }
        }
        sets.push_back(set);
    }

    std::vector<std::deque<std::size_t>> permutations;

    for (auto &set : sets)
    {

        std::sort(set.begin(), set.end());

        do
        {
            std::deque<std::size_t> permutation;

            for (auto &item : set)
                permutation.push_back(item);

            permutations.push_back(permutation);

        } while (std::next_permutation(set.begin(), set.end()));
    }

    std::sort(permutations.begin(), permutations.end(),
              [](std::deque<std::size_t> a, std::deque<std::size_t> b) {
                  if (a.size() < b.size())
                      return true;
                  else if (a.size() > b.size())
                      return false;
                  else
                      return (a[0] < b[0]);
              });

    return permutations;
}