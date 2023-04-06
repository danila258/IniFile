#ifndef INIFILE_H
#define INIFILE_H

#include "IniSection.h"

#include <array>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <type_traits>
#include <cctype>


namespace parser
{
    constexpr auto numSeparator = '.';
    constexpr auto commentStart = ';';
    constexpr auto minus = '-';
}

namespace alias
{
    constexpr std::array<const char*, 4> trueValue = {"true", "on", "yes", "1"};
    constexpr auto computerTruePrint = "true";
    constexpr auto computerFalsePrint = "false";
}


class IniFile
{
private:
    struct element
    {
        element(const IniSection& section);
        element(std::string name, size_t lineNum = 0);

        std::string _name;
        size_t _lineNum;

        bool operator==(const element& other) const;
    };

    struct elementHash
    {
        std::size_t operator()(const element& elem) const noexcept;
    };

public:
    explicit IniFile(std::string path);

    std::vector<IniSection> operator[](const IniSection& name);

    void load();
	void save() const;

	template<typename T>
	T read(const IniSection& section, const std::string& key, T defaultValue = T{});

    IniSection writeSection(const std::string& section);

	template<typename T>
	void writeKeyValue(const IniSection& section, const std::string& key, T value);

    bool sectionExists(const IniSection& section);
    bool keyExists(const IniSection& section, const std::string& key);

    std::vector<IniSection> sections();
    std::vector<IniSection> sectionRange(const IniSection& section);
    size_t sectionCount(const IniSection& section) const;

    std::vector<std::string> keys(const IniSection& section);
    size_t getKeyLineNum(const IniSection& section, const std::string& key);

private:
    std::string _path;
    std::unordered_multimap<element, std::unordered_map<element, std::string, elementHash>, elementHash> _data;

    using dataIterator = std::unordered_multimap<element, std::unordered_map<element, std::string, elementHash>, elementHash>::iterator;

    static std::string readWord(const std::string& line);

    dataIterator getIterator(const IniSection& section);
    size_t getIteratorIndex(dataIterator it);

    std::string addLineNum(dataIterator it, const std::string& key, const std::string& message);
};


template<typename T>
T IniFile::read(const IniSection& section, const std::string& key, T defaultValue)
{
    auto pairIt = getIterator(section);

    if (pairIt == _data.end() || pairIt->second.find(key) == pairIt->second.end())
    {
        return defaultValue;
    }

    const std::string& line = pairIt->second.find(key)->second;

    if (std::is_integral<T>() || std::is_floating_point<T>())
    {
        size_t dotCount = 0;

        for (auto it = line.begin(); it != line.end(); ++it)
        {
            if (*it == parser::minus)
            {
                if (it != line.begin())
                {
                    throw std::runtime_error( addLineNum(pairIt, key, "wrong '-' position") );
                }
            }
            else if (*it == parser::numSeparator)
            {
                ++dotCount;
            }
            else if ( !std::isdigit(*it) )
            {
                throw std::runtime_error( addLineNum(pairIt, key, "not digit character in digit value") );
            }
        }

        if (dotCount > 1)
        {
            throw std::runtime_error(addLineNum(pairIt, key, "incorrect number of dots"));
        }
    }

    if ( std::is_unsigned<T>() )
    {
        if (line.find('-') != std::string::npos)
        {
            throw std::runtime_error( addLineNum(pairIt, key, "minus in unsigned value") );
        }
    }

    T value;

    std::istringstream stream(pairIt->second[key] );
    stream >> value;

    return value;
}

template<typename T>
void IniFile::writeKeyValue(const IniSection& section, const std::string& key, T value)
{
    auto it = getIterator(section);

    if (it == _data.end())
    {
        return;
    }

    it->second[key] << std::to_string(value);
}


#endif  //INIFILE_H