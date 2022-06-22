#include "friendmodel.hpp"
#include "db.h"

#include<iostream>
using namespace std;

// 添加好友关系
void FriendModel::insert(int userid, int friendid)
{
    //1.组装sql 语句
        char sql[1024]={0};
        /*Insert into这个，不用写字断了，因为两个字段都要插入，*/

        //cout<< "FriendModel::insert:==userid ---- friendid=="<<  userid << friendid << endl;
        sprintf(sql, "insert into friend values(%d,%d)",userid, friendid );

        MySQL mysql; // 这里 这里自己写的Mysql  y是小写。
 
        if (mysql.connect() ){
            //cout<< "##2FriendModel::insert:==userid ---- friendid=="<<  userid << friendid << endl;
            mysql.update(sql);
        }
}

// 返回用户好友列表
vector<User> FriendModel::query(int userid)
{
     // 1.组装sql 语句

       char sql[1024] = {0};
       // 注意 这里 设计到了多表联查的问题 
       sprintf(sql, "select a.id, a.name, a.state from user a inner join friend b on b.friendid = a.id where b.userid=%d", userid);

       MySQL mysql; // 这里 这里自己写的Mysql  y是小写。
       vector<User> vec; 
       if (mysql.connect())
       {

           MYSQL_RES *res = mysql.query(sql); //返回的是   MYSQL_RES 里面就是返回结果 的内容
           if (res != nullptr)
           {   
               MYSQL_ROW row;                                 // 不等于null 等一查询成功了

               /* 这个 mysql_fetch_row() 返回当前的 行之后  会自动 跳到下一行   */
               while ( (row = mysql_fetch_row(res) ) != nullptr )//从这个资源 上 把他的行全拿出来 -- 由于是根据 主键来查的， 主键肯定不允许重复 --肯定就是一行
              {
                 User user;
                 user.setId(atoi( row[0]));
                 user.setName (row[1]);
                 user.setState(row[2]);

                 vec.emplace_back(user);

              }

              mysql_free_result(res); //释放资源
              return vec;

           }
       }

       return vec;
}
