#pragma once

#include <functional>
#include <string>

// Code forked from Boat
// https://github.com/gyroknight/boat-a1p1

namespace StrConverter
{
/**
 * @brief Checks if a string can be fully converted to the type specified using
 * the provided converter.
 *
 * @tparam T The type to convert to
 * @param s The string to convert
 * @param value A buffer to store the attempted conversion
 * @param stringConverter A function that takes in a string and an end index
 *                        pointer and converts the string to the given type
 * @return true if the string was fully converted
 * @return false if the string could not be converted or end index is not the
 *         end of string
 */
template <typename T>
bool canConvert(
    const std::string &s, T &value,
    std::function<T(const std::string &, size_t *)> stringConverter);
/**
 * @brief Checks if a string is an unsigned integer.
 *
 * Uses std::stoul and bounds checking underneath since there is no standard
 * function to convert explicitly to unsigned int
 *
 * @param s The string to convert
 * @param value Buffer to store converted value
 * @return true if the string is an unsigned integer
 * @return false if it is not
 */
bool isUInt(const std::string &s, unsigned long &value);
} // namespace StrConverter
