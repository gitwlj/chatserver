




#配置 头文件 搜素 路劲
#3
#include_directories()


add_compile_options(-std=c++11)
#配置 库文件 搜素 路劲
#4
#link_directories()


#需要编译 源文件 列表
#这个 src_list  可以小写
set(src_list ./muduo_server.cpp)

#万一 有 100 个cpp文件呢?
# 把 . 指定路径下的 所有源文件名字 放入变量名SRC_LIST 
#aux_source_directory(. SRC_LIST)

#设置可执行文件  存放路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)



#生成可执行 文件  #1
# 表示 生成可执行文件 server 有src_list 变量 所定义的源文件 编译而来
add_executable(server ${src_list})

#6 链接 库 ---- 表示 server 这个目标程序， 需要链接 那些muduo_net muduo_base pthread 
# 这里不用 加-l  这个会自动加上 -l 的
target_link_libraries(server muduo_net muduo_base pthread)