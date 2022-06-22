#ifndef USERMODEL_H
#define USERMODEL_H

#include "user.hpp"

// User 表的数据操作类 
class UserModel {

public:
    // User 表的增加方法
    /*传引用 可以修改 这个实参的值 ，增加的时候 id 是没法填的 ，id 是你增加到到这个数据库 里边， ID这个字段呢，是主键自增的
     自动生成 ，然后反过来 填到这个user 中 
     */
    bool insert(User &user);

   //根据用户号码 查询用户信息 
    User query(int id);
    // 更新用户的状态信息
    bool updateState(User user);

       // 重置用户的状态信息
    void resetState();




private:


};


#endif