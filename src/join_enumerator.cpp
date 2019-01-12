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

    for( size_t i = 0; i < relArrSize - 1; i++ )
    {
        std::string key;

        if(rels[i] < rels[i + 1])
            key = (std::to_string(rels[i]) + "." + std::to_string(rels[i + 1]));
        else
            key = (std::to_string(rels[i + 1]) + "." + std::to_string(rels[i]));

        std::unordered_map< std::string, std::deque<RHJ::Query::Predicate> >::iterator it = connections.find(key);

        const std::deque<RHJ::Query::Predicate>* subPredList = nullptr;

        if( it != connections.end())
        {
            subPredList = &(it->second);

            for(const auto& pred : *subPredList)
                predList.emplace_back(pred);
        }
        
        
    }

    predArrSize = predList.size();

    RHJ::Query::Predicate* predArr = new RHJ::Query::Predicate[predArrSize];


    for( size_t i = 0; i < predArrSize; i++ )
        predArr[i] = predList[i];
    //
    
    float cost = 0.0f;//RHJ::Statistics::expected_cost(predArr, predArrSize);
    delete[] predArr;
    return cost;
} 

RHJ::JoinEnumerator::JoinEnumerator(const RHJ::Query& query)
{

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

std::deque<std::size_t> RHJ::JoinEnumerator::generateBestCombination()
{
    std::vector<std::size_t> input;
    for(const auto& item : startSet)
        input.push_back(item);

    std::vector<std::deque<std::size_t>> possiblePermutations = RHJ::JoinEnumerator::generateSubCombinations(input);

    for (const auto& permutation : possiblePermutations)
    {
        for(const auto& nextRelation : startSet)
        {
            if( findInDeque(permutation, nextRelation) || !connected(nextRelation, permutation) )
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

    return bestTree[key];
}

bool RHJ::JoinEnumerator::connected(std::size_t relA, const std::deque<std::size_t>& permutation)
{
    for( const auto& item : permutation )
    {
        std::size_t pair[] = { relA, item };
        std::sort(std::begin(pair), std::end(pair), [](std::size_t a, std::size_t b) { return (a < b); });

        std::string pairStr =  std::to_string(pair[0]) + "." + std::to_string(pair[1]);

        if( connections.find(pairStr) != connections.end() )
            return true;
    }

    return false;
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