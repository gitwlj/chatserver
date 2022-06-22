#ifndef  PUBLICMSG_H
#define PUBLICMSG_H

/*service  和 client 的公共文件*/


/* enum  默认 从 0 开始 可是加上1 之后 就从1 开始 
 LOGIN_MSG  跟 login() 方法 绑定起来 ， 当收到 LOGIN_MSG 的时候 ，就能够 自动的调用这个 login()函数 
*/
enum EnMsgTye{
    LOGIN_MSG = 1,// 登录消息
    LOGINOUT_MSG,    // 注销消息
    LOGIN_MSG_ACK,
    REG_MSG,         // 注册消息
    REG_MSG_ACK,     // 注册相应消息
    ONE_CHAT_MSG,  // 聊天消息
    ADD_FRIEND_MSG, // 添加好友消息
    
    CREATE_GROUP_MSG, //创建群组的消息
    ADD_GROUP_MSG,     // 加入群组
    GROUP_CHAT_MSG,    // 群聊天

 

};

#endif