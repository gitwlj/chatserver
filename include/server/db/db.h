#ifndef  DB_H
#define DB_H

#include <mysql/mysql.h>


#include <string>
using namespace std;

/*配置信息 就不要往这里放 -- 这里没有实现代码 */
// 数据库配置信息
/*
static string server = "127.0.0.1";
static string user = "root";
static string password = "123456";
static string dbname = "chat";
*/

// 数据库操作类
class MySQL
{
public:
    // 初始化数据库连接
    MySQL();
   
    // 释放数据库连接资源
    // 这里用UserModel示例，通过UserModel如何对业务层封装底层数据库的操作。代码示例如下： 
    ~MySQL();
  
    // 连接数据库
    bool connect();
   
    // 更新操作
    bool update(string sql);
   
    // 查询操作 -- 是需要返回结果的
    MYSQL_RES *query(string sql);

    MYSQL * getConnection();
   
private:
    MYSQL *_conn;
};

#endif