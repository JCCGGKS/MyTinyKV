/**
 * @file skiplist.h
 * @author cfq (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-06-29
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef SKIPLIST_H_
#define SKIPLIST_H_
#include<iostream>
#include<cstdlib>
#include<cmath>
#include<cstring>
#include<fstream>
#include<pthread.h>
#include "RWLock.h"

#define STORE_ROOT "./store/"
#define SKIP_LIST_P 0.5

//std::mutex mtx; //mutex for critical
//pthread_rwlock_t rw;
RWLock rw;
std::string delimiter = " ";
char res[1024];//服务器给出的response

//Class template to implement node
template<typename K,typename V>
class Node{
public:
    Node(){}
    Node(K k , V v,int);
    
    V get_value() const;

    void set_value(V);

    K get_key()const;

    ~Node();

    //Linear array to hold pointers to next node of different level
    //typedef typename Node<K,V>* mynode;
    //mynode *forward;
    Node<K,V> **forward;

    //节点所在层级
    int node_level;
private:
    K key;
    V value;
};

//带参构造函数
template<typename K,typename V>
Node<K,V>::Node(K k , V v,int level):key(k),value(v),node_level(level){
    //level+1 because array index is from 0 start
    forward = new Node<K,V> *[level+1];

    //fill forward array with 0
    memset(forward , 0,sizeof(Node<K,V>*)*(level+1));
}

//获取value
template<typename K,typename V>
V Node<K,V>::get_value() const{
    return value;
}

//设置value
template<typename K,typename V>
void Node<K,V>::set_value(V value){
    this->value = value;
}

//获取key
template<typename K,typename V>
K Node<K,V>::get_key() const{
    return key;
}

//析构函数
template<typename K,typename V>
Node<K,V>::~Node(){
    delete []forward;
}

//class template for skiplist
template<typename K,typename V>
class SkipList{
public:
    SkipList(){}
    SkipList(int);
    ~SkipList();
    int get_random_level();
    Node<K,V>*create_node(K,V,int);
    int insert_element(K,V);
    void display_list();
    bool search_element(K);
    void delete_element(K);
    void dump_file(std::string);
    void load_file(std::string);
    int size();

private:
    void get_key_value_from_string(const std::string&str,std::string&key,std::string&value);
    bool is_valid_string(const std::string&str);

private:
    //Maximum level of the skip list
    int _max_level;

    //cuurent level of skip list
    int _skip_list_level;

    //pointer to header node
    Node<K,V>*_header;

    //file operator
    std::ofstream _file_writer;
    std::ifstream _file_reader;

    //skiplist current element count;
    int _element_count;
};

//create new node
template<typename K,typename V>
Node<K,V>* SkipList<K,V>::create_node(const K k,const V v,int level){
    Node<K,V>* node = new Node<K,V>(k,v,level);
    return node;
}

//insert given key and value in skip list
//return 1 means element exists
//return 0 means del sucessfully
template<typename K,typename V>
int SkipList<K,V>::insert_element(const K key,const V value){
    rw.writeLock();
    Node<K,V>*current = this->_header;

    //create update array and initializer it
    //update is array which put node that the node->forward[i] should be operatored later
    Node<K,V> *update[_max_level+1];
    memset(update,0,sizeof(Node<K,V>*)*(_max_level+1));

    //start from highest level of skip list
    for(int i=_skip_list_level;i>=0;--i){
        //在同一层内寻找符合的node
        while(current->forward[i]&&current->forward[i]->get_key()<key){
            current = current->forward[i];
        }
        //找到之后更新update
        update[i]=current;
    }

    //reached level 0 and forward pointer to right node ,which is desired to insert key
    current = current->forward[0];

    //if current node have key equal to searched key we get it
    if(current && current->get_key()==key){
        std::cout<<"key: "<<key<<", exists"<<std::endl;
        strcpy(res,"key exists,modify value!\n");
        current->set_value(value);
        rw.writeUnlock();
        return 1;
    }

    //if current is NULL that meadns we have eached to end of the level
    //if current's key is not equal that means we have to insert node between update[0] and current node
    if(!current || current->get_key()!=key){
        //get randomlevel for node
        int random_level = get_random_level();

        //if random level is greater than skip list's current level,initializer update value with pointer to header
        if(random_level-1 > _skip_list_level){
            for(int i=_skip_list_level+1;i<random_level;++i){
                update[i]=_header;
            }
            _skip_list_level=random_level;
        }

        //create new node with random level 
        Node<K,V> *insert_node = create_node(key,value,random_level);

        //insert node
        for(int i=0;i<random_level;++i){
            //就好像在链表中插入一个元素一样
            insert_node->forward[i]=update[i]->forward[i];
            update[i]->forward[i]=insert_node;
        }

        std::cout<<"Successfully inserted key:"<<key<<", value:"<<value<<std::endl;
        strcpy(res,"Successfully inserted key!\n");
        _element_count++;
    }
    rw.writeUnlock();
    return 0;
}

//Display skip list
template<typename K,typename V>
void SkipList<K,V>::display_list(){

    std::cout<<"\n******Skip List******\n";
    for(int i=0;i<=_skip_list_level;++i){
        Node<K,V>* node=this->_header->forward[i];
        std::cout<<"Level"<<i<<":\n";
        while(node){
            std::cout<<node->get_key()<<" "<<node->get_value()<<"\n";
            node=node->forward[i];
        }
        std::cout<<"*********************";
        std::cout<<std::endl;
    }
}

//Dump data in memeory  to file
template<typename K,typename V>
void SkipList<K,V>::dump_file(std::string filename){
    std::cout<<"dump_file----------------"<<std::endl;
    _file_writer.open(STORE_ROOT+filename);
    Node<K,V>*node = this->_header->forward[0];
    while(node){
        _file_writer<<node->get_key()<<" "<<node->get_value()<<"\n";
        _file_writer.flush();
        std::cout<<node->get_key()<<" "<<node->get_value()<<";\n";
        node=node->forward[0];
    }
    _file_writer.flush();
    _file_writer.close();
    strcpy(res,"OK\n");
    return ;
}

//load data from disk
template<typename K,typename V>
void SkipList<K,V>::load_file(std::string filename){
    _file_reader.open(STORE_ROOT+filename);
    std::cout<<"load_file------------------"<<std::endl;
    std::string line;
    std::string key;
    std::string value;
    while(getline(_file_reader,line)){
        get_key_value_from_string(line,key,value);
        if(key.empty() || value.empty()) {
            continue;
        }

        insert_element(std::stoi(key),value);
        std::cout<<"key:"<<key<<" , value:"<<value<<std::endl;
    }
    strcpy(res,"OK\n");
    _file_reader.close();
}

//Get current SkipList size
template<typename K,typename V>
int SkipList<K,V>::size(){
    return _element_count;
}

//get key and value from string
template<typename K,typename V>
void SkipList<K,V>::get_key_value_from_string(const std::string&str,std::string&key,std::string&value){
    if(!is_valid_string(str)){
        return;
    }

    key = str.substr(0,str.find(delimiter));
    value = str.substr(str.find(delimiter)+1,str.size());   
}

//is valid string
template<typename K,typename V>
bool SkipList<K,V>::is_valid_string(const std::string&str){
    if(str.empty()) return false;
    //表示不存在delimiter
    if(str.find(delimiter)==std::string::npos) return false;
    return true;
}

//delete element from skip list
template<typename K,typename V>
void SkipList<K,V>::delete_element(K key){
    rw.writeLock();
    Node<K,V> *current  = this->_header;

    Node<K,V>* update[_max_level+1];
    memset(update,0,sizeof(Node<K,V>*)*(_max_level+1));

    for(int i=_skip_list_level;i>=0;--i){
        while(current->forward[i]&&current->forward[i]->get_key()<key){
            current = current->forward[i];
        }
        update[i]=current;
    }

    //make current point to right
    current = current->forward[0];

    //if current is NULL that meadns we have eached to end of the level
    //if current's key is not equal key that means we have not to delete node  
    if(!current || current->get_key()!=key) {
        std::cout<<"Not fount key!!!"<<std::endl;
        strcpy(res,"Not Found Key!\n");
        rw.writeUnlock();
        return ;
    }

    //delete node that we want
    for(int i=_skip_list_level;i>=0;--i){
        //if at level i , next node is not target node ,break the loop
        if(update[i]->forward[i]!=current) continue;
        update[i]->forward[i]=current->forward[i];
    }

    //delete memory
    delete current;

    //修改_skip_list_level
    while(_skip_list_level>0 && _header->forward[_skip_list_level]==0){
        --_skip_list_level;
    }

    std::cout<<"Successfully deleted key "<<key<<std::endl;
    strcpy(res,"Successfuly deleted key\n");

    --_element_count;

    rw.writeUnlock();
    return ;
}

//Search for element in skiplist
template<typename K,typename V>
bool SkipList<K,V>::search_element(K key){

    rw.readLock();
    std::cout<<"search_element-----------"<<std::endl;
    Node<K,V>* current = _header;

    //start from highest level of skip list
    for(int i=_skip_list_level;i>=0;--i){
        while(current->forward[i]&&current->forward[i]->get_key()<key){
            current=current->forward[i];
        }
    }

    //reached level 0 and advance pointer to right node,which we search
    current = current->forward[0];

    //if current node have key equal to searched key ,we get it
    if(current && current->get_key()==key){
        std::cout<<"Found key:"<<key<<" , value: "<<current->get_value()<<std::endl;
        char te[128];
        sprintf(te,"Found key : %d , value : %s\n",key,current->get_value().c_str());
        strcpy(res,te);
        rw.readUnlock();
        return true;
    }

    std::cout<<"Not Found Key:"<<key<<std::endl;
    strcpy(res,"Not Found!\n");
    rw.readUnlock();
    return false;
}

//construct skip list
template<typename K,typename V>
SkipList<K,V>::SkipList(int max_level):_max_level(max_level),_skip_list_level(0),_element_count(0){
    //create header node and initalizer key and value to null
    K k;
    V v;
    this->_header = new Node<K,V>(k,v,_max_level);
}

//destructor skip list
template<typename K,typename V>
SkipList<K,V>::~SkipList(){
    if(_file_writer.is_open()){
        _file_writer.close();
    }
    if(_file_reader.is_open()){
        _file_reader.close();
    }

    delete _header;
}
 
//get random level
template<typename K,typename V>
int SkipList<K,V>::get_random_level(){
    int level = 1;
    while(rand()%2 < 0.5 && level<_max_level){
        level+=1;
    }
    return level;
}

#endif