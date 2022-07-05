/**
 * @file RWLock.h
 * @author cfq (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-07-01
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef READWRITE_LOCK_H_
#define READWRITE_LOCK_H_
#include<mutex>
//用两个互斥锁实现的读写锁
class RWLock{
public:
    RWLock():read_cnt(0){}
    ~RWLock(){}
    void readLock(){
        read_mutex.lock();
        if(++read_cnt==1){
            //第一个读者到达
            write_mutex.lock();
        }
        read_mutex.unlock();
    }
    void readUnlock(){
        read_mutex.lock();
        if(--read_cnt==0){
            write_mutex.unlock();
        }
        read_mutex.unlock();
    }
    void writeLock(){
        write_mutex.lock();
    }
    void writeUnlock(){
        write_mutex.unlock();
    }
private:
    std::mutex read_mutex;
    std::mutex write_mutex;
    int read_cnt;
};
#endif