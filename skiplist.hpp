#pragma once
#include <iostream> 
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <mutex>
#include <fstream>
#include <memory.h>
#include <cstdio>
#include <algorithm>


std::mutex mtx;

template<typename K, typename V>
class Node {
public:
	//Node();
	Node(K k, V v, int);
	~Node();

	K get_key() const;
	V get_value() const;
	void set_value(V);

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

	int get_random_level();
	Node<K, V>* create_node(K, V, int);
	int insert_element(K, V);
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


// *********************************
// 节点的类模板实现
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
V Node<K, V>::get_value() const{
	return this->val;
}

template<typename K, typename V>
K Node<K, V>::get_key() const {
	return this->key;
}

template<typename K, typename V>
void Node<K, V>::set_value(V val) {
	this->val = val;
}

// **************************
// SkipList的类模板实现
template<typename K, typename V>
SkipList<K, V>::SkipList(int maxLevel) {
	this->_max_level = maxLevel;
	this->_skip_list_level = 0;
	this->_element_count = 0;

	K k;
	V v;
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

template<typename K, typename V>
Node<K, V>* SkipList<K, V>::create_node(const K key, const V val, int level) {
	Node<K, V>* node = new Node<K, V>(key, val, level);
	return node;
}

template<typename K, typename V>
int SkipList<K, V>::insert_element(const K key, const V val) {
	mtx.lock();
	Node<K, V>* current = this->_header;
	Node<K, V>* update[this->_max_level + 1];

	memset(update, 0, sizeof(Node<K, V>*) * (this->_max_level + 1));

	// 从跳表的最高层级开始
	for (int i = this->_skip_list_level; i >= 0; i--) {	
		// 在每个层级顺序搜索i应该插入的坐标
		while (current->forward[i] != NULL && current->forward[i]->get_key() < key) {
			current = current->forward[i];
		}
		update[i] = current;
	}

	current = current->forward[0];

	// 键已存在
	if (current != NULL && current->get_key() == key) {
		std::cout << "key: " << key << ",exists" << std::endl;
		mtx.unlock();
		return 1;
	}

	// current是NULL，说明已经找到了最后的层级
	// 如果current的key不是要插入的key，就插入当前key
	if (current == NULL || current->get_key() != key) {
		int random_level = get_random_level();
		if (random_level > this->_skip_list_level) {
			for (int i = this->_skip_list_level + 1; i < random_level + 1; i++) {
				update[i] = _header;
			}
			this->_skip_list_level = random_level;
		}
		Node<K, V>* inserted_node = create_node(key, val, random_level);
		for (int i = 0; i < random_level; i++) {
			inserted_node->forward[i] = update[i]->forward[i];
			update[i]->forward[i] = inserted_node;
		}
		std::cout << "Successfully inserted { " << key << " : " << " \"" << val << "\"" << " }" << std::endl;
		this->_element_count++;
	}
	mtx.unlock();

	return 0;
}

template<typename K, typename V>
int SkipList<K, V>::get_random_level() {
	int k = 1;
	int r = rand();
	while (r%2) {
		k++;
		r = rand();
	}

	return std::min(k, this->_max_level);
}
