#include "chatservice.hpp"
#include "publicmsg.hpp"

#include <muduo/base/Logging.h>

#include <vector>
#include <iostream>

using namespace std;
using namespace muduo;


/*静态方法 实现的时候 不用 加 static */
// static ChatService * instance();

ChatService *ChatService::instance()
{

    static ChatService service; // 单列对象   而且还是 线程安全的
    return &service;
}

/*  注册消息 以及对应的回调操作*/
 ChatService::ChatService ()
 {
     // 用户基本业务 管理相关事件处理 回调注册
    _msgHandlerMap.insert( {LOGIN_MSG , std::bind(&ChatService::login, this, _1, _2, _3 ) });
    _msgHandlerMap.insert( {LOGINOUT_MSG , std::bind(&ChatService::loginout, this, _1, _2, _3 ) });
    _msgHandlerMap.insert( {REG_MSG , std::bind(&ChatService::reg, this, _1, _2, _3 ) });
    _msgHandlerMap.insert( {ONE_CHAT_MSG , std::bind(&ChatService::oneChat, this, _1, _2, _3 ) });
    _msgHandlerMap.insert( {ADD_FRIEND_MSG , std::bind(&ChatService::addFriend, this, _1, _2, _3 ) });

    // 群组 业务管理相关事件 处理回调注册
    _msgHandlerMap.insert( {CREATE_GROUP_MSG , std::bind(&ChatService::createGroup, this, _1, _2, _3 ) });
    _msgHandlerMap.insert( {ADD_GROUP_MSG , std::bind(&ChatService::addGroup, this, _1, _2, _3 ) });
    _msgHandlerMap.insert( {GROUP_CHAT_MSG , std::bind(&ChatService::groupChat, this, _1, _2, _3 ) });

   // 连接redis 服务器
   if (_redis.connect()){

       // 设置上报redis消息的回调
       _redis.init_notify_handler(std::bind(&ChatService::handleRedisSubscribeMessage,this, _1, _2));
   }



 }

 // 服务器 异常后业务业务重置方法
  void ChatService::reset(){
        // 把online 状态的用户，设置成offile
        _usermoel.resetState();

  }


 MsgHandler  ChatService::getHandler( int  msgid)
 {
     auto it = _msgHandlerMap.find(msgid);

     if (it == _msgHandlerMap.end() ){
         /* 注意使用 muduo库的时候 ， 不要输出 endl，人家会自动输出的， 人家没有对着 endl 进行重载  */
        // LOG_ERROR<<"msgid" << msgid << "can not find Handler!" ;

         // 放回 一个默认的处理器， 空操作---lanba表达式 --> 这个 默认的处理器就执行 打印的操作
         return [=] (const TcpConnectionPtr &conn, json &js, Timestamp time){
              LOG_ERROR<<"msgid" << msgid << "can not find Handler!" ; 
         };



     }else {
         return _msgHandlerMap[msgid];
     }

 }


