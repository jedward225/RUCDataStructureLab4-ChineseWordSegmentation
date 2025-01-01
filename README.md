# Chinese Word Segmentation

## 项目概述

本项目旨在实现一个中文分词器，能够利用Dijkstra算法找到最短路径，并新增A*算法来获取第k短路径，以提供用户选择不同的分词方式。通过这种方式，用户可以根据需要选择最适合的分词策略，为文本处理任务提供灵活性。

## 安装指南

### 环境需求

- C++ 编译器 (支持C++11或更高版本)
- CMake (3.0 或更高版本)

### 构建步骤

1. **克隆仓库**

   ```bash
   git clone https://github.com/YourUsername/RUCDataStructureLab4-ChineseWordSegmentation.git
   cd RUCDataStructureLab4-ChineseWordSegmentation
   ```
   
2. **创建构建目录并编译**

   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```
   
3. **运行程序**

    

   如果您希望使用默认字典文件`dict.txt`：

   ```bash
   ./ChineseWordSegmentation
   ```
   
   如果您想要指定自己的字典文件：

   ```bash
   ./ChineseWordSegmentation path/to/dict.txt
   ```

## 使用说明

启动程序后，您将看到一个简单的命令行界面，允许您选择不同的分词方式：

- **选项1**: 使用Dijkstra算法进行最短路径分词。
- **选项2**: 使用A*算法进行第K短路径分词，需输入k值。
- **选项0**: 退出程序。

对于每个选项，您可以输入要分词的文本（输入`exit`可随时退出），然后程序会输出分词结果。

## 代码结构

### 主要文件

- **`main.cpp`**: 包含程序入口点，处理用户交互逻辑。
- **`Segmenter.h/cpp`**: 定义了分词器类及其接口。
- **`Graph.h/cpp`**: 实现图结构及相关的最短路径和第K短路径算法。
- **`Dictionary.h/cpp`**: 管理词汇表，加载并检查单词是否存在于字典中。

### 类定义

#### `Segmenter` 类

- 功能:

  - 加载字典文件。

  - 提供两种分词方法：最短路径分词和第K短路径分词。
  
- 实现细节:

  ```c++
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
  
  // Segmenter.cpp
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
  ```

#### `Graph` 类

- 功能:

  - 构建句子对应的图结构。
  - 实现Dijkstra算法找到最短路径。
  - 实现A*算法找到第K短路径。

- 实现细节:

  ```c++
  // graph.h
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
      std::vector<std::string> getKthShortestPath(int k);
  
  private:
      struct Edge {
          size_t to;
          double cost;
      };
  
      void buildGraph();
      void dijkstra();
  
      std::vector<std::string> aStarKthShortest(int k);
  
      std::string sentence;
      const Dictionary& dictionary;
      std::vector<std::vector<Edge>> adj;
      std::vector<double> dist;
      std::vector<size_t> prev;
  };
  
  #endif
  
  
  // graph.cpp
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
  ```

#### `Dictionary` 类

- 功能:

  - 加载字典文件到内存。

  - 提供接口检查给定字符串是否是合法单词
  
- 实现细节:

  ```c++
  // dictionary.h
  #ifndef DICTIONARY_H
  #define DICTIONARY_H
  
  #include <unordered_map>
  #include <string>
  
  struct WordInfo {
      int frequency;
      std::string property;
  };
  
  class Dictionary {
  public:
      void loadFromFile(const std::string& filePath);
      bool containsWord(const std::string& word) const;
  
  private:
      std::unordered_map<std::string, WordInfo> wordMap;
  };
  
  #endif // DICTIONARY_H
  
  // dictionary.cpp
  #include "Dictionary.h"
  #include <fstream>
  #include <sstream>
  
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
  ```

## 实现细节

### Dijkstra算法

用于寻找从起点到终点的最短路径。该算法保证在无负权重的情况下找到最优解，并且适用于加权图。

### A*算法

结合了启发式搜索的特点，可以有效地找到第K短路径。相比于简单的深度优先搜索，A*算法通过估计剩余成本来指导搜索过程，从而提高效率。

具体而言，我采用的 `Heuristic` 函数如下所示：

```c++
    auto heuristic = [](size_t current, size_t target) -> double {
        return static_cast<double>(target - current); // 简单的启发函数：剩余字符数
    };
