#include<iostream>
#include<cstdlib>
#include<ctime>
#include "skiplist.hpp"


int main() {
	srand(time(nullptr));
	SkipList<int, std::string> skipList(6);
	skipList.insert_element(1, "ѧ");
	skipList.insert_element(3, "�㷨");
	skipList.insert_element(7, "��׼");
	skipList.insert_element(8, "΢�Ź��ںţ���������¼");
	skipList.insert_element(9, "ѧϰ");
	skipList.insert_element(19, "�㷨����·");
	skipList.insert_element(19, "�Ͽ��ע����ᷢ���������");

	return 0;
}