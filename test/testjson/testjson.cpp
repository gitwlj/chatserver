#include "json.hpp"
//using namespace nlohmann;
using json = nlohmann::json;


#include <iostream>
#include <vector>
#include <string>
#include <map>


using namespace std;

// json 序列化实例1
void func1(){

    json js;// 底层就是用 链式哈希表 来存储的  
    js["msgtype"] = 2;
    js["from"] = "zhang san";
    js["to"] = "li si";
    js["msg"] = "hello, what are you doing now?";

    cout << js<< endl;
    // 通 过 网络的形式发送出去
    string  sendBuf = js.dump();
    cout << sendBuf << endl;
    cout << sendBuf.c_str() << endl;
}


// json 序列化实例2
void func2(){

    json js;
    // 填加 数组
    js["id"] = {1,2,3,4,5};
    //  添加  key -value
    js["name"] = "zhang san";
    // 添加 对象
    js["msg"]["zhang san"] = "hello zhang san !";
    js["msg"]["liu shou"] ="hello liu china";
    // 上面两句话 等同于 这一次性 添加数组对象
    js["msg"] = {{"zhang san","hello zhang san !"},{"liu shou","hello liu china"} };

    cout << js << endl; 
}


// json 序列化实例3--容器
void func3(){

    json js;
    // 直接序列化   vector 容器
    vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);

    js["list"] = vec;
     
     // 直接序列化  一个map 容器
    map<int ,string>m;
    m.insert({1,"黄山"});
    m.insert({2,"华山"});
    m.insert({3,"泰山"});
    js["path"] = m;

    cout << js << endl;

    string sendBuf = js.dump(); // json 数据对象 ----》序列化 json 字符串 
    cout << sendBuf << endl;


}

// json  反序列化

string func1f(){

    json js;// 底层就是用 链式哈希表 来存储的  
    js["msg_type"] = 2;
    js["from"] = "zhang san";
    js["to"] = "li si";
    js["msg"] = "hello, what are you doing now?";


    string  sendBuf = js.dump();
    // cout << sendBuf << endl;
    // cout << sendBuf.c_str() << endl;

    return sendBuf;
}


// json 序列化实例2
string func2f(){

    json js;
    // 填加 数组
    js["id"] = {1,2,3,4,5};
    //  添加  key -value
    js["name"] = "zhang san";
    // 添加 对象
    js["msg"]["zhang san"] = "hello zhang san !";
    js["msg"]["liu shou"] ="hello liu china";
    // 上面两句话 等同于 这一次性 添加数组对象
    js["msg"] = {{"zhang san","hello zhang san !"},{"liu shou","hello liu china"} };

    //cout << js << endl;

    return js.dump(); 
}


// json 序列化实例3--容器
string func3f(){

    json js;
    // 直接序列化   vector 容器
    vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);

    js["list"] = vec;
     
     // 直接序列化  一个map 容器
    map<int ,string>m;
    m.insert({1,"黄山"});
    m.insert({2,"华山"});
    m.insert({3,"泰山"});
    js["path"] = m;

    //cout << js << endl;

    string sendBuf = js.dump(); // json 数据对象 ----》序列化 json 字符串 
    //cout << sendBuf << endl;
    return sendBuf;

}




int main(){
    //func1();
    //func2();
    //func3();

    //反序列
     string recvBuf = func3f();
    // 数据的反序列化 json字符串 --》 反序列化 数据对象
    
    json  js_buf = json::parse(recvBuf);
    
    // cout << js_buf["msg_type"] << endl;
    // cout << js_buf["from"] << endl;
    // cout << js_buf["to"] << endl;
    // cout << js_buf["msg"] << endl;

    // cout << js_buf["id"] << endl;

    // auto arr = js_buf["id"];
    // cout<< arr[2] << endl;

    // auto msgjs = js_buf["msg"];
    // cout << msgjs["zhang san"] << endl;
    // cout << msgjs["liu shou"] << endl;


    vector<int> vec = js_buf["list"];

    for (int &v:vec){
        cout << v << " ";
    }
    cout << endl;

    map<int ,string> mymap = js_buf["path"];

    for (auto  &p : mymap){
        cout << p.first << " " << p.second<< endl;
    } 


    return 0;
}