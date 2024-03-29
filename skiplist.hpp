#pragma once
#include <iostream> 
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <mutex>
#include <fstream>
#include <memory.h>
#include <cstdio>


template<typename K, typename V>
class Node {
public:
	//Node();
	Node(K k, V v, int);
	~Node();

	//K get_key() const;
	//V get_value() const;
	//void set_value(V);

	Node<K, V>** forward;			// 指向Node指针的指针
	int node_level;
private:
	K key;
	V val;
};


template<typename K, typename V>
class SkipList {
public:
	SkipList(int);
	~SkipList();

//	int get_random_level();
//	Node<K, V>* create_node(K, V, int);
//	int insert_element();
//	void display_list();
//	bool search_element(K);
//	void delete_element(K);
	void clear(Node<K, V>*);
//	int size();
//
//	void dump_file();
//	void load_file();
//
//private:
//	void get_key_value_from_string(const std::string& str, std::string* key, std::string* value);
//	bool is_valid_string(const std::string& str);

private:
	int _max_level;						// 最大层级
	int _skip_list_level;				// 当前层级
	Node<K, V>* _header;
	std::ofstream _file_writer;
	std::ifstream _file_reader;			
	int _element_count;					// 跳表元素计数器
};



template<typename K, typename V>
Node<K, V>::Node(const K k, const V v, int level) {
	this->key = k;
	this->val = v;
	this->node_level = level;
	this->forward = new Node<K, V>* [level + 1];
	memset(this->forward, 0, sizeof(Node<K, V>*) * (level + 1));
}

template<typename K, typename V>
Node<K, V>::~Node() {
	delete []forward;
}

template<typename K, typename V>
SkipList<K, V>::SkipList(int maxLevel) {
	this->_max_level = maxLevel;
	this->_skip_list_level = 0;
	this->_element_count = 0;

	K k = 0;
	V v;									// 局部变量必须初始化，std::string有默认初始化
	this->_header = new Node<K, V>(k, v, _max_level);
}

template<typename K, typename V>
SkipList<K, V>::~SkipList() {
	if (this->_file_writer.is_open())
		_file_writer.close();
	if (_file_reader.is_open())
		_file_reader.close();

	if (_header->forward[0] != nullptr) {
		clear(_header->forward[0]);
	}
	delete(_header);
}

template<typename K, typename V>
void SkipList<K, V>::clear(Node<K, V>* cur) {
	if (cur->forward[0] != nullptr) {
		clear(cur->forward[0]);
	}
	delete(cur);
}