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

std::size_t cost(std::deque<std::size_t> rel)
{
    return 100 - rel.size();
} 

RHJ::JoinEnumerator::JoinEnumerator(const RHJ::Query& query, Statistics *stats, tuple_payload_t **columns)
    : stats(stats), columns(columns)
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
            // pair[0] = query.predicates[i].left.rel;
            // pair[1] = query.predicates[i].left.col;
            // pair[2] = query.predicates[i].right.operand.rel;
            // pair[3] = query.predicates[i].right.operand.col;
        }
        else if ( query.predicates[i].left.rel >= query.predicates[i].right.operand.rel )
        {
            pair[0] = query.predicates[i].right.operand.rel;
            pair[1] = query.predicates[i].left.rel;
            // pair[0] = query.predicates[i].right.operand.rel;
            // pair[1] = query.predicates[i].right.operand.col;
            // pair[2] = query.predicates[i].left.rel;
            // pair[3] = query.predicates[i].left.col;
        }
        else
        {
            // if(query.predicates[i].left.col < query.predicates[i].right.operand.col)
            // {
            //     pair[0] = query.predicates[i].right.operand.rel;
            //     pair[1] = query.predicates[i].right.operand.col;
            //     pair[2] = query.predicates[i].left.rel;
            //     pair[3] = query.predicates[i].left.col;
            // }
            // else
            // {
            //     pair[0] = query.predicates[i].right.operand.rel;
            //     pair[1] = query.predicates[i].right.operand.col;
            //     pair[2] = query.predicates[i].left.rel;
            //     pair[3] = query.predicates[i].left.col;
            // }
        }

        connections.insert(std::to_string(pair[0]) + '.' + std::to_string(pair[1]));


        bestTree[std::to_string(pair[0])] = std::deque<std::size_t>(pair[0]);
        bestTree[std::to_string(pair[1])] = std::deque<std::size_t>(pair[1]);

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
    std::deque<std::size_t> bestOrder;

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
            std::size_t treeSize = currTree.size();

            for(std::size_t i = 0; i < treeSize; i++)
            {
                if(i == treeSize - 1)
                    key += currTree[i];
                else
                    key += currTree[i] + '.';
            }

            if( bestTree.find(key) == bestTree.end() || cost(bestTree[key]) > cost(currTree) )
            {
                bestTree[key] = currTree;
                if(cost(bestOrder) > cost(currTree))
                    bestOrder = currTree;
            }
        }
    }

    return bestOrder;
}

bool RHJ::JoinEnumerator::connected(std::size_t relA, const std::deque<std::size_t>& permutation)
{
    for( const auto& item : permutation )
    {
        std::size_t pair[] = { relA, item };
        std::sort(std::begin(pair), std::end(pair), [](std::size_t a, std::size_t b) { return (a < b); });

        std::string pairStr =  std::to_string(pair[0]) + '.' + std::to_string(pair[1]);

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