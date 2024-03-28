#pragma once
#include <iostream>


template<typename K, typename V>
class Node {
public:
	Node();
	Node(K k, V v, int);
	~Node();

	K get_key() const;
	V get_value() const;
	void set_value(V);

	Node<K, V>** forward;
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
	int insert_element();
	void display_list();
	bool search_element(K);
	void delete_element(K);
	void clear(Node<K, V>*);
	int size();

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