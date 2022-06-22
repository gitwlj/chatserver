#ifndef GROUPMODEL_H

#define GROUPMODEL_H

#include "group.hpp"
#include <vector>
#include <string>
using namespace std;

//维护群组信息的操作接口方法

class GroupModel{

public:

    // 创建群组
    bool createGroup( Group &group);
    
    // 加入群组
    void addGroup(int userid, int groupid, string role);
   
    // 查询 用户所在群组信息
    vector <Group> queryGroup(int userid);

    // 根据指定的groupid 查询群组用户id列表 ，除了userid 自己 主要用户群聊业务给其他成员群发消息 
    //vector<int> queryGroupUsers(int userid,int groupid);
     vector <int > queryGroupUsers(int userid, int groupid);

};
#endif