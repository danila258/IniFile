#include "IniFile.h"

IniFile::element::element(const IniSection& section) : _name(section), _lineNum(section.getLineNum())
{}

IniFile::element::element(std::string  name, size_t lineNum) : _name( std::move(name) ), _lineNum(lineNum)
{}

bool IniFile::element::operator==(const element& other) const
{
    return _name == other._name;
}

std::size_t IniFile::elementHash::operator()(const element& elem) const noexcept
{
return std::hash<std::string>{}(elem._name);
}


IniFile::IniFile(std::string  path) : _path( std::move(path) )
{}

std::vector<IniSection> IniFile::operator[](const IniSection& name)
{
    return this->sectionRange(name);
}

void IniFile::load()
{
    std::ios_base::sync_with_stdio(false);

    std::fstream file;
    file.open(_path);

    if ( !file.is_open() )
    {
        throw std::runtime_error("can't open IniFile: " + _path);
    }

    size_t lineNum = 1;
    std::string line;

    auto sectionIt = _data.end();
    std::string section;
    std::string key;
    std::string value;

    while ( getline(file, line) )
    {
        size_t leftBracketPos = line.find_first_of('[');
        size_t rightBracketPos = line.find_first_of(']');

        if (leftBracketPos != std::string::npos && rightBracketPos != std::string::npos)
        {
            section = line.substr(leftBracketPos + 1, line.find_last_of(']') - line.find_first_of('[') - 1);
            sectionIt = _data.insert({{section, lineNum}, {}});
            ++lineNum;
            continue;
        }

        size_t equalPos = line.find_first_of('=');

        if (equalPos != std::string::npos)
        {
            key = readWord( line.substr(0, equalPos) );
            value = readWord( line.substr(equalPos + 1) );

            if (key.empty() || value.empty())
            {
                throw std::runtime_error("empty key or value in line: " + std::to_string(lineNum));
            }

            if (sectionIt == _data.end())
            {
                throw std::runtime_error("key and value without section in line: " + std::to_string(lineNum));
            }

            if (sectionIt->second.find(key) != sectionIt->second.end())
            {
                throw std::runtime_error("duplicate key in line: " + std::to_string(lineNum));
            }

            sectionIt->second.insert({{key, lineNum}, value});
        }

        ++lineNum;
    }
}

void IniFile::save() const
{
    std::ios_base::sync_with_stdio(false);

    std::fstream file;
    file.open(_path, std::ios::out);

    if ( !file.is_open() )
    {
        throw std::runtime_error("can't save IniFile");
    }

    std::vector<std::pair<element, std::vector<std::pair<element, std::string>>>> arr;
    arr.reserve( _data.size() );

    for (auto& pair : _data)
    {
        arr.push_back({pair.first, {}});
        arr.back().second.reserve( pair.second.size() );

        for (auto& keyVal : pair.second)
        {
            arr.back().second.emplace_back(keyVal.first, keyVal.second);
        }

        std::sort(arr.back().second.begin(), arr.back().second.end(),
                  [](const std::pair<element, std::string>& a, const std::pair<element, std::string>& b){
                      return a.first._lineNum < b.first._lineNum;
                  });
    }

    std::sort(arr.begin(), arr.end(), [](const std::pair<element, std::vector<std::pair<element, std::string>>>& a,
    const std::pair<element, std::vector<std::pair<element, std::string>>>& b){
        return a.first._lineNum < b.first._lineNum;
    });

    for (const auto& item : arr)
    {
        file << '[' << item.first._name << ']' << '\n';

        for (auto&& pair : item.second)
        {
            file << pair.first._name << " = " << pair.second << '\n';
        }

        file << '\n';
    }
}

template<>
char IniFile::read(const IniSection& section, const std::string& key, char defaultValue)
{
    auto pairIt = getIterator(section);

    if (pairIt == _data.end() || pairIt->second.find(key) == pairIt->second.end())
    {
        return defaultValue;
    }

    if (pairIt->second[key].size() > 1)
    {
        throw std::runtime_error( addLineNum(pairIt, key, "more than one character") );
    }

    return pairIt->second[key].front();
}

template<>
bool IniFile::read(const IniSection& section, const std::string& key, bool defaultValue)
{
    auto it = getIterator(section);

    if (it == _data.end() || it->second.find(key) == it->second.end())
    {
        return defaultValue;
    }

    std::string valueCopy = it->second[key];
    std::transform(valueCopy.begin(), valueCopy.end(), valueCopy.begin(), [](unsigned char c){
        return std::tolower(c);
    });

    if (std::find(alias::trueValue.begin(), alias::trueValue.end(), valueCopy) != alias::trueValue.end())
    {
        return true;
    }

    return false;
}

