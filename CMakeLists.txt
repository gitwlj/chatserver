#外层的CMakeLists.txt
cmake_minimum_required(VERSION 3.0)
project(main) 

#配置编译选项

# 加上 这个 -g  代表这个文件 是可以调式的  --如果没有这个 也可以 那就是  可执行文件  是不能进行调试的哦
# 2
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)   # 在这个 CMAKE_CXX_FLAGS 基础上 在加 -g 选项 或者 -O2 选项

#配置 最终可执行文件输出的路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#配置头文件 的搜索路径
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/server)
include_directories(${PROJECT_SOURCE_DIR}/include/server/db)
include_directories(${PROJECT_SOURCE_DIR}/include/server/model)
include_directories(${PROJECT_SOURCE_DIR}/include/server/redis)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty)

add_compile_options(-std=c++11)

#指定 搜素的子目录 
add_subdirectory(src)