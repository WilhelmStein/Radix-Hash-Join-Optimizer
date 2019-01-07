#pragma once
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <vector>
#include <deque>
#include <string>
#include <types.hpp>
#include <executioner.hpp>


namespace RHJ {
    class JoinEnumerator {

        std::unordered_map< std::string, std::deque<std::string> > bestTree;

        public:
        
        JoinEnumerator(const std::unordered_set<std::string>& queryColumns);
        ~JoinEnumerator();

        std::deque<std::string> generateBestCombination();

        static std::deque<tuple_payload_t> generatePermutations(std::vector<std::string> strArr);
        
    };
}