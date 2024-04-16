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
std::string STORE_FILE = "dumpFile";
char delimiter = ':';

template<typename K, typename V>
class Node {
public:
	//Node();
	Node(K k, V v, int);
	~Node();

	K get_key() const;
	V get_value() const;
	void set_value(V);
	void print_node();

	Node<K, V>** forward;			// 指向数组指针的指针
	int node_level;
private:
	K key;
	V value;
};


template<typename K, typename V>
class SkipList {
public:
	SkipList(int);
	~SkipList();

	int get_random_level();
	Node<K, V>* create_node(K, V, int);
	int insert_element(K, V);
	void display_list();
	bool search_element(K);
	bool delete_element(K);
	void clear(Node<K, V>*);
//	int size();

	void dump_file();
	void load_file();

private:
	void get_key_value_from_string(const std::string& str, std::string* key, std::string* value);
	bool is_valid_string(const std::string& str);

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
	this->value = v;
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
	return this->value;
}

template<typename K, typename V>
K Node<K, V>::get_key() const {
	return this->key;
}

template<typename K, typename V>
void Node<K, V>::set_value(V val) {
	this->value = val;
}

template<typename K, typename V>
inline void Node<K, V>::print_node()
{
	std::cout << "key: " << this->key << "; value: " << this->value << std::endl;
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
void SkipList<K, V>::dump_file()
{
	_file_writer.open(STORE_FILE);
	Node<K, V>* node = _header->forward[0];
	while (node != nullptr) {
		_file_writer << node->get_key() << ":" << node->get_value() << ";\n";
		node = node->forward[0];
	}
	_file_writer.flush();
	_file_writer.close();
}

template<typename K, typename V>
void SkipList<K, V>::load_file()
{
	_file_reader.open(STORE_FILE);
	std::string line;
	std::string* key = new std::string();
	std::string* value = new std::string();

	while (getline(_file_reader, line)) {
		get_key_value_from_string(line, key, value);
		if (key->empty() || value->empty()) continue;

		insert_element(stoi(*key), *value);
		std::cout << "key:" << *key << "value:" << *value << std::endl;
	}
	delete key;
	delete value;
	_file_reader.close();
}

template <typename K, typename V>
bool SkipList<K, V>::is_valid_string(const std::string& str) {
	return !str.empty() && str.find(delimiter) != std::string::npos;
}

template<typename K, typename V>
void SkipList<K, V>::get_key_value_from_string(const std::string& str, std::string* key, std::string* value)
{
	if (!is_valid_string(str)) {
		return;
	}
	*key = str.substr(0, str.find(delimiter));
	*value = str.substr(str.find(delimiter) + 1, str.length());
}

template<typename K, typename V>
Node<K, V>* SkipList<K, V>::create_node(const K key, const V val, int level) {
	Node<K, V>* node = new Node<K, V>(key, val, level);
	return node;
}

template<typename K, typename V>
int SkipList<K, V>::insert_element(const K key, const V value) {
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
	// 移动到最底层的下一个节点，准备插入
	current = current->forward[0];

	// 键已存在
	if (current != NULL && current->get_key() == key) {
		std::cout << "key: " << key << ",exists" << std::endl;
		mtx.unlock();
		return 1;
	}

	// 插入操作
	if (current == NULL || current->get_key() != key) {
		int random_level = get_random_level();
		// 处理新节点的层级比当前层级更高的情况
		if (random_level > this->_skip_list_level) {
			for (int i = this->_skip_list_level + 1; i < random_level + 1; i++) {
				update[i] = _header;
			}
			this->_skip_list_level = random_level;
		}
		Node<K, V>* inserted_node = create_node(key, value, random_level);
		for (int i = 0; i < random_level; i++) {
			// 新插入的节点的下一个节点(类似链表操作)
			inserted_node->forward[i] = update[i]->forward[i];
			update[i]->forward[i] = inserted_node;
		}
		std::cout << "Successfully inserted { " << key << " : " << " \"" << value << "\"" << " }" << std::endl;
		this->_element_count++;
	}
	mtx.unlock();

	return 0;
}

template<typename K, typename V>
void SkipList<K, V>::display_list() {
	std::cout << "\n****************************SKip List********************************\n" << std::endl;
	for (int i = 0; i <= this->_skip_list_level; i++) {
		Node<K, V>* node = this->_header->forward[i];
		if (!node) break;
		std::cout << "Level " << i << ": ";
		while (node != NULL) {
			std::cout << node->get_key() << ":" << node->get_value() << ";";
			node = node->forward[i];
		}
		std::cout << std::endl;
	}
	std::cout << "*********************************************************************" << std::endl;
}

template<typename K, typename V>
bool SkipList<K, V>::search_element(K key)
{
	Node<K, V>* current = this->_header;
	for (int i = _skip_list_level-1; i >= 0; i--) {
		// 遍历当前的层级
		if (DEBUG) {
			if (current != _header) {
				std::cout << "第" << i << "层：";
				current->print_node();
			}
		}

		while (current->forward[i] && current->forward[i]->get_key() < key) {
			current = current->forward[i];
		}
	}
	// 此时已经查找到跳表的最底层，需要检查current节点的下一个节点是否为目标节点
	current = current->forward[0];
	if (current && current->get_key() == key) {
		std::cout << "Search Success!" << "Key: " << key << "; Value: " << current->get_value() << std::endl;
		return true;
	}
	std::cout << "Search Failed" << std::endl;
	return false;
}

template<typename K, typename V>
bool SkipList<K, V>::delete_element(const K key) {
	Node<K, V>* current = _header;
	Node<K, V>* update[_max_level + 1];
	memset(update, sizeof(Node<K, V>*) * (_max_level + 1), 0);

	for (int i = _skip_list_level; i >= 0; i--) {
		while (current->forward[i] && current->forward[i]->get_key() < key) {
			current = current->forward[i];
		}
		update[i] = current;
	}
	current = current->forward[0];

	if (current && current->get_key() == key) {
		for (int i = 0; i <= _skip_list_level; i++) {
			if (update[i]->forward[i] != current) break;
			update[i]->forward[i] = current->forward[i];
		}
		while (_skip_list_level > 0 && _header->forward[_skip_list_level] == NULL) {
			_skip_list_level--;
		}
		delete current;
		_element_count--;
		std::cout << "delete key:" << key << " element success!" << std::endl;
		return true;
	}

	return false;
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
