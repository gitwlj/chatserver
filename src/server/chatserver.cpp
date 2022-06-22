#include "chatserver.hpp"
#include "json.hpp"
#include "chatservice.hpp"


#include <functional>
#include <string>

using namespace std;
using namespace placeholders;
using  json = nlohmann::json;


 // 初始化 聊天 服务器对象
ChatServer::ChatServer(EventLoop *loop,
                       const InetAddress &listenAddr,
                       const string &nameArg)
    : _server(loop, listenAddr, nameArg), _loop(loop)
{
    //注册回调 链接

    _server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1) );

    // 注册 读写 回调链接
    _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3) );

    // 设置 线程数量
    _server.setThreadNum(4);

}

// 启动服务
void ChatServer::start()
{   
    _server.start();
}

// 上报 链接相关信息的回调函数
void ChatServer::onConnection(const TcpConnectionPtr &conn)
{
    // 客户端 断开连接 
    if (!conn->connected()){
        ChatService::instance()->clientCloseException(conn);// 异常关闭 
        conn->shutdown();// close (fd);
    }
}

//上报 读写事件相关信息的回调函数
void ChatServer::onMessage(const TcpConnectionPtr &conn,
                           Buffer *buffer,
                           Timestamp time)
{

    string  buf = buffer->retrieveAllAsString();

    //数据的反序列化
    json js = json::parse(buf);

    // 通过 js["msg_id"] 一个id 绑定一个 业务 ， 而不用想 以前一样 去id 字段 然后通过 判断 做 相应的业务
    // 通过 js["msg_id"]  获取 ==》 业务handler ---这个handler 就是事先绑定的，这个模块 在网络中  不可见 看不着的
     // 通过 js["msg_id"]  获取 ==》 业务handler --- 业务(拿到 conn js  time )
  
     /* 达到的目的 ： 完全解耦 网络模块的代码和业务模块的代码 ， 不要在这里指名道姓的 业务模块相关的方法 --实现 就是OOP的思想。
     具体实现： 基于面向 接口的  --接口 就是基于 抽象基类  编程， 或者就是  回调操作  
       */



      auto msgHandler = ChatService::instance()->getHandler( js["msgid"].get<int>() );  // 注意这  instance() 的括号 一定要加上 


      /*回调 消息 绑定好的 事件处理器 来执行相应的业务 */
      msgHandler(conn, js,time);
      

}