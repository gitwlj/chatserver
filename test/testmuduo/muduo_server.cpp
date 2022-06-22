/*

muduo 网络库 给用户 提供了两个主要的类
TcpSever : 用于编写服务器 程序的
Tcpclient ： 便于编写 客户端 的


epoll + 线程池
好处： 能够把网络 io 代码和业务 代码 区分开
                        1.用户的连接 和 断开    2.用户的可读写事件

*/

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>

#include <iostream>
#include <functional> // 要用绑定器
#include <string>

using namespace std;
using namespace muduo;
using namespace muduo::net;
using namespace placeholders;

/*基于 muduo 网络库 开发服务器程序
    1.组合 TcpSever 对象  --简单来说就是在 ChatSever 中 建立 TcpSever 对象
    2. 创建 EventLoop事件循环对象指针
    3.  明确 TcpSever 构造函数 需要什么参数，输出 ChatSever 的构造函数
    4. 在当前 服务器类 的构造函数中 注册 处理连接的 回调函数 和处理读写事件 的 回调函数
    5. 设置合适的服务端 线程数量， muduo库 会自己 分配 i/o线程  和worker 线程
*/
class ChatSever
{

public:
    ChatSever(EventLoop *loop,               // 事件循环
              const InetAddress &listenAddr, // ip 地址 + 端口
              const string &nameArg)         //  服务器名字
        : _server(loop, listenAddr, nameArg), _loop(loop)
    {
        // func()  普通函数 调用 （我是知道 这个时候 要发生 这个事件， 我还知道这个事 怎么做 ）
        //  给服务器 注册 新用户连接的创建和断开 回调

        // 回调 是 什么时候 发生(我不知道)，  （和 发生之后做什么） 不在同一时刻
        // 现在我知道  新用户连接的创建和  已连接用户断开 要怎么做，   但是这两件 什么时候发生 我不知道，
        // 这需要 网络对端 发送相应的数据消息 ，给我 上报 ，我才知道 有哪些 用户的连接 或者 用户的断开了，
        //  谁帮我 监听呢？--需要这个 网络库--

        // 所以 我需要注册回调  当相应的事件发生以后 ， 他就会帮我去调用 这个回调函数 ，我关注 在回调 里关注我的业务就可以了

        _server.setConnectionCallback(std::bind(&ChatSever::onConnection, this, _1));
        // 绑定 这个this  到这个 对象方法中 ， 而且人家的函数中 还有 参数 一个

        // 给服务器 注册用户 读写事件 回调
        _server.setMessageCallback(std::bind(&ChatSever::onMessage, this, _1, _2, _3));
        //  _1, _2, _3 成为参数 占位符

        //设置 服务端线程数量 -- 一般 4 核cpu 的话 ， 设置为4 就差不多了--
        // 这样的话 就是 1 个 IO 线程 3 个worker线程
        _server.setThreadNum(4);
    }
    // 开启 事件循环
    void start()
    {
        _server.start();
    }

private:
    // 专门 处理 用户的连接创建和断开的 epoll  listenfd accept 这些都封装了，只暴露 回调的接口，onConnection
    void onConnection(const TcpConnectionPtr &conn)
    {

        if (conn->connected())
        {
            cout << conn->peerAddress().toIpPort() << " -> " << conn->localAddress().toIpPort() << "start :: online" << endl;
        }
        else
        {
            cout << conn->peerAddress().toIpPort() << " -> " << conn->localAddress().toIpPort() << "start :: offline" << endl;
            conn->shutdown(); // 类似 close(fd) ;
            // _loop->quit(); // 整个 服务器 退出循环
        
        }
    }
    // 专门 处理 用户的 读写事件
    void onMessage(const TcpConnectionPtr &conn, // 连接
                   Buffer *buffer,                  // 缓冲区
                   Timestamp time)               // 接受到数据的 时间信息
    {
        string recvbuf = buffer->retrieveAllAsString();
        cout << "recv data :" << recvbuf<< "time :" << time.toString() <<endl;

        // 在给你发送回去
        conn->send(recvbuf);

    }
    TcpServer _server; // 第一步 要做的事情
    EventLoop *_loop;  // #2  组合 EventLoop 对象---类似 epoll
};

int main(){

    EventLoop loop;// 创建 loop  -类似 epoll
    InetAddress addr("127.0.0.1",6000);
    ChatSever sever( &loop,addr,"ChatSever");
    sever.start(); // 启动服务 --类似将 listenfd  epoll_ctl  ---》epoll 
    loop.loop();// 类似 epoll_wait()  以阻塞方式 等待 新用户的连接， 已连接用户 的读写事件 等 
    return 0;


}