#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <unordered_map>
#include <string>

struct WordInfo {
    int frequency;
    std::string property;
};
// Dictionary.h
class Dictionary {
public:
    void loadFromFile(const std::string& filePath);
    bool containsWord(const std::string& word) const;
    bool isPunctuation(const std::string& word) const; // 新增方法，判断是否是标点符号

private:
    // 中文标点符号集合
    static const std::unordered_set<std::string> punctuationSet;
};


#endif // DICTIONARY_H