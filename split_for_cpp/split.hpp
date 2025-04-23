/**
 * @brief Splits a string into tokens based on a delimiter.
 *
 * @tparam Container The type of container to store the result.
 *                  Supported types: std::set<std::string>, std::vector<std::string>,
 *                  std::stack<std::string>, std::queue<std::string>, std::pair<std::string, std::string>.
 * @param str The input string to be split.
 * @param delimiter The delimiter used to split the string.
 * @return Container A container holding the split results.
 */

#ifndef HAZUKI_SPLIT_HPP
#define HAZUKI_SPLIT_HPP

#include <string>
#include <string_view>
#include <set>
#include <vector>
#include <stack>
#include <queue>
#include <utility>
#include <type_traits>

namespace hazuki
{
    template <typename Container>
    Container split(std::string_view str, std::string_view delimiter)
    {
        Container tokens;
        size_t start = 0, end = 0;

        static_assert(std::is_same_v<Container, std::set<std::string>> ||
                          std::is_same_v<Container, std::vector<std::string>> ||
                          std::is_same_v<Container, std::stack<std::string>> ||
                          std::is_same_v<Container, std::queue<std::string>> ||
                          std::is_same_v<Container, std::pair<std::string, std::string>>,
                      "Unsupported container type");

        if (delimiter.empty())
        {
            throw "Delimiter cannot be empty";
        }

        while ((end = str.find(delimiter, start)) != std::string_view::npos)
        {
            if constexpr (std::is_same_v<Container, std::set<std::string>> ||
                          std::is_same_v<Container, std::vector<std::string>>)
            {
                tokens.insert(tokens.end(), std::string(str.substr(start, end - start)));
            }
            else if constexpr (std::is_same_v<Container, std::stack<std::string>>)
            {
                tokens.push(std::string(str.substr(start, end - start)));
            }
            else if constexpr (std::is_same_v<Container, std::queue<std::string>>)
            {
                tokens.push(std::string(str.substr(start, end - start)));
            }
            else if constexpr (std::is_same_v<Container, std::pair<std::string, std::string>>)
            {
                size_t delimiter_pos = str.find(delimiter, start);
                if (delimiter_pos != std::string_view::npos)
                {
                    tokens.first = std::string(str.substr(start, delimiter_pos - start));
                    tokens.second = std::string(str.substr(delimiter_pos + delimiter.length()));
                }
                else
                {
                    tokens.first = std::string(str.substr(start));
                    tokens.second = "";
                }
                return tokens;
            }
            start = end + delimiter.length();
        }

        if (!str.substr(start).empty())
        {
            if constexpr (std::is_same_v<Container, std::set<std::string>> ||
                          std::is_same_v<Container, std::vector<std::string>>)
            {
                tokens.insert(tokens.end(), std::string(str.substr(start)));
            }
            else if constexpr (std::is_same_v<Container, std::stack<std::string>>)
            {
                tokens.push(std::string(str.substr(start)));
            }
            else if constexpr (std::is_same_v<Container, std::queue<std::string>>)
            {
                tokens.push(std::string(str.substr(start)));
            }
            else if constexpr (std::is_same_v<Container, std::pair<std::string, std::string>>)
            {
                tokens.first = std::string(str.substr(start));
                tokens.second = "";
            }
        }

        return tokens;
    }
}

#endif