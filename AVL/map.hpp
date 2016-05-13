#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

template<
	class Key,
	class T,
	class Compare = std::less<Key>
>
class map{
public:
	typedef pair<const Key, T> value_type;
	
private:
	template<class FT, class ST>
	struct mypair{
		FT first;
		ST second;
		
		mypair() {}
		mypair(const FT &a, const ST &b) : first(a), second(b) {}
	};
	struct AVLNode{
		size_t size, level;
		AVLNode *child[2];
		value_type data;
		
		AVLNode(const value_type &data) : data(data), size(1), level(1) {
			child[0] = child[1] = nullptr;
		}
		
		void update() {
			size = 1;
			level = 1;
			if (child[0] != nullptr) {
				size += child[0] -> size;
				if (level < child[0] -> level + 1) {
					level = child[0] -> level + 1;
				}
			}
			if (child[1] != nullptr) {
				size += child[1] -> size;
				if (level < child[1] -> level + 1) {
					level = child[1] -> level + 1;
				}
			}
		}
	};
	
	AVLNode *root;
	size_t total;
	static Compare cmp;
	
	AVLNode * getNode(const size_t &rank)const {
		if (rank < 1 || rank > total) {
			throw index_out_of_bound();
		}
		AVLNode *p = root;
		size_t now = rank;
		while (true) {
			size_t lSize = (p -> child[0] == nullptr) ? 0 : (p -> child[0] -> size);
			if (now == lSize + 1) break;
			p = p -> child[now > lSize];
			if (now > lSize) {
				now -= lSize + 1;
			}
		}
		return p;
	}
	pair<size_t, AVLNode *> getNode(const Key &key)const {
		AVLNode *p = root;
		size_t rank = 0;
		while (p != nullptr) {
			size_t lSize = ((p -> child[0] == nullptr) ? 0 : p -> child[0] -> size);
			if (!cmp((p -> data).first, key) && !cmp(key, (p -> data).first)) {
				return pair<size_t, AVLNode *>(rank + lSize + 1, p);
				break;
			}
			if (cmp((p -> data).first, key)) {
				rank = rank + lSize + 1;
				p = p -> child[1];
			} else {
				p = p -> child[0];
			}
		}
		return pair<size_t, AVLNode *>(total + 1, nullptr);
	}
	static size_t getHeight(const AVLNode *p) {
		return p == nullptr ? 0 : (p -> level);
	}
	static void maintain(AVLNode *&p) {
		size_t lHeight = getHeight(p -> child[0]);
		size_t rHeight = getHeight(p -> child[1]);
		if (lHeight > rHeight + 1) {
			if (getHeight(p -> child[0] -> child[0]) >= getHeight(p -> child[0] -> child[1])) {
				rotate(p, 0);
			} else {
				rotate(p -> child[0], 1);
				rotate(p, 0);
			}
		} else if (rHeight > lHeight + 1) {
			if (getHeight(p -> child[1] -> child[1]) >= getHeight(p -> child[1] -> child[0])) {
				rotate(p, 1);
			} else {
				rotate(p -> child[1], 0);
				rotate(p, 1);
			}
		}
		p -> update();
	}
	static mypair<size_t, bool> insert(AVLNode *&p, const value_type &value, const size_t &rank) {
		if (!cmp(value.first, (p -> data).first) && !cmp((p -> data).first, value.first)) {
			return mypair<size_t, bool>(rank + ((p -> child[0] == nullptr) ? 0 : p -> child[0] -> size) + 1, false);
		}
		int c = cmp((p -> data).first, value.first);
		mypair<size_t, bool> ret;
		if (p -> child[c]) {
			if (c == 0) {
				ret = insert(p -> child[c], value, rank);
			} else {
				ret = insert(p -> child[c], value, rank + ((p -> child[0] == nullptr) ? 0 : p -> child[0] -> size) + 1);
			}
		} else {
			p -> child[c] = new AVLNode(value);
			ret = mypair<size_t, bool>(rank + c * (((p -> child[0] == nullptr) ? 0 : p -> child[0] -> size) + 1) + 1, true);
		}
		maintain(p);
		return ret;
	}
	static void remove(AVLNode *&now, const size_t &rank) {
		size_t lSize = (now -> child[0] == nullptr) ? 0 : now -> child[0] -> size;
		if (rank == lSize + 1) {
			if (now -> child[0] && now -> child[1]) {
				int c = 1;
				AVLNode *f = now;
				for (AVLNode *p = now -> child[1]; p -> child[0]; c = 0, f = p, p = p -> child[0]);
				std::swap(now, f -> child[c]);
				std::swap(now -> child, f -> child[c] -> child);
				remove(now -> child[1], 1);
			} else {
				AVLNode *tmp = now;
				now = (now -> child[0]) ? now -> child[0] : now -> child[1];
				delete tmp;
				return;
			}
		} else {
			if (rank <= lSize) {
				remove(now -> child[0], rank);
			} else {
				remove(now -> child[1], rank - lSize - 1);
			}
		}
		maintain(now);
	}
	static void rotate(AVLNode *&x, int k) {
		AVLNode *y = x -> child[k];
		x -> child[k] = y -> child[k ^ 1];
		y -> child[k ^ 1] = x;
		x -> update();
		y -> update();
		x = y;
	}
	static void lRotate(AVLNode *&x) {
		if (x -> child[0] != nullptr && x -> child[0] -> level == x -> level) {
			rotate(x, 0);
		}
	}
	static void rRotate(AVLNode *&x) {
		if (x -> child[1] != nullptr && x -> child[1] -> child[1] != nullptr && 
		    x -> level <= x -> child[1] -> child[1] -> level) {
			rotate(x, 1);
			x -> level++;
		}
	}
	static void clear(AVLNode *&now) {
		if (now -> child[0]) clear(now -> child[0]);
		if (now -> child[1]) clear(now -> child[1]);
		delete now;
		now = nullptr;
	}
	static AVLNode * copy(const AVLNode * const p) {
		if (p == nullptr) return nullptr;
		AVLNode *ret = new AVLNode(*p);
		if (p -> child[0]) ret -> child[0] = copy(p -> child[0]);
		if (p -> child[1]) ret -> child[1] = copy(p -> child[1]);
		return ret;
	}
	
public:
	class const_iterator;
	class iterator{
	friend class map<Key, T, Compare>;
	private:
		map<Key, T, Compare> *address;
		size_t rank;
		
