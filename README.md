# IniFile

## Example

```cpp
#include "IniFile/IniFile.h"

// ...

IniFile file("path");
file.load();

std::vector<IniSection> allSections = file.sections();
std::vector<IniSection> range = file.sectionRange("section");
size_t count = file.sectionCount("section");

// When passing a non-IniSection, the first section in the file with the same name is searched
std::vector<std::string> keys = file.keys("section");
size_t num = file.getKeyLineNum("section", "key");

IniSection newSection = file.writeSection("new section");
file.writeKeyValue<int>(newSection, "key", 5);

std::string value = file.read<std::string>(newSection, "key", "default value");

bool sectionFlag = file.sectionExists("section");
bool keyFlag = file.keyExists("section", "key");

file.save();
```

## CMake

```cmake
# CMakeLists.txt
add_subdirectory(IniFile)
...
target_link_libraries(${PROJECT_NAME} PRIVATE IniFile)
```
