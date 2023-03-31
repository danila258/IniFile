#ifndef IniFile_H
#define IniFile_H

#include "IniSection.h"

#include <array>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>


namespace alias
{
    constexpr std::array<const char*, 4> trueValue = {"true", "on", "yes", "1"};
    constexpr auto computerTruePrint = "true";
    constexpr auto computerFalsePrint = "false";
}


class IniFile
{
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
    struct element
    {
        element(const IniSection& section);
        element(std::string  name, size_t lineNum = 0);

        std::string _name;
        size_t _lineNum;

        bool operator==(const element& other) const;
    };

    struct elementHash
    {
        std::size_t operator()(const element& elem) const noexcept;
    };

    std::string _path;
    std::unordered_multimap<element, std::unordered_map<element, std::string, elementHash>, elementHash> _data;

    static std::string readWord(const std::string& line);

    std::unordered_multimap<element, std::unordered_map<element, std::string, elementHash>, elementHash>::iterator
    getIterator(const IniSection& section);

    size_t getIteratorIndex(std::unordered_multimap<element, std::unordered_map<element, std::string, elementHash>, elementHash>::iterator it);
};


template<typename T>
T IniFile::read(const IniSection& section, const std::string& key, T defaultValue)
{
    auto it = getIterator(section);

    if (it == _data.end() || it->second.find(key) == it->second.end())
    {
        return defaultValue;
    }

    T var;

    std::istringstream stream( it->second[key] );
    stream >> var;

    return var;
}

template<typename T>
void IniFile::writeKeyValue(const IniSection& section, const std::string& key, T value)
{
    auto it = getIterator(section);

    if (it == _data.end())
    {
        return;
    }

    it->second[key] = std::to_string(value);
}


#endif  //IniFile_H