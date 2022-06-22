#ifndef REDIS_H
#define REDIS_H


#include <hiredis/hiredis.h>
#include <thread>
#include <functional>

using namespace std;
  

class   Redis{

public:
    Redis();
    ~Redis();
    // 连接redis服务器
    bool connect();
    
    //向redis 指定的通道channel 发布消息
    bool publish(int channel, string messages);
    
  //向redis 指定的通道channel 订阅消息
    bool subscribe(int channel);

     //向redis 指定的通道channel 取消订阅消息
    bool unsubscribe(int channel);
    
    // 在独立线程中 接受订阅通道中的消息
    void observer_channel_message();

    void init_notify_handler(function<void (int, string)>fn);


private:

    /*为什么 有两个呢？---因为从刚刚的客户端的来看  命令上来看-- subscribe之后 那是阻塞的----publish 和 subscribe 不能在用一个上下文 操作 */

    // hredis 同步上下文对象  负责publish 消息
    redisContext * _publish_context;

     // hredis 同步上下文对象  负责 subscribe 消息
    redisContext * _subcribe_context;

    // 回调操作 收到了订阅消息 给server层上报
    function<void (int,string)>_notify_message_handler;


};


#endif