template<>
std::string IniFile::read(const IniSection& section, const std::string& key, std::string defaultValue)
{
    auto it = getIterator(section);

    if (it == _data.end() || it->second.find(key) == it->second.end())
    {
        return defaultValue;
    }

    return it->second[key];
}

IniSection IniFile::writeSection(const std::string& section)
{
    auto it = _data.insert({section, {}});
    return {section, getIteratorIndex(it)};
}

template<>
void IniFile::writeKeyValue(const IniSection& section, const std::string& key, bool value)
{
    auto it = getIterator(section);

    if (it == _data.end())
    {
        return;
    }

    if (value)
    {
        it->second[key] = alias::computerTruePrint;
    }
    else
    {
        it->second[key] = alias::computerFalsePrint;
    }
}

template<>
void IniFile::writeKeyValue(const IniSection& section, const std::string& key, const std::string& value)
{
    auto it = getIterator(section);

    if (it == _data.end())
    {
        return;
    }

    it->second[key] = value;
}

bool IniFile::sectionExists(const IniSection& section)
{
    auto it = getIterator(section);

    if (it == _data.end())
    {
        return false;
    }

    return true;
}

bool IniFile::keyExists(const IniSection& section, const std::string& key)
{
    auto it = getIterator(section);

    if (it == _data.end())
    {
        return false;
    }

    if (it->second.find(key) == it->second.end())
    {
        return false;
    }

    return true;
}

std::vector<IniSection> IniFile::sections()
{
    std::vector<IniSection> sections;

    for (auto it = _data.begin(); it != _data.end(); ++it)
    {
        sections.emplace_back(it->first._name, getIteratorIndex(it), it->first._lineNum);
    }

    return sections;
}

std::vector<IniSection> IniFile::sectionRange(const IniSection& section)
{
    std::vector<IniSection> sectionsArr;
    size_t index = 0;
    auto pairIt = _data.equal_range(section);

    while (pairIt.first != pairIt.second)
    {
        sectionsArr.emplace_back(section, index, pairIt.first->first._lineNum);
        ++index;
        ++pairIt.first;
    }

    return sectionsArr;
}

size_t IniFile::sectionCount(const IniSection& section) const
{
    auto pairIt = _data.equal_range(section);

    if (pairIt.first == _data.end() && pairIt.second == _data.end())
    {
        return 0;
    }

    return std::distance(pairIt.first, pairIt.second);
}

std::vector<std::string> IniFile::keys(const IniSection& section)
{
    std::vector<std::string> keys;

    auto it = getIterator(section);

    if (it == _data.end())
    {
        return keys;
    }

    keys.reserve( it->second.size() );

    for (auto& pair : it->second)
    {
        keys.emplace_back(pair.first._name);
    }

    return keys;
}

size_t IniFile::getKeyLineNum(const IniSection& section, const std::string& key)
{
    auto it = getIterator(section);

    if (it == _data.end())
    {
        return 0;
    }

    return it->second.find(key)->first._lineNum;
}

std::string IniFile::readWord(const std::string& line)
{
    if ( line.empty() )
    {
        return {};
    }

    size_t startPos = 0;
    size_t endPos = line.size();

    for (const auto& item : line)
    {
        if (item != ' ')
        {
            break;
        }

        ++startPos;
    }

    for (auto it = line.rbegin(); it != line.rend(); ++it)
    {
        if (*it != ' ')
        {
            break;
        }

        --endPos;
    }

    if ( line.find(parser::commentStart) != std::string::npos)
    {
        return line.substr(startPos, line.find(parser::commentStart) - 1);
    }

    return line.substr(startPos, endPos);
}

IniFile::dataIterator IniFile::getIterator(const IniSection& section)
{
    auto it = _data.equal_range(section);

    if (it.first == _data.end() && it.second == _data.end() || section.getIndex() >= std::distance(it.first, it.second) + 1)
    {
        return _data.end();
    }

    std::advance(it.first, section.getIndex());

    return it.first;
}

size_t IniFile::getIteratorIndex(dataIterator it)
{
    auto firstIt = _data.equal_range(it->first._name).first;
    return std::distance(firstIt, it);
}

std::string IniFile::addLineNum(dataIterator it, const std::string& key, const std::string& message)
{

    return message + " in line: " + std::to_string(it->second.find(key)->first._lineNum);
}