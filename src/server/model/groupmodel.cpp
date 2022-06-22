
#include "db.h"
#include "groupmodel.hpp"

// 创建群组---就是往 allgroup 添加数据
bool GroupModel::createGroup(Group &group)
{
     //1.组装sql 语句
        char sql[1024]={0};
        /*Insert into这个，不用写字断了，因为两个字段都要插入，*/
        sprintf(sql, "insert into allgroup(groupname, groupdesc) values('%s', '%s')",
        group.getName().c_str(), group.getDesc().c_str() );

        MySQL mysql; // 这里 这里自己写的Mysql  y是小写。

        if (mysql.connect() ){
            if (mysql.update(sql)){
                group.setId( mysql_insert_id( mysql.getConnection() ) );
                return true;
            }        
        }

        return false;
}

// 加入群组---groupuser 加数据
void GroupModel::addGroup(int userid, int groupid, string role)
{
     //1.组装sql 语句
        /*应该针对于group user跟all group进行一个良表的联合查询*/
        char sql[1024]={0};
        /*Insert into这个，不用写字断了，因为两个字段都要插入，*/
        sprintf(sql, "insert into groupuser values(%d, %d,'%s')",
        groupid,userid, role.c_str());

        MySQL mysql; // 这里 这里自己写的Mysql  y是小写。
        if (mysql.connect() ){
            mysql.update(sql);
        }

}

// 根据 指定的userid 查询群组用户id列表 除了userid 自己 主要用户群聊业务费群组其他成员发消息
vector<Group> GroupModel::queryGroup(int userid)
{   
    /* 
    1. 先根据 userid 在groupuser表中 查询 改用户的所属的群组信息
    2. 在根据群组信息 查询属于该群组的所有用户id， 并且和user 表 进行多表的联合查询 查出用户的详细信息
    */


    // 1.组装sql 语句

       char sql[1024] = {0};
       sprintf(sql, "select a.id, a.groupname, a.groupdesc from allgroup a  inner join  groupuser b on a.id = b.groupid where b.userid=%d",
        userid);

       vector<Group>groupVec; //存储 了这个用户 所在的所在的所有群组  以及这个群组里边儿所有的用户信息

       MySQL mysql; // 这里 这里自己写的Mysql  y是小写
       if (mysql.connect())
       {
           MYSQL_RES *res = mysql.query(sql); //返回的是   MYSQL_RES 里面就是返回结果 的内容
           if (res != nullptr)
           {   
               // 查询 userid 所有的群组信息

               MYSQL_ROW row;                                 // 不等于null 等一查询成功了
               /* 这个 mysql_fetch_row() 返回当前的 行之后  会自动 跳到下一行   */
               while ( (row = mysql_fetch_row(res) ) != nullptr )//从这个资源 上 把他的行全拿出来 -- 由于是根据 主键来查的， 主键肯定不允许重复 --肯定就是一行
              {
                  Group group;
                  group.setId(atoi(row[0]));
                  group.setName (row[1]);
                  group.setDesc (row[2]);

                 groupVec.push_back( group);
              }

              mysql_free_result(res); //释放资源
           }
       }

        // 查询群组的用户数量--- 主要是 写入 group 的vector<Groupuser> users;里面

        for (Group &group :  groupVec){

            sprintf(sql, "select a.id, a.name, a.state, b.grouprole from user a  inner join  groupuser b on  b.userid=a.id where b.groupid=%d",
            group.getId());


       
            MySQL mysql; // 这里 这里自己写的Mysql  y是小写。
            if (mysql.connect())
            {
                MYSQL_RES *res = mysql.query(sql); //返回的是   MYSQL_RES 里面就是返回结果 的内容
                if (res != nullptr)
                {   
                    // 查询 userid 所有的群组信息

                    MYSQL_ROW row;                                 // 不等于null 等一查询成功了
                    /* 这个 mysql_fetch_row() 返回当前的 行之后  会自动 跳到下一行   */
                    while ( (row = mysql_fetch_row(res) ) != nullptr )   //从这个资源 上 把他的行全拿出来 -- 由于是根据 主键来查的， 主键肯定不允许重复 --肯定就是一行
                    {
                        GroupUser user;
                        user.setId(atoi(row[0]));
                        user.setName (row[1]);
                        user.setState (row[2]);
                        user.setRole(row[3]);

                        group.getUser().push_back(user);// 注意这里的
                    }

                    mysql_free_result(res); //释放资源
                }
       }
      
       return groupVec;
    }


}

vector<int> GroupModel::queryGroupUsers(int userid, int groupid)
{

    // 1.组装sql 语句

    char sql[1024] = {0};

    /*为啥呢，因为我发群聊消息你就不要再给我转发了，
    是不是，我发群聊消息你应该查的查，查询的是这个群组里边除了我以外其他组员的是不是ID，
    然后放到这个victor里边儿，可以外边返回，*/

    sprintf(sql, "select userid from groupuser where groupid = %d and userid !=%d", groupid, userid);

    MySQL mysql; // 这里 这里自己写的Mysql  y是小写。

    vector<int> idVec;

    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql); //返回的是   MYSQL_RES 里面就是返回结果 的内容
        if (res != nullptr)
        {
            MYSQL_ROW row; // 不等于null 等一查询成功了

            /* 这个 mysql_fetch_row() 返回当前的 行之后  会自动 跳到下一行   */
            while ((row = mysql_fetch_row(res)) != nullptr) //从这个资源 上 把他的行全拿出来 -- 由于是根据 主键来查的， 主键肯定不允许重复 --肯定就是一行
            {
                idVec.push_back (atoi(row[0]));
            }
            mysql_free_result(res); //释放资源
        }
    }

    return idVec;
}