#include "Dictionary.h"
#include <fstream>
#include <sstream>
#include <unordered_set>

void Dictionary::loadFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("无法打开字典文件");
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string word, property;
        int frequency;
        if (iss >> word >> frequency >> property) {
            wordMap[word] = {frequency, property};
        }
    }
}

bool Dictionary::containsWord(const std::string& word) const {
    return wordMap.find(word) != wordMap.end();
}

const std::unordered_set<std::string> Dictionary::punctuationSet = {
    "，", "。", "！", "？", "：", "；", "（", "）", "【", "】", "《", "》",
    "、", "——", "「", "」", "·", "～", "“", "”", "＂", "′", "℃", "‰"
};

bool Dictionary::isPunctuation(const std::string& word) const {
    return punctuationSet.find(word) != punctuationSet.end();
}