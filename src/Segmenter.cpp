#include "Segmenter.h"
#include <fstream>
#include <sstream>
#include <limits>

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


std::vector<std::string> Segmenter::segmentKthShortest(int k, const std::string& sentence) {
    return findKthShortestPath(k, sentence);
}

std::vector<std::string> Segmenter::findKthShortestPath(int k, const std::string& sentence) {
    Graph graph(sentence, dictionary);
    return graph.getKthShortestPath(k);
}
