#include <iostream>
#include <fstream>
#include <limits>
#include "Segmenter.h"

int main(int argc, char* argv[]) {
    const char* dictPath = "dict.txt";
    if (argc > 1) {
        dictPath = argv[1];
    }

    std::cout << "Using dictionary file: " << dictPath << std::endl;

    try {
        Segmenter segmenter(dictPath);

        std::string text;
        int choice;
        int k;

        while (true) {
            std::cout << "请选择功能（1: Dijkstra最短路径分词, 2: A*第K短路分词, 0: 退出）: ";
            std::cin >> choice;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // 清除输入缓冲区

            if (choice == 0) break;

            if (choice == 2) {
                std::cout << "请输入k值: ";
                std::cin >> k;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // 清除输入缓冲区
            }

            std::cout << "请输入要分词的文本（输入'exit'退出）: ";
            std::getline(std::cin, text);

            if (text == "exit") break;

            if (choice == 1) {
                auto result = segmenter.segment(text);
                for (size_t i = 0; i < result.size(); ++i) {
                    if (i != 0) std::cout << "/";
                    std::cout << result[i];
                }
                std::cout << std::endl;
            } else if (choice == 2) {
                auto result = segmenter.segmentKthShortest(k, text);
                if (!result.empty()) {
                    for (size_t i = 0; i < result.size(); ++i) {
                        if (i != 0) std::cout << "/";
                        std::cout << result[i];
                    }
                    std::cout << std::endl;
                } else {
                    std::cout << "未找到第" << k << "短路径。" << std::endl;
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}