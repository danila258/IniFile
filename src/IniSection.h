#ifndef INISECTION_H
#define INISECTION_H

#include <string>
#include <utility>


struct IniSection
{
public:
    IniSection(std::string section, size_t index = 0, size_t lineNum = 0);
    IniSection(const char* section, size_t index = 0, size_t lineNum = 0);
    operator std::string() const;
    friend std::string operator+(const std::string& str, const IniSection& section);

    size_t getIndex() const;
    size_t getLineNum() const;

private:
    std::string _name;
    size_t _index;
    size_t _lineNum;
};


#endif //INISECTION_H
