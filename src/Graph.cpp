#include "Graph.h"
#include <limits>
#include <algorithm>
#include <sstream>
#include <functional>
#include <unordered_set>

struct State {
    double totalCost; // tot = actualCost + heuristicCost
    double actualCost;

    std::vector<size_t> path;

    bool operator<(const State& other) const {
        return totalCost > other.totalCost; // 最小堆性质
    }
};

Graph::Graph(const std::string& sentence, const Dictionary& dictionary)
    : sentence(sentence), dictionary(dictionary) {
    buildGraph();
}

void Graph::buildGraph() {
    adj.resize(sentence.length() + 1);
    size_t sentenceLength = sentence.length();
    
    for (size_t start = 0; start <= sentenceLength; ++start) {
        for (size_t end = start + 1; end <= sentenceLength; ++end) {
            std::string word = sentence.substr(start, end - start);

            if (dictionary.containsWord(word) || dictionary.isPunctuation(word)) {
                adj[start].push_back({end, 1.0}); // 权值设为 1.0
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

    std::priority_queue<std::pair<double, size_t>, 
                        std::vector<std::pair<double, size_t>>,
                        std::greater<std::pair<double, size_t>>
                       > pq;
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

std::vector<std::string> Graph::getKthShortestPath(int k) {
    return aStarKthShortest(k);
}

std::vector<std::string> Graph::aStarKthShortest(int k) {
    using State = struct State;

    auto heuristic = [](size_t current, size_t target) -> double {
        return static_cast<double>(target - current); // 简单的启发函数：剩余字符数
    };

    std::priority_queue<State> pq;
    std::unordered_set<std::string> foundPaths;
    int foundCount = 0;

    pq.push({heuristic(0, sentence.length()), 0, {0}});

    while (!pq.empty()) {
        State currentState = pq.top();
        pq.pop();

        if (currentState.path.back() == sentence.length()) {
            foundCount++;
            if (foundCount == k) {
                std::vector<std::string> segmentedPath;
                for (size_t i = 0; i < currentState.path.size() - 1; ++i) {
                    segmentedPath.push_back(sentence.substr(currentState.path[i], currentState.path[i + 1] - currentState.path[i]));
                }
                return segmentedPath;
            }
            continue;
        }

        for (const auto& edge : adj[currentState.path.back()]) {
            std::vector<size_t> newPath = currentState.path;
            newPath.push_back(edge.to);
            double newActualCost = currentState.actualCost + edge.cost;
            double newTotalCost = newActualCost + heuristic(edge.to, sentence.length());

            // 将路径转换为字符串以便于比较
            std::ostringstream oss;
            for (auto node : newPath) {
                oss << node << ",";
            }
            std::string pathStr = oss.str();

            if (foundPaths.find(pathStr) == foundPaths.end()) {
                foundPaths.insert(pathStr);
                pq.push({newTotalCost, newActualCost, newPath});
            }
        }
    }

    return {}; // 如果没有找到，返回空路径
}