/* 处理 登录业务  --对 id  pwd  pwd 进行对比
*/
void ChatService::login(const TcpConnectionPtr &conn, json &js, Timestamp time) //登录方法
{
    //LOG_INFO<< "do login service!!!";

    // 拿到 对应的 name  和 pwd
    int id = js["id"].get<int>();     //js["id"] 放回的是 字符串的哟 注意 ind  转换一下
    string pwd = js["password"];
    
    User  user = _usermoel.query(id); // 传入 一个id 值 我们一般在表中 用主键查 --返回 这个id对应色数据 


    //
    if( user.getId() ==id &&  user.getPasswd() == pwd){       // -1 是默认找到的
        // 密码校验正确--登录成功

        if (user.getState() =="online"){//该用户 已经登录--不允许重复登录

            json response;

            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 2;
            response["errmsg"] = "this account is using , input another";

            conn->send(response.dump() );


        }else{
            // 登录成功，记录用户连接信息
            {
              lock_guard<mutex> lock(_connnMutex);
              _userConnMap.insert({id,conn});

            }
            //id 用户登录成功后， 向redis 订阅channel(id)
            _redis.subscribe(id);

            //登录成功--首先要跟更新 用户状态信息

            user.setState("online");
            _usermoel.updateState(user);// 需要更新数据库呀


             json response;

            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 0;
            response["id"] = user.getId();
            response["name"] = user.getName();

            /*查询 该用户是否 有离线消息  要有的话呢，你就直接装在这个js里边儿，是不是给他带回去，*/
           vector<string> vec = _offlinemsgmodel.query(id);
           if (!vec.empty()){
               response["offlinemsg"] = vec;
               // 读取该用户的离线消息后， 把该用户的所有离线消息删除掉
               _offlinemsgmodel.remove(id);
           }

           // 查询改用户的好友信息并返回

           vector<User> uservec = _friendmodel.query(id);
            if (!uservec.empty() ){
                //response["friend"] = vec; 这样写 可能不太好--因为 User 是自定义的类型，json 可能不会识别

                vector<string> vec2;
                for (auto & user:uservec){
                    json jstemp;
                    jstemp["id"] = user.getId();
                    jstemp["name"] = user.getName();
                    jstemp["state"] = user.getState();

                    vec2.push_back(jstemp.dump());
                }
                response["friends"] = vec2;
                
            }

            // 查询 用户的群组信息
            vector<Group> groupuserVec = _groupmodel.queryGroup(id);
            if (!groupuserVec.empty()){
                // group:[{groupid:[xxx, xxx, xxx, xxx]}]
                vector<string>groupV;
                for (Group & group : groupuserVec){
                    json grpjson;
                    grpjson["id"] = group.getId();
                    grpjson["groupname"] = group.getName();
                    grpjson["groupdesc"] = group.getDesc();

                    vector<string>userV;
                    for (GroupUser & user: group.getUser()){
                        json userjs;
                        userjs["id"] = user.getId();
                        userjs["name"] = user.getName();
                        userjs["state"] = user.getState();
                        userjs["role"] = user.getRole();

                        userV.push_back( userjs.dump() );
                    }

                    grpjson["users"] = userV;
                    groupV.push_back( grpjson.dump() );
                }

                response["groups"] = groupV;
            }

          
            conn->send(response.dump() );

        }
        

    }else{
        /*这里 你可以 看看 是不是 业务可以做的细一点 */
        //登录失败--用户不存在 或者 用户存在 但是密码错误

         json response;

        response["msgid"] = LOGIN_MSG_ACK;
        response["errno"] = 1;
         response["errmsg"] = "id or passwd  is invalid";
        
        conn->send(response.dump() );


    }






}
/* 处理 注册 业务 */
/*  name  passwd */
void ChatService::reg(const TcpConnectionPtr &conn, json &js, Timestamp time)// 注册方法
{

    //LOG_INFO<< "do reg service!!!"; 最开始的测试

    string name = js["name"];
    string pwd = js["password"];


    User user;
    user.setName(name);
    user.setPasswd(pwd);

    bool state = _usermoel.insert(user);
    if (state){
        // 注册成功
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 0;   
         // response["erron"] = 0; 2022 0522 的bug
         response["id"] = user.getId();


        conn->send(response.dump());

    }else {
        // 注册 失败 
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 1;
       // response["id"] = user.getId();// 注册失败 就没有 这个id
        conn->send(response.dump());

    }

}

void ChatService::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    
    int toid = js["toid"].get<int>();

    {
        lock_guard<mutex> lock(_connnMutex);
        auto it = _userConnMap.find(toid);

        if (it != _userConnMap.end()){
            // toid 在线 转发消息---服务器 主动的 推送消息 给 toid 用户 
            /*A给B发a发到服务器上的消息，原封不动的再发给谁就行了，再发个币，相当于服务器在这里边做了一次消息中转吗*/

            /*说明 在同一台chatserver 注册的*/
            it->second->send( js.dump());// 原封不动的转发gei B  
            return ;
        }


    }

    // 查询toid 是否在在线--看看是不是在其他服务器登录着
    User user = _usermoel.query(toid);
    if(user.getState() =="online"){
        _redis.publish(toid, js.dump());
        return ;
    }


    // toid 不在线  存储离线消息
    _offlinemsgmodel.insert(toid, js.dump() );
}

//  添加好友 业务 msgid  id friendid
void ChatService::addFriend(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    int friendid = js["friendid"].get<int>();

    //cout<< "ChatService::addFriend:==userid ---- friendid=="<<  userid << friendid << endl;
    //存储好友信息
    _friendmodel.insert(userid, friendid);

}

