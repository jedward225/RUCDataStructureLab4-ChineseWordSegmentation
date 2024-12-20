cmake_minimum_required(VERSION 3.10)

# 设置项目名称和版本
project(ChineseWordSegmentation VERSION 1.0)

# 指定C++标准为C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 添加包含目录（如果需要）
include_directories(${PROJECT_SOURCE_DIR}/include)

# 定义源文件列表
file(GLOB_RECURSE SOURCES "src/*.cpp")

# 添加可执行文件，并指定所有源文件
add_executable(ChineseWordSegmentation ${SOURCES})

# 如果有任何库或额外的链接需求，可以在这里添加
# target_link_libraries(ChineseWordSegmentation ...)