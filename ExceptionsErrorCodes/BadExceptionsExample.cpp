#include <stdexcept>
#include <unordered_map>

namespace BadExcceptionsExample {

    void DoubleEntry_Worser(std::unordered_map<int, int>& map, int key)
    {
        try
        {
            map[key] = map.at(key) * 2;
        }
        catch (const std::out_of_range&)
        {
            map[key] = 1;
        }
    }

    void DoubleEntry_Gooder(std::unordered_map<int, int>& map, int key)
    {
        auto existingValue_it = map.find(key);

        if (existingValue_it == map.end())
        {
            map[key] = 1;
            return;
        }

        map[key] = existingValue_it->second * 2;
    }

} // namespace BadExcceptionsExample
