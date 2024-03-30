#include<iostream>
#include<cstdlib>
#include<ctime>
#include "skiplist.hpp"


int main() {
	srand(time(nullptr));
	SkipList<int, std::string> skipList(6);
	skipList.insert_element(1, "学");
	skipList.insert_element(3, "算法");
	skipList.insert_element(7, "认准");
	skipList.insert_element(8, "微信公众号：代码随想录");
	skipList.insert_element(9, "学习");
	skipList.insert_element(19, "算法不迷路");
	skipList.insert_element(19, "赶快关注吧你会发现详见很晚！");

	return 0;
}