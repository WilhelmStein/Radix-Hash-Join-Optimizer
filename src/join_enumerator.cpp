#include <join_enumerator.hpp>

RHJ::JoinEnumerator::JoinEnumerator(const std::unordered_set<std::string>& queryColumns) {

    for(auto& column : queryColumns)
        bestTree[column].push_back(column);
}

RHJ::JoinEnumerator::~JoinEnumerator() {
    bestTree.clear();
}

std::deque<std::string> RHJ::JoinEnumerator::generateBestCombination() {
    for(std::size_t i = 1; i < bestTree.size(); i++) {
        
    }
}

std::deque<tuple_payload_t> RHJ::JoinEnumerator::generatePermutations(std::vector<std::string> strArr){
    
}  