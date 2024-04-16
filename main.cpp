#include<iostream>
#include<cstdlib>
#include<ctime>
#include "skiplist.hpp"

constexpr auto DEBUG = false;


int main() {
	srand(time(nullptr));
	SkipList<int, std::string> skipList(6);
	//skipList.insert_element(1, "zhangsan");
	//skipList.insert_element(3, "lisi");
	//skipList.insert_element(7, "hajier");
	//skipList.insert_element(8, "wangwu");
	//skipList.insert_element(9, "chengdu");
	//skipList.display_list();
	//skipList.search_element(7);
	//
	//skipList.insert_element(19, "majiang");
	//skipList.insert_element(19, "rapper");
	//skipList.delete_element(9);
	//skipList.display_list();
	//skipList.dump_file();

	skipList.load_file();
	skipList.display_list();

	return 0;
}