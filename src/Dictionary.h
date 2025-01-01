#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <unordered_map>
#include <string>
#include <unordered_set>

struct WordInfo {
    int frequency;
    std::string property;
};

class Dictionary {
public:
    void loadFromFile(const std::string& filePath);
    bool containsWord(const std::string& word) const;
    bool isPunctuation(const std::string& word) const; // 新增方法，判断是否是标点符号

private:
    static const std::unordered_set<std::string> punctuationSet;
    std::unordered_map<std::string, WordInfo> wordMap;
};


#endif // DICTIONARY_H