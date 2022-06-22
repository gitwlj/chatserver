#ifndef GROUPUSER_H
#define GROUPUSER_H

#include "user.hpp"


/*注意这里  public 继承  User --对应的User 要修改权限----private 是继承不到的 ---改为protect
 那些 方法 也继承差不多了
  User (int id = -1,string name = "",string passwd = "",string state="offline")
    void setId(int id) 
    void setName(string  name)
    void setPasswd(string  pwd)
    void setState
    int getId()
    string getName() 
    string getPasswd() 
    string getState()

*/
class GroupUser : public User
{
public:

    void setRole (string role){
        this->role = role;
    }

    string getRole(){
        return this->role;
    }    
private:
    string role;
};

#endif