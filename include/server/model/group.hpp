#ifndef GROUP_H
#define GROUP_H


#include<vector>
#include <string>

#include "groupuser.hpp"
using namespace std;

class Group{

public:
    Group(int id =-1, string name = "",string desc = ""){
        this->id = id;
        this->name = name;
        this->desc = desc;
    }

    void setId(int id){
        this->id = id;
    }
    
    void setName(string name){
        this->name = name;
    }

    void setDesc(string  desc){
        this->desc = desc;
    }

    int getId(){
        return this->id;
    }

    string getName(){
        return this->name;
    }
    
    string getDesc(){
        return this->desc;
    }
    /* 注意这里返回的 这个的引用的*/
    vector<GroupUser> & getUser(){
        return this->users;
    }



private:
        int id;
        string name;
        string desc;

        /*为什么还要再加一个vector呢，就是说呢，
        因为我到时候要获知，一个组里边有多少成员，所以我从数据库查出来以后呢，
        我就都把这个组的成员放在这个vector当中，给业务层去使用，好的吧
        放回的 还得 包括 角色信息 因此user 不适合 
        */
        
        vector<GroupUser> users;//为什么还要再加一个vector呢，就是说呢，因为我到时候要获知，一个组里边有多少成员，所以我从数据库查出来以后呢，我就都把这个组的成员放在这个vector当中，给业务层去使用，好的吧

};



#endif