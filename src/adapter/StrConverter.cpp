#include <functional>
#include <string>

#include "StrConverter.h"

// Code forked from Boat
// https://github.com/gyroknight/boat-a1p1

template <typename T>
bool StrConverter::canConvert(
    const std::string &s, T &value,
    std::function<T(const std::string &, size_t *)> stringConverter)
{
    size_t endIdx;
    try
    {
        value = stringConverter(s, &endIdx);
    }
    catch (const std::invalid_argument &e)
    {
        return false;
    }
    return endIdx == s.size();
}

bool StrConverter::isUInt(const std::string &s, unsigned long &value)
{
    auto ulConverter = [](auto str, auto idx) { return std::stoul(str, idx); };
    return StrConverter::canConvert<unsigned long>(s, value, ulConverter) &&
           value == static_cast<unsigned int>(value);
}