		iterator(map<Key, T, Compare> * const a, const size_t &r) : address(a), rank(r) {}
		
	public:
		iterator() : address(nullptr), rank(0) {}
		iterator operator +(const size_t &rhs)const {
			if (rank + rhs > address -> total + 1) {
				throw invalid_iterator();
			}
			return iterator(address, rank + rhs);
		}
		iterator operator -(const size_t &rhs)const {
			if (rank - rhs < 1) {
				throw invalid_iterator();
			}
			return iterator(address, rank - rhs);
		}
		iterator operator ++(int) {
			if (rank > address -> total) {
				throw invalid_iterator();
			}
			iterator tmp = *this;
			rank++;
			return tmp;
		}
		iterator & operator ++() {
			if (rank > address -> total) {
				throw invalid_iterator();
			}
			rank++;
			return *this;
		}
		iterator operator --(int) {
			if (rank - 1 < 1) {
				throw invalid_iterator();
			}
			iterator tmp = *this;
			rank--;
			return tmp;
		}
		iterator & operator --() {
			if (rank - 1 < 1) {
				throw invalid_iterator();
			}
			rank--;
			return *this;
		}
		value_type & operator *()const {
			AVLNode *tmp = address -> getNode(rank);
			return tmp -> data;
		}
		value_type * operator ->() noexcept {
			AVLNode *tmp = address -> getNode(rank);
			return &(tmp -> data);
		}
		bool operator ==(const iterator &rhs)const {
			return address == rhs.address && rank == rhs.rank;
		}
		bool operator ==(const const_iterator &rhs)const {
			return address == rhs.address && rank == rhs.rank;
		}
		bool operator !=(const iterator &rhs)const {
			return address != rhs.address || rank != rhs.rank;
		}
		bool operator !=(const const_iterator &rhs)const {
			return address != rhs.address || rank != rhs.rank;
		}
	};
	class const_iterator{
	friend class map<Key, T, Compare>;
	private:
		const map<Key, T, Compare> * address;
		size_t rank;
		
		const_iterator(const map<Key, T, Compare> * const a, const size_t &r) : address(a), rank(r) {}
		
