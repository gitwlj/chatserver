#include "redis.hpp"
#include <iostream>

using namespace std;

Redis::Redis()
        :_publish_context(nullptr),_subcribe_context(nullptr){


}
Redis::~Redis(){
    if (_publish_context !=nullptr){
        redisFree(_publish_context);//释放资源
    }

    if(_subcribe_context !=nullptr){
        redisFree(_subcribe_context);
    }


}
// 连接redis服务器
bool Redis::connect(){
    // 负责publish 发布消息的上下文连接
    _publish_context = redisConnect("127.0.0.1",6379);
    if(_publish_context == nullptr){
        cerr<< "publish connect redis failed!" << endl;
        return false;
    }
      // 负责subscribe 订阅消息的上下文连接
    _subcribe_context = redisConnect("127.0.0.1",6379);
    if(_subcribe_context ==nullptr){
        cerr << "subcribe connect redis failed" << endl;
        return false;
    }

    // 在单独的线程中 监听通道上的事件， 有消息给业务层进行上报
    /*为什么 需要单独线程呢？
    因为subscribe 是阻塞的--会一直在等待的 ，不能因为一个订阅 就把人家服务器的一个线程给一直霸占了
    */
    thread t([&] () {
        observer_channel_message();
    });
    t.detach();

    cout << "connect redis-server success!" << endl;
    return true;

}

//向redis 指定的通道channel 发布消息
bool Redis::publish(int channel, string message){

    //测试信息cout<<"Redis::publish---"<<channel<<"message "<<message<<endl;

    redisReply * reply = (redisReply *) redisCommand(_publish_context,"PUBLISH %d %s",channel, message.c_str() );
    /*它的放回值 是 动态生成的结构体 -- 用完之后 需要释放哟*/
    if(nullptr == reply){
        cerr << "publish command failed"<< endl;
        return false;
    }
     /*它的放回值 是 动态生成的结构体 -- 用完之后 需要释放哟*/
    freeReplyObject( reply);
    return true;

}

//向redis 指定的通道channel 订阅消息
bool Redis::subscribe(int channel){

    /*在publish 中 用了redisCommand --为什么subscribute 不用了
   却用 redisAppendCommand()  还有redisBufferWrite() 
    其实这就是使用 同步发布订阅的一个问题所在 --- 
    redisCommand 做的事情就是 先把 你要发的命令 缓存本地 ，实际上redisCommand 第一个调用就是redisAppendCommand 就是缓存到本地
    第二个就是 调用redisBufferWrite 把命令发送到redis server上  然后调用 redisGetReply() 来阻塞等待 命令执行的结果

    这个subscribe 这个命令是有相应的 但是是以阻塞方式来等待的
    我们 服务器 向 redis subscribe 订阅通道， 希望的是 你订阅一下就完了 你不能在这 阻塞等待 这个通到上 发生的消息 
    一个chatsetver 在4 个线程（一个io + 3 个worker ），也就是说 以subscribe 的方式 你最多订阅 3 通道， 这是我们肯定接受不了的

    那为什么publish 可以 用了redisCommand---- 因为publish 一致性马上就有回复的 ，这个不会阻塞当前线程
    但是 subscribe 不可以，它会阻塞当前线程

    */
   /*我们能做的是--- 这个命令组装好之后 -缓存 然后发送就完了--因为这种方式不会 阻塞等待  redis server 给我们的响应*/


    // SUBSCRIBE    命令本身会造成线程阻塞等待通道里发送的事情， 这里只做订阅通道 不接受通道消息
    //通道消息的接受 专门 在obser_channel_message 函数中的独立线程中进行
    // 只负责发送命令 不阻塞 redis server 响应消息吗否则会和 notifyMsg 线程抢占响应资源
   if(REDIS_ERR == redisAppendCommand(this->_subcribe_context, "SUBSCRIBE %d", channel)){
       cerr << "subscribe command failed!" << endl;
       return false;
   }


    // redisBufferWrite 可以循环发送缓冲区 直到缓冲区数据发送完毕（done 被置1）
   int done =0;
   while(!done){
       if(REDIS_ERR == redisBufferWrite(this->_subcribe_context,&done)){
           cerr << "subscribe command failed!" << endl;
           return false;
       }
   }
   //没有 redisGetReply()

   return true;
}

//向redis 指定的通道channel 取消订阅消息
bool Redis::unsubscribe(int channel)
{
    if (REDIS_ERR == redisAppendCommand(this->_subcribe_context, "UNSUBSCRIBE %d", channel))
    {
        cerr << "subscribe command failed!" << endl;
        return false;
    }

    // redisBufferWrite 可以循环发送缓冲区 直到缓冲区数据发送完毕（done 被置1）
    int done = 0;
    while (!done)
    {
        if (REDIS_ERR == redisBufferWrite(this->_subcribe_context, &done))
        {
            cerr << "subscribe command failed!" << endl;
            return false;
        }
    }
    //没有 redisGetReply()

    return true;
}

// 在独立线程中 接受订阅通道中的消息
void Redis::observer_channel_message(){

    redisReply *reply = nullptr;
    //以循环阻塞的方式来等待
    while(REDIS_OK == redisGetReply(this->_subcribe_context, (void **)&reply )){
        // 订阅收到的消息 是一个带三元素的数组----message channel msg()--例如： "message" "13" "hello world !!!!!"
        if( reply != nullptr && reply->element[2]!= nullptr && reply->element[2]->str != nullptr){
            //给 业务层 上报 通道上发生的消息
              //测试信息cout<<"Redis::observer_channel_message:"<< reply->element[2]->str <<endl;
            _notify_message_handler(atoi(reply->element[1]->str) , reply->element[2]->str );
        }
        freeReplyObject(reply);
    }

    cerr<<">>>>>>>>>>>>>>>>>>>>>>> observer_channel_message quit<<<<<<<<<<<<<<<<<<<<<<<<<" <<endl;

}

void Redis::init_notify_handler(function<void(int, string)> fn){
    this->_notify_message_handler = fn;
}