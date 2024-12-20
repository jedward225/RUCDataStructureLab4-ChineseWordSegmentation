#include <iostream>
#include "Segmenter.h"

int main() {
    Segmenter segmenter("dict.txt");

    std::string text;
    while (true) {
        std::cout << "请输入要分词的文本（输入'exit'退出）: ";
        std::getline(std::cin, text);

        if (text == "exit") break;

        auto result = segmenter.segment(text);
        for (size_t i = 0; i < result.size(); ++i) {
            if (i != 0) std::cout << "/";
            std::cout << result[i];
        }
        std::cout << std::endl;
    }

    return 0;
}