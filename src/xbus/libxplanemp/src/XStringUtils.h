#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <string>
#include <vector>
#include <algorithm>

std::vector<std::string> tokenize(const std::string &str, const std::string &delim)
{
    std::string dup = str;
    std::vector<std::string> result;
    if (delim.empty())
    {
        result.push_back(dup);
        return result;
    }

    if (dup.empty()) return result;

    while (true)
    {
        auto position = dup.find_first_of(delim);
        std::string token = dup.substr(0, position);

        if (!token.empty())
        {
            result.push_back(token);
        }

        // Nothing remaining
        if (position == std::string::npos) return result;

        dup = dup.substr(position + 1);
    }
}


#endif // STRING_UTILS_H