	public:
		const_iterator() : address(nullptr), rank(0) {}
		const_iterator(const iterator &other) : address(other.address), rank(other.rank){}
		const_iterator operator +(const size_t &rhs)const {
			if (rank + rhs > address -> total + 1) {
				throw invalid_iterator();
			}
			return iterator(address, rank + rhs);
		}
		const_iterator operator -(const size_t &rhs)const {
			if (rank - rhs < 1) {
				throw invalid_iterator();
			}
			return iterator(address, rank - rhs);
		}
		const_iterator operator ++(int) {
			if (rank > address -> total) {
				throw invalid_iterator();
			}
			const_iterator tmp = *this;
			rank++;
			return tmp;
		}
		const_iterator & operator ++() {
			if (rank > address -> total) {
				throw invalid_iterator();
			}
			rank++;
			return *this;
		}
		const_iterator operator --(int) {
			if (rank - 1 < 1) {
				throw invalid_iterator();
			}
			const_iterator tmp = *this;
			rank--;
			return tmp;
		}
		const_iterator & operator --() {
			if (rank - 1 < 1) {
				throw invalid_iterator();
			}
			rank--;
			return *this;
		}
		const value_type & operator *()const {
			AVLNode *tmp = address -> getNode(rank);
			return tmp -> data;
		}
		const value_type * operator ->() noexcept {
			AVLNode *tmp = address -> getNode(rank);
			return &(tmp -> data);
		}
		bool operator ==(const iterator &rhs)const {
			return address == rhs.address && rank == rhs.rank;
		}
		bool operator ==(const const_iterator &rhs)const {
			return address == rhs.address && rank == rhs.rank;
		}
		bool operator !=(const iterator &rhs)const {
			return address != rhs.address || rank != rhs.rank;
		}
		bool operator !=(const const_iterator &rhs)const {
			return address != rhs.address || rank != rhs.rank;
		}
	};
	map() : root(nullptr), total(0) {}
	map(const map<Key, T, Compare> &other) : root(copy(other.root)), total(other.total) {}
	map<Key, T, Compare> & operator =(const map<Key, T, Compare> &other) {
		if (this == &other) {
			return *this;
		}
		clear();
		root = copy(other.root);
		total = other.total;
		return *this;
	}
	~map() {
		clear();
	}
	T & at(const Key &key) {
		pair<size_t, AVLNode *> ret = getNode(key);
		if (ret.second == nullptr) {
			throw index_out_of_bound();
		}
		return (*(ret.second)).data.second;;
	}
	const T & at(const Key &key) const {
		pair<size_t, AVLNode *> ret = getNode(key);
		if (ret.second == nullptr) {
			throw index_out_of_bound();
		}
		return (*(ret.second)).data.second;;
	}
	T & operator[](const Key &key) {
		pair<iterator, bool> ret = insert(value_type(key, T()));
		return (*ret.first).second;
	}
	const T & operator [](const Key &key)const {
		return at(key);
	}
	iterator begin() {
		return iterator(this, 1);
	}
	const_iterator cbegin()const {
		return const_iterator(this, 1);
	}
	iterator end() {
		return iterator(this, total + 1);
	}
	const_iterator cend()const {
		return const_iterator(this, total + 1);
	}
	bool empty() const {
		return total == 0;
	}
	const size_t & size() const {
		return total;
	}
	void clear() {
		if (root != nullptr) {
			clear(root);
		}
		total = 0;
	}
	pair<iterator, bool> insert(const value_type &value) {
		if (root == nullptr) {
			root = new AVLNode(value);
			total++;
			return pair<iterator, bool>(iterator(this, 1), true);
		}
		mypair<size_t, bool> ret = insert(root, value, 0);
		total += ret.second;
		return pair<iterator, bool>(iterator(this, ret.first), ret.second);
	}
	void erase(iterator pos) {
		if (pos.rank == total + 1 || pos.address != this) {
			throw invalid_iterator();
		}
		remove(root, pos.rank);
		total--;
	}
	size_t count(const Key &key)const {
		return getNode(key).second != nullptr;
	}
	iterator find(const Key &key) {
		pair<size_t, AVLNode *> ret = getNode(key);
		if (ret.second == nullptr) {
			return end();
		} else {
			return iterator(this, ret.first);
		}
	}
	const_iterator find(const Key &key)const {
		pair<size_t, AVLNode *> ret = getNode(key);
		if (ret.second == nullptr) {
			return cend();
		} else {
			return const_iterator(this, ret.first);
		}
	}
};

template<
	class Key,
	class T,
	class Compare
>
Compare map<Key, T, Compare>::cmp;

}
#endif

