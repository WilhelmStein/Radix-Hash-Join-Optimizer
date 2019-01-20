#pragma once
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <vector>
#include <deque>
#include <string>
#include <types.hpp>
#include <executioner.hpp>
#include <statistics.hpp>
#include <pair.hpp>


namespace RHJ {
    class JoinEnumerator {

        std::unordered_map<std::string, std::deque<std::size_t> > bestTree;
        std::unordered_map< std::string, std::deque<RHJ::Query::Predicate> > connections;

        std::set<std::size_t> startSet;

        #ifdef __REC_COST__

        void recCost(std::deque<std::size_t>& rels, std::vector<float>& costs, std::deque<std::size_t>& prevRels, std::vector<RHJ::Query::Predicate>& predList, std::size_t maxDepth, std::size_t currDepth);

        #endif

        public:
        
        JoinEnumerator(const RHJ::Query& query);
        ~JoinEnumerator();

        std::deque<RHJ::Query::Predicate> generateBestCombination();

        float cost(std::deque<std::size_t> rel);
        bool connectionExists(std::size_t relA, const std::deque<std::size_t>& permutation);
        std::vector<RHJ::Query::Predicate> connected(std::size_t relA, const std::deque<std::size_t>& permutation);
        std::unordered_map< std::string, std::deque<RHJ::Query::Predicate> >::iterator connected(std::size_t relA, std::size_t relB);

        static std::vector< std::deque<std::size_t>> generateSubCombinations(const std::vector<std::size_t>& set);
        
    };
}