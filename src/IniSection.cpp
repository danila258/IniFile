#include "IniSection.h"

IniSection::IniSection(std::string section, size_t index, size_t lineNum)
        : _name(std::move(section)), _index(index), _lineNum(lineNum)
{}

IniSection::IniSection(const char* section, size_t index, size_t lineNum)
        : _name(section), _index(index), _lineNum(lineNum)
{}

IniSection::IniSection(const IniSection& other) : IniSection(other._name, other._index, other._lineNum)
{}

IniSection& IniSection::operator=(const IniSection& other)
{
    if (this == &other)
    {
        return *this;
    }

    IniSection copy(other);
    *this = std::move(copy);

    return *this;
}

IniSection::IniSection(IniSection&& other)
{
    *this = std::move(other);
}

IniSection& IniSection::operator=(IniSection&& other)
{
    std::swap(this->_name, other._name);
    std::swap(this->_index, other._index);
    std::swap(this->_lineNum, other._lineNum);

    return *this;
}

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