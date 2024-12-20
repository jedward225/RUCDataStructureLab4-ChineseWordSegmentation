// Segmenter.h
#ifndef SEGMENTER_H
#define SEGMENTER_H

#include <vector>
#include <string>
#include "Dictionary.h"
#include "Graph.h"

class Segmenter {
public:
    Segmenter(const std::string& dictPath);
    std::vector<std::string> segment(const std::string& sentence); // 使用Dijkstra算法
    std::vector<std::string> segmentKthShortest(int k, const std::string& sentence); // 使用A*算法

private:
    void loadDictionary(const std::string& path);
    std::vector<std::string> findShortestPath(const std::string& sentence);
    std::vector<std::string> findKthShortestPath(int k, const std::string& sentence);

    Dictionary dictionary;
};

#endif // SEGMENTER_H