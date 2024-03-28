#include "skiplist.h"


template<typename K, typename V>
Node<K, V>::Node() {
	std::count << "Node 默认构造函数" << std::endl;
}

template<typename K, typename V>
Node<K, V>::Node(K k, V v, int level) {
	this->key = k;
	this->val = v;
	this->node_level = level;
	this->forward = new Node<K, V>* [level + 1];
	memset(this->forward, 0, sizeof(Node<K, V>*) * (level + 1));
}