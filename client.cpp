/**
 * @file client.cpp
 * @author cfq (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-07-02
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include<stdio.h>
#include<iostream>
#include<arpa/inet.h>
#include<unistd.h>
#include<cstring>
#include<string>
#include<stdlib.h>

int main(){

    //创建通信的套接字
    int fd = socket(AF_INET,SOCK_STREAM,0);
    if(fd==-1){
        perror("socket"); 
        exit(-1);
    }
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(9999);
    inet_pton(AF_INET,"192.168.248.128",&saddr.sin_addr.s_addr);
    //连接
    int ret = connect(fd,(struct sockaddr*)&saddr,sizeof(saddr));
    if(ret==-1){
        perror("connect");
        exit(-1);
    }

    //开始通信
    char recvbuf[1024]={0};
    char usescanf[1024]={0};
    while(1){
        memset(recvbuf,0,sizeof(recvbuf));
        memset(usescanf,0,sizeof(usescanf));
        //读取一行完整的数据，直到回车结束
        fgets(usescanf,sizeof(usescanf),stdin);
        char p[]="exit";
        if(!strncmp(usescanf,p,4)){
            break;
        }
        write(fd,usescanf,strlen(usescanf)+1);
        int len = read(fd,recvbuf,sizeof(recvbuf));
        if(len==-1){
            perror("read");
            exit(-1);
        }else if(len==0){
            printf("server closed....\n");
            break;
        }else if(len>0){
            printf("server response : %s\n",recvbuf);
        }
    }
    close(fd);
    return 0;
}