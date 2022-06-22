#include "usermodel.hpp"
#include "db.h"

#include <iostream>
using namespace std;


   bool UserModel::insert(User &user){
       //1.组装sql 语句

        char sql[1024]={0};
        // 注意 这里 的表 User  我的 user  小写 可能会有问题：
        sprintf(sql, "insert into user(name, password, state) values('%s', '%s' ,'%s')",
        user.getName().c_str(), user.getPasswd().c_str(), user.getState().c_str() );

        MySQL mysql; // 这里 这里自己写的Mysql  y是小写。

        if (mysql.connect() ){

            if(mysql.update(sql)){
                
                // 获取 插入成功的用户数据生成的主键 id

                user.setId(mysql_insert_id(mysql.getConnection() ) );
                return true;
            }

        }


        return false;

   }

    User UserModel::query(int id)
   {

       // 1.组装sql 语句

       char sql[1024] = {0};
       // 注意 这里 的表 User  我的 user  小写 可能会有问题：
       sprintf(sql, "select * from user where id = %d", id);

       MySQL mysql; // 这里 这里自己写的Mysql  y是小写。

       if (mysql.connect())
       {

           MYSQL_RES *res = mysql.query(sql); //返回的是   MYSQL_RES 里面就是返回结果 的内容
           if (res != nullptr)
           {                                         // 不等于null 等一查询成功了
               MYSQL_ROW row = mysql_fetch_row(res); //从这个资源 上 把他的行全拿出来 -- 由于是根据 主键来查的， 主键肯定不允许重复 --肯定就是一行
               if (row != nullptr)
               {

                   User user;
                   user.setId( stoi (row[0]) );// ---2022 0518 注意 row[0] 是字符类型 需要转化 int
                   user.setName(row[1]);
                   user.setPasswd(row[2]);
                   user.setState(row[3]);
                   mysql_free_result(res); // 由于 * res 是指针 需要我们 释放 ---否则你这资源不断泄露
                   return user;
               }
           }
       }
       // 没有找到--就放回默认的

       return User(); /*   User (int id = -1,string name = "",string passwd = "",string state="offline")  */
    }

    bool UserModel::updateState(User user)
    {

        // 1.组装sql 语句

        char sql[1024] = {0};
        // 注意 这里 的表 User  我的 user  小写 可能会有问题：
        sprintf(sql, "update user set state = '%s' where id = %d",
                user.getState().c_str(), user.getId()); // 注意， format 里面是需要 char *  需要c_str() 转一下

        MySQL mysql; // 这里 这里自己写的Mysql  y是小写。

        if (mysql.connect())
        {

            if (mysql.update(sql))
            {

                return true;
            }
        }

        return false;

        //---出了作用域 Mysql 会自动释放连接资源 的update
    }

           // 更新用户的状态信息
    void  UserModel::resetState(){
        // 1.组装sql 语句

        char sql[1024] = {"update user set state = 'offline' where state = 'online'"};
        // 注意 这里 的表 User  我的 user  小写 可能会有问题：
        //sprintf(sql, "update user set state = 'offline' where state = online"); // 注意， format 里面是需要 char *  需要c_str() 转一下

        MySQL mysql; // 这里 这里自己写的Mysql  y是小写。

        if (mysql.connect())
        {
            mysql.update(sql);
           
        }
        //---出了作用域 Mysql 会自动释放连接资源 的update

    }
