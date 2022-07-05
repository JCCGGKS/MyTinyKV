/**
 * @file stress_test.cpp
 * @author cfq (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-07-02
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include<iostream>
#include<chrono>
#include<cstdlib>
#include<pthread.h>
#include<time.h>
#include "../skiplist.h"

#define NUM_THREADS 8
#define READ_THREADS 6
#define WRITE_THREADS 2
#define TEST_COUNT 100000

SkipList<int,std::string>skiplist(18);

void *insertElement(void *arg){
    long tid =  pthread_self();
    std::cout<<"thread id : "<<tid<<std::endl;
    int tmp = TEST_COUNT / NUM_THREADS;
    for(int i=tid*tmp,count=0;count<tmp;++i){
        ++count;
        skiplist.insert_element(rand()%TEST_COUNT,"a");
    }
    pthread_exit(NULL);
}

void *getElement(void *arg){
    long tid =  pthread_self();
    std::cout<<"thread id : "<<tid<<std::endl;
    int tmp = TEST_COUNT / NUM_THREADS;
    for(int i=tid*tmp,count=0;count<tmp;++i){
        ++count;
        skiplist.search_element(rand()%TEST_COUNT);
    }
    pthread_exit(NULL);
}

int main(){
    srand(time(NULL));
    {
        //pthread_t rthreads[READ_THREADS];
        pthread_t wthreads[WRITE_THREADS];
        int rc;
        int i;
        auto start = std::chrono::high_resolution_clock::now();

        for(i=0;i<WRITE_THREADS;++i){
            std::cout<<"main thread creating write threads , "<<i<<std::endl;
            rc = pthread_create(&wthreads[i],NULL,insertElement,NULL);
            if(rc){
                perror("pthread_create");
                exit(-1);
            }
        }
        void *ret;
        for(int i=0;i<WRITE_THREADS;++i){
            if(pthread_join(wthreads[i],&ret)){
                perror("pthread_join");
                exit(-1);
            }
        }
        auto finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> ela = finish - start;
    }
    skiplist.display_list();
    {
        pthread_t rthreads[READ_THREADS];
        //pthread_t wthreads[WRITE_THREADS];
        int rc;
        int i;
        auto start = std::chrono::high_resolution_clock::now();

        for(i=0;i<READ_THREADS;++i){
            std::cout<<"main thread creating read threads , "<<i<<std::endl;
            rc = pthread_create(&rthreads[i],NULL,getElement,NULL);
            if(rc){
                perror("pthread_create");
                exit(-1);
            }
        }
        void *ret;
        for(int i=0;i<READ_THREADS;++i){
            if(pthread_join(rthreads[i],&ret)){
                perror("pthread_join");
                exit(-1);
            }
        }
        auto finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> ela = finish - start;
        std::cout<<"insert ela :"<<ela.count()<<std::endl;
        std::cout<<"get ela :"<<ela.count()<<std::endl;
    }
    pthread_exit(NULL);
    return 0;
}