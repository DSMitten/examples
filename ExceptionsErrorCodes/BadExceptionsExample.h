#include <unordered_map>

#pragma once

namespace BadExcceptionsExample {

    void DoubleEntry_Worser(std::unordered_map<int, int>& map, int key);
    void DoubleEntry_Gooder(std::unordered_map<int, int>& map, int key);

} // namespace BadExcceptionsExample
