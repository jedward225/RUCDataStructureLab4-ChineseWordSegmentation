#include "Graph.h"
#include <limits>
#include <algorithm>
#include <sstream>

Graph::Graph(const std::string& sentence, const Dictionary& dictionary)
    : sentence(sentence), dictionary(dictionary) {
    buildGraph();
}

void Graph::buildGraph() {
    adj.resize(sentence.length() + 1);
    for (size_t start = 0; start <= sentence.length(); ++start) {
        for (size_t end = start + 1; end <= sentence.length(); ++end) {
            std::string word = sentence.substr(start, end - start);
            if (dictionary.containsWord(word)) {
                adj[start].push_back({end, 1.0}); // 权值定为1.0
            }
        }
    }
}

std::vector<std::string> Graph::getShortestPath() {
    dijkstra();
    std::vector<std::string> path;
    for (size_t pos = sentence.length(); pos > 0;) {
        size_t prevPos = prev[pos];
        path.push_back(sentence.substr(prevPos, pos - prevPos));
        pos = prevPos;
    }
    std::reverse(path.begin(), path.end());
    return path;
}

void Graph::dijkstra() {
    dist.assign(sentence.length() + 1, std::numeric_limits<double>::max());
    prev.assign(sentence.length() + 1, 0);
    dist[0] = 0;

    // 注意：这里添加了模板参数 std::greater<std::pair<double, size_t>>
    std::priority_queue<std::pair<double, size_t>, std::vector<std::pair<double, size_t>>, std::greater<std::pair<double, size_t>>> pq;
    pq.push({0, 0});

    while (!pq.empty()) {
        auto [cost, u] = pq.top();
        pq.pop();

        if (dist[u] < cost) continue;

        for (const auto& edge : adj[u]) {
            if (dist[edge.to] > cost + edge.cost) {
                dist[edge.to] = cost + edge.cost;
                prev[edge.to] = u;
                pq.push({dist[edge.to], edge.to});
            }
        }
    }
}