//  创建群组 业务--- id (那个用户创建群) --groupname()  --groupdesc()
void ChatService::createGroup(const TcpConnectionPtr &conn, json &js, Timestamp time){

    int userid = js["id"].get<int>();
    string name = js["groupname"];
    string desc = js["groupdesc"];

    // 存储 新创建的群组信息
    Group group(-1,name , desc);
    if (_groupmodel.createGroup(group)){
        //存储群组创建人信息
        _groupmodel.addGroup(userid, group.getId(),"creator");
    }

}

//  加入群组 业务  userid(用户id)   以及 你要加入 groupid 组id --还有角色 
void ChatService::addGroup(const TcpConnectionPtr &conn, json &js, Timestamp time){
    int userid= js["id"].get<int>();
    int groupid = js["groupid"].get<int>();

    _groupmodel.addGroup(userid , groupid, "normal");

}

//  群组聊天 业务
void ChatService::groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time){
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();

    // 得到 这个 群组 的其他用户
    vector<int> useridVec = _groupmodel.queryGroupUsers(userid, groupid); 

    /* 在我操作这个map的时候  不允许 其他人 其他线程  在userconnnectonmap 里边进行其他操作-- 其他的增删改查的操作
    因为 c++ 中 stl 中 map 它本身就不是一个线程安全的map 
      */

    lock_guard<mutex> lock(_connnMutex);
    for(int id : useridVec)
    {   
        //lock_guard<mutex> lock(_connnMutex);// 这个 位置不太对 --这字啊循环里面 会不断的加锁 解锁 操作--不过也可以--除了循环的右括号 就自动给解锁了
        auto it = _userConnMap.find(id);
        if (it != _userConnMap.end())
        {
            // 转发消息
            it->second->send( js.dump());
        }else {

            // 查询id 是否在在线--看看是不是在其他服务器登录着
            User user = _usermoel.query(id);
            if (user.getState() == "online")
            {
                _redis.publish(id, js.dump());

            }else{
                // 存储离线消息
                _offlinemsgmodel.insert(id,js.dump());
            }
        }

    }

}

void ChatService::loginout(const TcpConnectionPtr &conn, json &js, Timestamp time){

    int userid = js["id"].get<int>();
    //既然 要查询map 就要注意互斥

    {
        lock_guard<mutex> lock(_connnMutex);
        auto it = _userConnMap.find(userid);

        if(it != _userConnMap.end()){
            _userConnMap.erase(it);
        }
    }

    // 用户注销 相当于下线 在redis 中 取消订阅通道
    _redis.unsubscribe(userid);
    // 更新用户的额状态信息
    //user.setState("offline");
    User user(userid,"","","offline");
     _usermoel.updateState(user); // 也要更新下对应的数据库信息



}
//处理客户端异常退出
void ChatService::clientCloseException(const TcpConnectionPtr &conn)
{
    User user;
    {
        lock_guard<mutex> lock(_connnMutex);
        for (auto it = _userConnMap.begin(); it != _userConnMap.end(); ++it)
        {
            if (it->second == conn)
            {
                // 找到了对应的连接
                /*从map 表中 删除用户的连接信息*/
                user.setId(it->first);
                _userConnMap.erase(it);
                break;
            }
        }
    }

    // 用户注销 相当于下线 在redis 中 取消订阅通道
    _redis.unsubscribe(user.getId());

    // 更新用户的额状态信息
    if (user.getId() != -1)
    {
        user.setState("offline");
        _usermoel.updateState(user); // 也要更新下对应的数据库信息
    }
}


void ChatService::handleRedisSubscribeMessage(int userid, string msg)
{
     //测试信息cout<<"handleRedisSubcriMessage:---------userid:"<<userid<<"---msg "<<msg<<endl;
    lock_guard<mutex> lock(_connnMutex);
    auto it = _userConnMap.find(userid);

    if(it != _userConnMap.end() ){
        it->second->send(msg);
        return;
    }

    /*万一 在上报的过程中  高阳 下线了？ 所以需要下面的东西
    还有就是 往通道中发消息  或者通道取消息过程中--- 某某下线了 */
    //存储该用户的离线消息
    _offlinemsgmodel.insert(userid, msg);
    
}
