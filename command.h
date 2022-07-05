/**
 * @file commend.h
 * @author cfq (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-07-02
 * 
 * @copyright Copyright (c) 2022
 * 解析客户端发来的请求
 */

#ifndef COMMAND_H_
#define COMMAND_H_

#include<string>
#include<vector>
#include<set>
#include<unordered_set>
#include "skiplist.h"
using  std::string;
using std::vector;

const std::unordered_set<std::string>all_order{"set","get","del","dump","load"};
const int order_number = 5;
SkipList<int,string> skiplist(18);

class Command{
public:
    Command(){}
    Command(string s):request(s){}
    ~Command(){}
    void setCommand(string);
    string getCommand() const;
    void split_command();
    bool is_vaild_split(char *);
    void do_command(char *);
    vector<string> request_arg;//存放解析的结果
private:
    string request;
};

void Command::setCommand(string str){
    request = str;
}
//获取命令
string Command::getCommand() const{
    return request;
} 

//解析命令
void Command::split_command(){
    size_t pos;
    request+=delimiter;
    while((pos=request.find(delimiter))!=string::npos){
        string arg = request.substr(0,pos);
        request_arg.push_back(arg);
        request = request.substr(pos+1,request.size());
    }
}

//判断命令是否有效
bool Command::is_vaild_split(char *err){
    if(request.empty() || request.find(delimiter)==string::npos){
        char e[]="输入格式不对，请重新输入。具体格式如下:\nset key value\nget key\ndel key\ndump filename\nload filename\n";
        strcpy(err,e);
        return false;
    }
    split_command();
    if(all_order.count(request_arg[0])==0) {
        char e[]="(error)Unknow commend\n";
        strcpy(err,e);
        return false;
    }
    if(request_arg[0]=="set"&&request_arg.size()!=3){
        char e[]="(error)wrong number of arguments for 'set' command\n";
        strcpy(err,e);
        return false;
    }

    if(request_arg[0]=="get"&&request_arg.size()!=2){
        char e[]="(error)wrong number of arguments for 'get' command\n";
        strcpy(err,e);
        return false;
    }

    if(request_arg[0]=="del"&&request_arg.size()!=2){
        char e[]="(error)wrong number of arguments for 'del' command\n";
        strcpy(err,e);
        return false;
    }

    if(request_arg[0]=="dump"&&request_arg.size()!=2){
        char e[]="(error)wrong number of arguments for 'dump' command\n";
        strcpy(err,e);
        return false;
    }

    if(request_arg[0]=="load"&&request_arg.size()!=2){
        char e[]="(error)wrong number of arguments for 'load' command\n";
        strcpy(err,e);
        return false;
    }

    return true;
}
//执行命令
void Command::do_command(char *response){
    if(request_arg[0]=="set"){
        skiplist.insert_element(std::stoi(request_arg[1]),request_arg[2]);
    }else if(request_arg[0]=="get"){
        skiplist.search_element(std::stoi(request_arg[1]));
    }else if(request_arg[0]=="del"){
        skiplist.delete_element(std::stoi(request_arg[1]));
    }else if(request_arg[0]=="dump"){
        skiplist.dump_file(request_arg[1]);
    }else if(request_arg[0]=="load"){
        skiplist.load_file(request_arg[1]);
    }
    strcpy(response,res);
}
#endif