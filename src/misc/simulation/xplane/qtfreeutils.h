// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_XPLANE_XPLANQTFREEUTILS_H
#define SWIFT_MISC_SIMULATION_XPLANE_XPLANQTFREEUTILS_H

#include <algorithm>
#include <cctype>
#include <cmath>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

// Strict header only X-Plane model parser utils shared between Misc and xswiftbus.
// Header only is necessary to no require xswiftbus to link against Misc.

namespace swift::misc::simulation::xplane::qtfreeutils
{
    //! Get filename (including all extensions) from a filePath
    inline std::string getFileName(const std::string &filePath)
    {
        const std::string seperator = "/\\";
        const std::size_t sepPos = filePath.find_last_of(seperator);
        if (sepPos != std::string::npos)
        {
            return filePath.substr(sepPos + 1, filePath.size() - 1);
        }
        else
        {
            return filePath;
        }
    }

    //! Get the name of the parent directory
    inline std::string getDirName(const std::string &filePath)
    {
        const std::string seperator = "/\\";
        const std::size_t sepPos = filePath.find_last_of(seperator);
        if (sepPos != std::string::npos)
        {
            std::string dirPath = filePath.substr(0, sepPos);
            return getFileName(dirPath);
        }
        else
        {
            return {};
        }
    }

    //! Get the base name of the file
    inline std::string getBaseName(const std::string &filePath)
    {
        const std::string seperator = ".";
        const std::string fileName = getFileName(filePath);
        std::size_t sepPos = fileName.find(seperator);
        if (sepPos != std::string::npos)
        {
            return fileName.substr(0, sepPos);
        }
        else
        {
            return fileName;
        }
    }

    //! Split string by delimiter and maxSplitCount times
    inline std::vector<std::string> split(const std::string &str, size_t maxSplitCount = 0, const std::string &delimiter = " ")
    {
        std::string s(str);
        size_t pos = 0;
        std::vector<std::string> tokens;
        while ((pos = s.find(delimiter)) != std::string::npos)
        {
            tokens.push_back(s.substr(0, pos));
            s.erase(0, pos + delimiter.length());
            if (maxSplitCount > 0 && tokens.size() == maxSplitCount) { break; }
        }
        tokens.push_back(s);
        return tokens;
    }

    //! Normalize value to range start -> end (like for +-180degrees)
    inline double normalizeValue(const double value, const double start, const double end)
    {
        // https://stackoverflow.com/questions/1628386/normalise-orientation-between-0-and-360
        if (value >= start && value <= end) { return value; }
        const double width = end - start;
        const double offsetValue = value - start; // value relative to 0
        return (offsetValue - (floor(offsetValue / width) * width)) + start;
    }

    //! ACF properties
    struct AcfProperties
    {
        std::string aircraftIcaoCode; //!< Aircraft ICAO code
        std::string modelDescription; //!< Model description
        std::string modelName; //!< Model name
        std::string author; //!< Model author
        std::string modelString; //!< Generated model string
    };

    //! Get the model string for a flyable aircraft
    inline std::string stringForFlyableModel(const AcfProperties &acfProperties, const std::string &acfFile)
    {
        if (!acfProperties.author.empty())
        {
            if (!acfProperties.modelName.empty())
            {
                if (acfProperties.modelName.find(acfProperties.author) != std::string::npos) { return acfProperties.modelName; }
                else { return acfProperties.author + ' ' + acfProperties.modelName; }
            }
            else if (!acfProperties.aircraftIcaoCode.empty())
            {
                return acfProperties.author + ' ' + acfProperties.aircraftIcaoCode;
            }
        }
        return getDirName(acfFile) + ' ' + getBaseName(acfFile);
    }

