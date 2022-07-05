/**
 * @file main.cpp
 * @author cfq (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-07-01
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include<iostream>
#include "skiplist.h"

int main(){

    SkipList<int,std::string>skiplist(6);

    skiplist.insert_element(1,"1907381101");
    skiplist.insert_element(3,"19007381102");
    skiplist.insert_element(2,"19007381102");
    skiplist.insert_element(5,"19007381102");
    skiplist.insert_element(7,"19007381102");
    skiplist.insert_element(8,"19007381102");
    skiplist.insert_element(11,"19007381102");
    skiplist.insert_element(10,"19007381102");
    skiplist.insert_element(4,"19007381102");
    skiplist.insert_element(12,"19007381102");

    std::cout<<"skiplist size : "<<skiplist.size()<<std::endl;

    skiplist.display_list();

    skiplist.dump_file("dumpFile");

    skiplist.search_element(24);
    skiplist.search_element(1);
    skiplist.search_element(11);

    skiplist.delete_element(3);
    skiplist.delete_element(4);

    std::cout<<"skiplist size : "<<skiplist.size()<<std::endl;

    return 0;
}