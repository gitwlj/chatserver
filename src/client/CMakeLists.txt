
# 定义一个 SRC_LIST 变量，包含了改目录下所有的源文件
aux_source_directory(./ SRC_LIST)

#指定生成可执行文件
add_executable(ChatClient ${SRC_LIST})

# 指定可执行文件 链接时 需要的依赖文件
target_link_libraries(ChatClient pthread)