    //! String to lower case
    inline std::string toLower(std::string s)
    {
        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
            return std::tolower(c);
        });
        return s;
    }

    //! Compare case insensitive
    inline bool stringCompareCaseInsensitive(const std::string &str1, const std::string &str2)
    {
        if (str1.size() != str2.size()) { return false; }
        return std::equal(str1.begin(), str1.end(), str2.begin(), [](const char &c1, const char &c2) {
            return (c1 == c2 || std::toupper(c1) == std::toupper(c2));
        });
    }

    //! Yes/no from bool
    inline const std::string &boolToYesNo(bool t)
    {
        static const std::string y("yes");
        static const std::string n("no");
        return t ? y : n;
    }

    //! True/false from bool
    inline const std::string &boolTotrueFalse(bool t)
    {
        static const std::string tr("true");
        static const std::string fa("false");
        return t ? tr : fa;
    }

    //! Qt free version of fuzzy compare
    inline bool isFuzzyEqual(double v1, double v2)
    {
        // we allow some epsilon here
        // static const double Epsilon = 5 * std::numeric_limits<double>::min();
        static const double Epsilon = 1E-08;
        return (fabs(v1 - v2) < Epsilon);
    }

    //! Trim whitespace from the beginning and end, and replace sequences of whitespace with single space characters
    inline std::string simplifyWhitespace(const std::string &s)
    {
        std::string result;
        for (char c : s)
        {
            if (std::isspace(c))
            {
                if (!result.empty() && result.back() != ' ') { result.push_back(' '); }
            }
            else { result.push_back(c); }
        }
        while (!result.empty() && result.back() == ' ') { result.pop_back(); }
        return result;
    }

    //! Extract ACF properties from an aircraft file
    inline AcfProperties extractAcfProperties(const std::string &filePath)
    {
        std::ifstream fs(filePath, std::ios::in);
        if (!fs.is_open()) { return {}; }

        std::string i;
        std::string version;
        std::string acf;
        std::getline(fs, i);
        i = toLower(i);
        std::getline(fs, version);
        version = toLower(version);
        std::getline(fs, acf);
        acf = toLower(acf);

        AcfProperties acfProperties;

        if (i == "i" && version.find("version") != std::string::npos && acf == "acf")
        {
            std::string line;
            while (std::getline(fs, line))
            {
                auto tokens = split(line, 2);
                if (tokens.size() < 3 || tokens.at(0) != "P") { continue; }

                if (tokens.at(1) == "acf/_ICAO")
                {
                    acfProperties.aircraftIcaoCode = tokens.at(2);
                }
                else if (tokens.at(1) == "acf/_descrip")
                {
                    acfProperties.modelDescription = "[ACF] " + tokens.at(2);
                }
                else if (tokens.at(1) == "acf/_name")
                {
                    acfProperties.modelName = tokens.at(2);
                }
                else if (tokens.at(1) == "acf/_studio")
                {
                    acfProperties.author = tokens.at(2);
                }
                else if (tokens.at(1) == "acf/_author")
                {
                    if (!acfProperties.author.empty()) { continue; }
                    std::string author = tokens.at(2);
                    size_t pos = author.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_");
                    author = author.substr(0, pos);
                    if (author.empty()) { continue; }
                    acfProperties.author = author;
                }
            }
        }

        fs.close();
        acfProperties.modelString = simplifyWhitespace(stringForFlyableModel(acfProperties, filePath));
        return acfProperties;
    }

    //! Encoding-aware iterator adaptor for std::u8string
    template <typename I>
    struct Utf8Iterator
    {
        //! @{
        //! STL compatibility
        using value_type = typename std::iterator_traits<I>::value_type;
        using difference_type = typename std::iterator_traits<I>::difference_type;
        using reference = typename std::iterator_traits<I>::reference;
        using pointer = typename std::iterator_traits<I>::pointer;
        using iterator_category = std::forward_iterator_tag;
        //! @}

        //! Default constructor
        Utf8Iterator() = default;

        //! Constructor
        Utf8Iterator(I base, I end) : base(base), end(end) {}

        //! @{
        //! Equality
        friend bool operator==(Utf8Iterator a, Utf8Iterator b) { return a.base == b.base; }
        friend bool operator!=(Utf8Iterator a, Utf8Iterator b) { return a.base != b.base; }
        friend bool operator==(Utf8Iterator a, I b) { return a.base == b; }
        friend bool operator!=(Utf8Iterator a, I b) { return a.base != b; }
        friend bool operator==(I a, Utf8Iterator b) { return a == b.base; }
        friend bool operator!=(I a, Utf8Iterator b) { return a != b.base; }
        //! @}

        //! Dereference (not encoding-aware)
        reference operator*() const { return *base; }

        //! Pointer indirection (not encoding-aware)
        pointer operator->() const { return base.operator->(); }

        //! Pre-increment
        Utf8Iterator &operator++()
        {
            constexpr auto isContinuation = [](auto c) {
                return (c & static_cast<value_type>(0b11000000)) == static_cast<value_type>(0b10000000);
            };
            do {
                ++base;
            }
            while (base != end && isContinuation(*base));
            return *this;
        }

        //! Post-increment
        Utf8Iterator operator++(int)
        {
            auto copy = *this;
            ++*this;
            return copy;
        }

        I base; //!< Underlying iterator
        I end; //!< Underlying end iterator
    };
} // namespace swift::misc::simulation::xplane::qtfreeutils

#endif // guard
