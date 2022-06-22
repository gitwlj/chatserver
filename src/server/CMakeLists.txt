

#指定源文件---定义了 一个 src_list 变量 包含了 该目录下 所有的源文件

aux_source_directory(. src_list)
aux_source_directory(./db db_list)
aux_source_directory(./model model_list)
aux_source_directory(./redis redis_list)

#指定 生成可执行文件
add_executable(ChatServer ${src_list} ${db_list}  ${model_list}  ${redis_list})

#指定 可执行文件 链接时 需要的库文件 
target_link_libraries(ChatServer muduo_net muduo_base mysqlclient hiredis pthread)