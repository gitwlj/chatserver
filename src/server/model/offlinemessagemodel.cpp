#include "offlinemessagemodel.hpp"
#include "db.h"

// 存储用户的离线消息
void OfflineMsgModel::insert(int userid, string msg){
      //1.组装sql 语句

        char sql[1024]={0};
        /*Insert into这个，不用写字断了，因为两个字段都要插入，*/
        sprintf(sql, "insert into offlinemessage values(%d, '%s')",
        userid, msg.c_str() );

        MySQL mysql; // 这里 这里自己写的Mysql  y是小写。
 
        if (mysql.connect() ){
            mysql.update(sql);
        }

}

// 删除 用户的离线消息
void OfflineMsgModel::remove(int userid){
    
        char sql[1024]={0};
        /*Insert into这个，不用写字断了，因为两个字段都要插入，*/
        sprintf(sql, "delete from offlinemessage where userid=%d",
        userid );

        MySQL mysql; // 这里 这里自己写的Mysql  y是小写。
 
        if (mysql.connect() ){
            mysql.update(sql);
        }


}

// 查询用户的离线消息
vector<string> OfflineMsgModel::query(int userid){

     // 1.组装sql 语句

       char sql[1024] = {0};
       // 注意 这里 的表 User  我的 user  小写 可能会有问题：
       sprintf(sql, "select message from offlinemessage where userid = %d", userid);

       MySQL mysql; // 这里 这里自己写的Mysql  y是小写。
       vector<string> vec; 
       if (mysql.connect())
       {

           MYSQL_RES *res = mysql.query(sql); //返回的是   MYSQL_RES 里面就是返回结果 的内容
           if (res != nullptr)
           {   
               MYSQL_ROW row;                                 // 不等于null 等一查询成功了

               /* 这个 mysql_fetch_row() 返回当前的 行之后  会自动 跳到下一行   */
               while ( (row = mysql_fetch_row(res) ) != nullptr )//从这个资源 上 把他的行全拿出来 -- 由于是根据 主键来查的， 主键肯定不允许重复 --肯定就是一行
              {
                  vec.push_back(row[0]);
              }

              mysql_free_result(res); //释放资源
              return vec;

           }
       }

       return vec;

}