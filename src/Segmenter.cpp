#include "Segmenter.h"
#include <fstream>
#include <sstream>

Segmenter::Segmenter(const std::string& dictPath) {
    loadDictionary(dictPath);
}

void Segmenter::loadDictionary(const std::string& path) {
    dictionary.loadFromFile(path);
}

std::vector<std::string> Segmenter::segment(const std::string& sentence) {
    return findShortestPath(sentence);
}

std::vector<std::string> Segmenter::findShortestPath(const std::string& sentence) {
    Graph graph(sentence, dictionary);
    return graph.getShortestPath();
}