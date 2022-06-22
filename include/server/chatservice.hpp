#ifndef CHATSERVICE_H
#define CHATSERVICE_H

#include <muduo/net/TcpConnection.h>

#include <unordered_map>
#include <functional>

#include <mutex>


using namespace std;

using  namespace muduo;
using namespace muduo::net;

#include "json.hpp"
using namespace placeholders;
using  json = nlohmann::json;


#include "offlinemessagemodel.hpp"
#include "friendmodel.hpp"
#include "groupmodel.hpp"
#include "usermodel.hpp"

#include "redis.hpp"

/*消息事件 对应的回调 */
// 表示 处理消息事件 的事件回调方法类型 
using  MsgHandler = std::function< void ( const TcpConnectionPtr &conn, json &js, Timestamp time) >;



//聊天服务器业务类 --
/* 这个 业务类 实例 有一个 或多个 实例没什么 区别 ，实际上 有一个实例就够了 
    -- 因此 就采用 单例模式 呗

*/
class ChatService {

public:
    /* 获取单例对象的接口函数  --- 就是 构造函数 私有化之后  ，暴露的一个接口*/
    static ChatService * instance();


    /* 处理 登录业务 */
    void login( const TcpConnectionPtr &conn, json &js, Timestamp time);//登录方法
     /* 处理 注册 业务 */
    void reg( const TcpConnectionPtr &conn, json &js, Timestamp time); // 注册方法 

    //  一对一 聊天消息 
    void oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time);

     //  添加好友 业务 
    void addFriend(const TcpConnectionPtr &conn, json &js, Timestamp time);

     //  创建群组 业务 
    void createGroup(const TcpConnectionPtr &conn, json &js, Timestamp time);

     //  加入群组 业务 
    void addGroup(const TcpConnectionPtr &conn, json &js, Timestamp time);
    
    //  群组聊天 业务 
    void groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time);

 //  处理注销 业务 
    void loginout(const TcpConnectionPtr &conn, json &js, Timestamp time);




 


    // 处理 客户端 异常 退出
    void clientCloseException(const TcpConnectionPtr &conn);
    
    // 服务器 异常后业务业务重置方法
    void reset();
	
	   // 获取 消息对应的 处理器
    MsgHandler getHandler( int  msgid);

    // 获取 redis消息对应的回调函数
    void handleRedisSubscribeMessage(int, string);



private:    
    /* 单例来设计 --那么 构造函数 选哟私有化， 然后写一个 唯一的实例 ，然后暴露 一个接口 */
    ChatService ();

    /* 需要给这个  msgid 映射 事件回调
     -- 存储 消息id 何其对应的 业务处理 方法 
     */
    unordered_map <int , MsgHandler> _msgHandlerMap;

    // 存储 在线用户的通信连接

    unordered_map<int , TcpConnectionPtr> _userConnMap;

    // 定义 互斥锁 ，保证 _userConnMap的 线程安全
    mutex _connnMutex;

    //数据操作类
    UserModel _usermoel;
    OfflineMsgModel _offlinemsgmodel;
    FriendModel _friendmodel;
    GroupModel _groupmodel;

    //redis 操作对象 
    Redis _redis;
};


#endif