#include "IniSection.h"

IniSection::IniSection(std::string  section, size_t index, size_t lineNum)
        : _name(std::move(section)), _index(index), _lineNum(lineNum)
{}

IniSection::IniSection(const char* section, size_t index, size_t lineNum)
        : _name(section), _index(index), _lineNum(lineNum)
{}

IniSection::operator std::string() const
{
    return _name;
}

std::string operator+(const std::string& str, const IniSection& section)
{
    return str + section._name;
}

size_t IniSection::getIndex() const
{
    return _index;
}

size_t IniSection::getLineNum() const
{
    return _lineNum;
}