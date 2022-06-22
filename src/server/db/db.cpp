#include "db.h"
#include <muduo/base/Logging.h> // 主要 后面用到了人家的 日志 


#include<iostream>
using namespace std;


// 数据库配置信息

static string server = "127.0.0.1";
static string user = "root";
static string password = "123456";
static string dbname = "chat";

// 初始化数据库连接
MySQL::MySQL()
{
    _conn = mysql_init(nullptr);
}
// 释放数据库连接资源
// 这里用UserModel示例，通过UserModel如何对业务层封装底层数据库的操作。代码示例如下：
MySQL::~MySQL()
{
    if (_conn != nullptr)
        mysql_close(_conn);
}
// 连接数据库
bool MySQL::connect()
{


    MYSQL *p = mysql_real_connect(_conn, server.c_str(), user.c_str(),
                                  password.c_str(), dbname.c_str(), 3306, nullptr, 0);

       //cout<< "##MySQL::connect---32"<< endl;                              
    if (p != nullptr)
    {
        /*c和 c++ 因为c 和c++ 默认是ASCIII码，如果说 是直接从数据库上拉取这个中文的话 ，在我们本地的代码上  会显示 问号， 显示不正确 */
        mysql_query(_conn, "set names gbk");
          LOG_INFO << "connect mysql success!!!";
            //cout<< "##MySQL::connect---38"<< endl;            
    }else {
          LOG_INFO << "connect mysql fail !!!";

    }
    return p;
} 
// 更新操作
bool MySQL::update(string sql)
{
    if (mysql_query(_conn, sql.c_str()))
    {
        LOG_INFO << __FILE__ << ":" << __LINE__ << ":"
                 << sql << "更新失败!";
        return false;
    }

       cout<< "## MySQL::update55"<< endl;            
    return true;
}
// 查询操作 -- 是需要返回结果的
MYSQL_RES *MySQL::query(string sql)
{
    if (mysql_query(_conn, sql.c_str()))
    {
        LOG_INFO << __FILE__ << ":" << __LINE__ << ":"
                 << sql << "查询失败!";
        return nullptr;
    }
    return mysql_use_result(_conn);
}

MYSQL *MySQL::getConnection()
{
    return _conn;
}