#include "Segmenter.h"
#include <fstream>
#include <sstream>
#include <limits>
#include <iostream>
#include <string>

Segmenter::Segmenter(const std::string& dictPath) {
    loadDictionary(dictPath);
}

void Segmenter::loadDictionary(const std::string& path) {
    dictionary.loadFromFile(path);
}

std::vector<std::string> Segmenter::segment(const std::string& sentence) {
    std::string processedSentence;
    
    auto isChinesePunctuation = [](wchar_t wc) -> bool {
        return (wc >= 0x3000 && wc <= 0x303F);
    };

    std::wstring wideSentence(sentence.begin(), sentence.end());

    for (size_t i = 0; i < wideSentence.size(); ++i) {
        wchar_t currentChar = wideSentence[i];

        if (isChinesePunctuation(currentChar)) {
            if (!processedSentence.empty() && processedSentence.back() != ' ') {
                processedSentence += ' ';
            }
            processedSentence += static_cast<char>(currentChar);
            processedSentence += ' ';
        } else {
            processedSentence += static_cast<char>(currentChar);
        }
    }
    //std::cerr << "Processed sentence: " << processedSentence << std::endl;

    return findShortestPath(processedSentence);
}


std::vector<std::string> Segmenter::findShortestPath(const std::string& sentence) {
    Graph graph(sentence, dictionary);
    return graph.getShortestPath();
}


std::vector<std::string> Segmenter::segmentKthShortest(int k, const std::string& sentence) {
    return findKthShortestPath(k, sentence);
}

std::vector<std::string> Segmenter::findKthShortestPath(int k, const std::string& sentence) {
    Graph graph(sentence, dictionary);
    return graph.getKthShortestPath(k);
}
