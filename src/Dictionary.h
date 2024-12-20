#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <unordered_map>
#include <string>

struct WordInfo {
    int frequency;
    std::string property;
};

class Dictionary {
public:
    void loadFromFile(const std::string& filePath);
    bool containsWord(const std::string& word) const;

private:
    std::unordered_map<std::string, WordInfo> wordMap;
};

#endif // DICTIONARY_H