```

其实还可以做到一个优化，以下稍作提及和复杂度分析：

$\mathcal{A}^{\star}$**算法**定义了一个对当前状态 $x$ 的估价函数 $f(x)=g(x)+h(x)$ 。每次取出 $f(x)$ 最优的状态 $x$ 扩展其所有子状态，可以用优先队列 （堆）来维护这个值。

在求解 $k$ 短路问题时，其实可以令 $h(x)$ 为从当前结点到达终点 $T$ 的最短路径长度，这可以通过在反向图上对结点 $T$ 跑单源最短路预处理出对每个结点的这个值。

由于设计的距离函数和估价函数，对于每个状态需要记录两个值，为当前到达的结点$x$和已经走过的距离 $g(x)$，将这种状态记为 $(x,g(x))$。

**优化**：由于只需要求出从初始结点到目标结点的第 $k$ 短路，所以已经取出的状态到达一个结点的次数大于 $k$ 次时，可以不扩展其子状态。因为之前 $k$ 次已经形成了 $k$ 条合法路径，当前状态不会影响到最后的答案。注意到，$k$ 短路问题下，如果图的形态是一个 $n$ 元环，该算法时间最坏是 $O(nk\log n)$ 的。但是落脚到本题，一个“树状”结构，这种算法可以在略好的复杂度内求出从起始点 $s$ 到每个结点的前 $k$ 短路。

> 除此以外，笔者因为具有ICPC算法竞赛经历，对于此问题，还有一种基于可持久化可并堆的方式来解决，这里少做提点（代码中并未涉及）：
>
> **可持久化可并堆优化 $k$ 短路算法**
>
> 1. 最短路树与任意路径
>
>    在反向图上从 $T$ 开始跑最短路，设在原图上结点 $x$ 到 $T$ 的最短路长度为 $dist_x$，建出任意一棵以 $T$
>    为根的最短路树 $Tr$
>
> 2. 性质
>
>    设一条从 $S$ 到 $T$ 的路径经过的边集为 $P$ ，去掉 $P$ 中与 $Tr$ 的交集得到 $P^\prime$。
>
>    $P^{\prime}$有如下性质：
>
>    - 对于一条不在 $Tr$ 上的边 $e$，其为从 $u$ 到 $v$ 的一条边，边权为 $w$，定义其代价 $\Delta e=dist_v+w-dist_u$，即为选择该边后路径长度的增加量。则路径 $P$ 的长度 $L_P=dist_s+\sum_{e\in P^{\prime}}\Delta e$。
>
>    - 将 $P$ 和 $P^\prime$ 中的所有边按照从 $S$ 到 $T$ 所经过的顺序依次排列，则对于 $P^\prime$ 中相邻的两条边 $e_1,e_2$，有 $u_{e_2}$ 与 $v_{e_1}$ 相等或为其在 $Tr$ 上的祖先。因为在 $P$ 中 $e_1,e_2$ 直接相连或中间都为树边。
>
>    - 对于一个确定存在的 $P^\prime$，有且仅有一个 $S$ ，使得 $S^\prime=P^{\prime}$。因为由于性质 2，$P^\prime$ 中相邻的两条边的起点和终点之间在 $Tr$ 上只有一条路径。
>
> 3. 问题转化
>    - 性质 1 告诉我们知道集合 $P^{\prime}$ 后，如何求出 $L_P$ 的值。
>    - 性质 2 告诉我们所有 $P^{\prime}$ 一定满足的条件，所有满足这个条件的边集 $P^{\prime}$ 都是合法的，也就告诉我们生成 $P^{\prime}$ 的方法。
>    - 性质 3 告诉我们对于每个合法的 $P^{\prime}$ 有且仅有一个边集 $P$ 与之对应
>    - 那么问题转化为：求 $L_P$ 的值第 $k$ 小的满足性质 2 的集合 $P^\prime$。
>
> 4. 过程
>
>    由于性质 2,我们可以记录按照从 $S$ 到 $T$ 的顺序排列的最后一条边和 $L_P$ 的值，来表示一个边集 $P^\prime$ 我们用一个小根堆来维护这样的边集 $P^{\prime}$。
>
>    初始我们将起点为 $1$ 或 $1$ 在 $Tr$ 上的祖先的所有的边中 $\Delta e$ 最小的一条边加入小根堆，每次取出堆顶的一个边集 $S$，有两种方法可以生成可能的新边集：
>
>    1. 替换 $S$ 中的最后一条边为满足相同条件的 $\Delta e$ 更大的边。
>
>    2. 在最后一条边后接上一条边，设 $x$ 为 $S$ 中最后一条边的终点，由性质2可得这条边需要满足其起点为 $x$ 或 $x$ 在 $Tr$ 上的祖先。
>
> 将生成的新边集也加入小根堆。重复以上操作 $k-1$ 次后求出的就是从 $S$ 到 $T$ 的第 $k$ 短路。
>
> 对于每个结点 $x$，我们将以其为起点的边的 $\Delta e$ 建成一个小根堆。为了方便查找一个结点 $x$ 与 $x$ 在 $Tr$ 上的祖先在小根堆上的信息，我们将这些信息合并在一个编号为 $x$ 的小根堆上。回顾以上生成新边集的方法，我们发现只要我们把紧接着可能的下一个边集加入小根堆，并保证这种生成方法可以覆盖所有可能的边集即可。记录最后选择的一条边在堆上对应的结点 $T$，有更优的方法生成新的边集：
>
> 1. 替换 $S$ 中的最后一条边为 $T$ 在堆上的左右儿子对应的边。
>
> 2. 在最后一条边后接上一条新的边，设 $x$ 为 $S$ 中最后一条边的终点，则接上编号为 $x$ 的小根堆的堆顶结点对应的边。
>
> 用这种方法，每次生成新的边集只会扩展出最多三个结点，小根堆中的结点总数是$O(n+k)$。
>
> 所以此算法的瓶颈在合并一个结点与其在$T$ 上的祖先的信息。使用可持久化可并堆优化合并一个结点与其在 $Tr$ 上的祖先的信息，每次将一个结点与其在 $Tr$ 上的父亲合并，时间复杂度为 $O((n+m)\log m+k\log k)$，空间复杂度为 $O(m+n\log m+k)$。这样的算法会更加的优。

## 注意事项

- 确保字典文件格式正确，每行一个单词。
- 如果使用自定义字典，请确保路径正确无误。
- 输入文本时避免使用特殊字符，除非它们被包含在字典中。

## 参考文献

https://oi-wiki.org/graph/kth-path/

## 联系方式

如果您有任何问题或建议，请联系 `j2259706494@gmail.com` 或访问项目的GitHub页面提交Issue。