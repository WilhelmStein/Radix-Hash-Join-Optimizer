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
    //
    
    float cost = RHJ::Statistics::expected_cost(predArr, predArrSize);
    delete[] predArr;
    return cost;
} 

RHJ::JoinEnumerator::JoinEnumerator(const RHJ::Query& query)
{
    RHJ::Statistics::preprocess(query);

    for(std::size_t i = 0; i < query.preCount; i++)
    {
        if(query.predicates[i].type != RHJ::Query::Predicate::Type::join_t)
            continue;

        std::size_t pair[2];

        if( query.predicates[i].left.rel < query.predicates[i].right.operand.rel )
        {
            pair[0] = query.predicates[i].left.rel;
            pair[1] = query.predicates[i].right.operand.rel;
        }
        else
        {
            pair[0] = query.predicates[i].right.operand.rel;
            pair[1] = query.predicates[i].left.rel;
        }

        std::string relA = std::to_string(pair[0]), relB = std::to_string(pair[1]);
        connections[relA + "." + relB].emplace_back(query.predicates[i]);

        bestTree[relA] = std::deque<std::size_t>(pair[0]);
        bestTree[relB] = std::deque<std::size_t>(pair[1]);

        if(startSet.find(pair[0]) == startSet.end())
            startSet.insert(pair[0]);
        
        if(startSet.find(pair[1]) == startSet.end())
            startSet.insert(pair[1]);
    }
}

RHJ::JoinEnumerator::~JoinEnumerator()
{
    bestTree.clear();

    // for( auto& pair : connections )
    //     delete[] pair;
}

std::deque<RHJ::Query::Predicate> RHJ::JoinEnumerator::generateBestCombination()
{
    std::vector<std::size_t> input;
    for(const auto& item : startSet)
        input.push_back(item);

    std::vector<std::deque<std::size_t>> possiblePermutations = RHJ::JoinEnumerator::generateSubCombinations(input);

    for (const auto& permutation : possiblePermutations)
    {
        for(const auto& nextRelation : startSet)
        {
            if( findInDeque(permutation, nextRelation) || !connected(nextRelation, permutation).size() )
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

std::vector<RHJ::Query::Predicate>
RHJ::JoinEnumerator::connected(std::size_t relA, const std::deque<std::size_t>& permutation)
{
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