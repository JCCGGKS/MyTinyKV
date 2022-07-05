/**
 * @file server.cpp
 * @author cfq (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-07-02
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "command.h"
#include<cstdio>
#include<iostream>
#include<cstdlib>
#include<cstring>
#include<string>
#include<unistd.h>
#include<arpa/inet.h>
#include<pthread.h>

#define MAX_CLIENT_NUM 128
Command *comm;


//存输客户端的信息
struct sockinfo{
    int fd;//通信的文件描述符
    struct sockaddr_in addr;
    pthread_t tid;//线程号
}sockinfos[MAX_CLIENT_NUM];

void *working(void *arg){
    struct sockinfo* caddinfo = (struct sockinfo*)arg;
    
    //获取客户端信息
    char clienIP[16];
    inet_ntop(AF_INET,&caddinfo->addr.sin_addr.s_addr,clienIP,16);
    unsigned short clienPort = ntohs(caddinfo->addr.sin_port);
    printf("client ip is : %s , client port is :%d\n",clienIP,clienPort);

    //开始通信
    char recvbuf[1024];
    while(1){
        memset(recvbuf,0,sizeof(recvbuf));
        int len = read(caddinfo->fd,recvbuf,sizeof(recvbuf));
        if(len==-1){
            perror("read");
            exit(-1);
        }else if(len==0){
            printf("client closed....\n");
            break;
        }else if(len>0){
            comm->setCommand(std::string(recvbuf));
            comm->request_arg.clear();
            char err[1024];
            char response[1024];
            if(comm->is_vaild_split(err)){
                comm->do_command(response);
            }else{
                write(caddinfo->fd,err,strlen(err)+1);
            }
            write(caddinfo->fd,response,strlen(response)+1);
        }
    }
    close(caddinfo->fd);
    pthread_exit(NULL);
}

int main(){
    comm = new Command();
    //创建socket
    int lfd  = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(lfd==-1){
        perror("socket");
        exit(-1);
    }

    struct sockaddr_in sadd;
    sadd.sin_family = AF_INET;
    sadd.sin_port = htons(9999);
    sadd.sin_addr.s_addr = INADDR_ANY;

    //设置端口复用
    int optval = 1;
	int ret = setsockopt(lfd,SOL_SOCKET,SO_REUSEPORT,&optval,sizeof(optval));
    //绑定端口号和IP地址
    ret = bind(lfd,(struct sockaddr *)&sadd,sizeof(sadd));
    if(ret==-1){
        perror("bind");
        exit(-1);
    }

    //监听
    ret=listen(lfd,128);
    if(ret==-1){
        perror("listen");
        exit(-1);
    }
    //初始化客户端信息
    for(int i=0;i<MAX_CLIENT_NUM;++i){
        memset((void*)&sockinfos[i],0,sizeof(sockinfos[i]));
        sockinfos[i].fd=-1;
        sockinfos[i].tid=-1;
    }

    //循环等待
    while(1){
        struct sockaddr_in cadd;
        socklen_t len = sizeof(cadd);

        //取出连接
        int cfd = accept(lfd,(struct sockaddr *)&cadd,&len);
        //找出一个可用的位置存储客户端的信息
        int in;
        for(in=0;in<MAX_CLIENT_NUM;++in){
            if(sockinfos[in].fd==-1){
                sockinfos[in].fd=cfd;
                sockinfos[in].addr=cadd;
                break;
            }
            //如果从头到尾遍历一遍还没有找到合适的位置就先等一等
            if(in==MAX_CLIENT_NUM-1){
                sleep(1);
                --in;
            }
        }
        //创建子线程
        pthread_create(&sockinfos[in].tid,NULL,working,&sockinfos[in]);
       // pthread_detach(sockinfos[in].fd);
    }
    pthread_exit(NULL);
    close(lfd);
    delete comm;
    return 0;
}