#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <string>
#include <queue>
#include <map>
#include "Dictionary.h"

class Graph {
public:
    Graph(const std::string& sentence, const Dictionary& dictionary);
    std::vector<std::string> getShortestPath();

private:
    struct Edge {
        size_t to;
        double cost;
    };

    void buildGraph();
    void dijkstra();

    std::string sentence;
    const Dictionary& dictionary;
    std::vector<std::vector<Edge>> adj;
    std::vector<double> dist;
    std::vector<size_t> prev;
};

#endif // GRAPH_H