#ifndef FRIENDMODEL_H
#define FRIENDMODEL_H

#include <vector>
using namespace std;
#include "user.hpp"
// 提供 Friend 好友 表的操作接口方法
class FriendModel{

public:
    // 添加好友关系
    void  insert(int userid , int friendid);


    // 返回用户好友列表
    vector <User> query(int userid);




};   



#endif