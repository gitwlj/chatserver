#include "chatserver.hpp"
#include "chatservice.hpp"

#include <iostream>

#include <signal.h>
using namespace std;

// 处理服务器 crtl +c 结束后，重置user 的状态信息
void restHandler(int){

    ChatService::instance()->reset();
    exit(0);

}

int main(int argc, char ** argv){

    if (argc <3){
        cerr << "command invalid! example: ./ChatServer 127.0.0.1 6000" << endl;
        exit(-1);
    }

    char *ip=argv[1];
    uint16_t port = atoi(argv[2]);

    signal (SIGINT, restHandler);
    
    EventLoop loop;
    InetAddress addr(ip,port);

    ChatServer server(&loop,addr,"CHatServer");
    server.start();

    loop.loop();// 开启事件循环，



    return 0;
}