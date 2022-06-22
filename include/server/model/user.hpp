#ifndef USER_H
#define USER_H

#include <string>
using  namespace std;


// 匹配User表的ORM 类
class User{

public:

    User (int id = -1,string name = "",string passwd = "",string state="offline"){
        this->id = id;
        this->name = name;
        this->passwd = passwd;
        this->state = state;
    }

    /*此时 一定要加 this  因为 你现在的成员变量  跟形参的名字一样*/

    void setId(int id){
        this->id =id;
    }
    
    void setName(string  name){
        this->name =name;
    }
    
    void setPasswd(string  pwd){
        this->passwd =pwd;
    }
    
    void setState(string  state){
        this->state =state;
    }


    int getId(){
        return this->id;
    }
    
    string getName(){
       return this->name;
    }
    
    string getPasswd(){
        return this->passwd;
    }
    
    string getState(){
        return this->state;
    }



protected:
    int id;
    string name;
    string passwd;
    string state;

};



#endif