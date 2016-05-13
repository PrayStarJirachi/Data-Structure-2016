#ifndef SJTU_PMAP_HPP
#define SJTU_PMAP_HPP

#include <functional>
#include <cstddef>
#include <memory>
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
	struct AVLNode;
	typedef std::shared_ptr<AVLNode> Pointer;
	
	template<class FT, class ST>
	struct mypair{
		FT first;
		ST second;
		
		mypair() {}
		mypair(const FT &a, const ST &b) : first(a), second(b) {}
	};
	struct AVLNode{
		size_t size, level;
		Pointer child[2];
		value_type data;
		
		AVLNode(const value_type &data) : data(data), size(1), level(0) {
			child[0] = child[1] = nullptr;
		}
		
		inline void update() {
			size = 1;
			if (child[0] != nullptr) size += child[0] -> size;
			if (child[1] != nullptr) size += child[1] -> size;
		}
	};
	
	
	Pointer root;
	size_t total;
	static Compare cmp;
	
	void split(const size_t &rank) {
		Pointer *p = &root;
		size_t now = rank;
		while (true) {
			*p = std::make_shared<AVLNode>(AVLNode(**p));
			size_t lSize = ((*p) -> child[0] == nullptr) ? 0 : ((*p) -> child[0] -> size);
			p = &((*p) -> child[now > lSize]);
			if (now == lSize + 1) break;
			if (now > lSize) {
				now -= lSize + 1;
			}
		}
	}
	static size_t getHeight(const Pointer &p) {
		return p == nullptr ? 0 : (p -> level);
	}
	static void maintain(Pointer &p) {
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
	Pointer getNode(const size_t &rank)const {
		if (rank < 1 || rank > total) {
			throw index_out_of_bound();
		}
		Pointer p = root;
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
	mypair<size_t, Pointer> getNode(const Key &key)const {
		Pointer p = root;
		size_t rank = 0;
		while (p != nullptr) {
			size_t lSize = ((p -> child[0] == nullptr) ? 0 : p -> child[0] -> size);
			if (!cmp((p -> data).first, key) && !cmp(key, (p -> data).first)) {
				return mypair<size_t, Pointer>(rank + lSize + 1, p);
				break;
			}
			if (cmp((p -> data).first, key)) {
				rank = rank + lSize + 1;
				p = p -> child[1];
			} else {
				p = p -> child[0];
			}
		}
		return mypair<size_t, Pointer>(total + 1, nullptr);
	}
	static mypair<size_t, bool> insert(Pointer &p, const value_type &value, const size_t &rank) {
		if (!cmp(value.first, (p -> data).first) && !cmp((p -> data).first, value.first)) {
			return mypair<size_t, bool>(rank + ((p -> child[0] == nullptr) ? 0 : p -> child[0] -> size) + 1, false);
		}
		p = std::make_shared<AVLNode>(AVLNode(*p));
		int c = cmp((p -> data).first, value.first);
		mypair<size_t, bool> ret;
		if (p -> child[c]) {
			if (c == 0) {
				ret = insert(p -> child[c], value, rank);
			} else {
				ret = insert(p -> child[c], value, rank + ((p -> child[0] == nullptr) ? 0 : p -> child[0] -> size) + 1);
			}
		} else {
			p -> child[c] = std::make_shared<AVLNode>(AVLNode(value));
			ret = mypair<size_t, bool>(rank + c * (((p -> child[0] == nullptr) ? 0 : p -> child[0] -> size) + 1) + 1, true);
		}
		if (ret.second) {
			maintain(p);
		}
		return ret;
	}
	static void remove(Pointer &now, const size_t &rank) {
		size_t lSize = (now -> child[0] == nullptr) ? 0 : now -> child[0] -> size;
		now = std::make_shared<AVLNode>(AVLNode(*now));
		if (rank == lSize + 1) {
			if (now -> child[0] && now -> child[1]) {
				int c = 1;
				Pointer f = now;
				now -> child[1] = std::make_shared<AVLNode>(AVLNode(*now -> child[1]));
				for (Pointer p = now -> child[1]; p -> child[0]; c = 0, f = p, p = p -> child[0]) {
					if (p -> child[0]) {
						p -> child[0] = std::make_shared<AVLNode>(*p -> child[0]);
					}
				}
				std::swap(now, f -> child[c]);
				std::swap(now -> child, f -> child[c] -> child);
				remove(now -> child[1], 1);
			} else {
				Pointer tmp = now;
				now = (now -> child[0]) ? now -> child[0] : now -> child[1];
				tmp = nullptr;
				return;
			}
		} else {
			if (rank <= lSize) {
				remove(now -> child[0], rank);
			} else {
				remove(now -> child[1], rank - lSize - 1);
			}
		}
		if (now -> child[1]) {
			now -> child[1] = std::make_shared<AVLNode>(AVLNode(*now -> child[1]));
		}
		maintain(now);
	}
	static void rotate(Pointer &x, int k) {
		x -> child[k] = std::make_shared<AVLNode>(AVLNode(*(x -> child[k])));
		Pointer y = x -> child[k];
		x -> child[k] = y -> child[k ^ 1];
		y -> child[k ^ 1] = x;
		x -> update();
		y -> update();
		x = y;
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
			address -> split(rank);
			Pointer tmp = address -> getNode(rank);
			return tmp -> data;
		}
		value_type * operator ->() noexcept {
			address -> split(rank);
			Pointer tmp = address -> getNode(rank);
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
			Pointer tmp = address -> getNode(rank);
			return tmp -> data;
		}
		const value_type * operator ->() noexcept {
			Pointer tmp = address -> getNode(rank);
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
	map(const map<Key, T, Compare> &other) : total(other.total) {
		if (other.root != nullptr) {
			root = std::make_shared<AVLNode>(AVLNode(*other.root));
		}
	}
	map<Key, T, Compare> & operator =(const map<Key, T, Compare> &other) {
		if (this == &other) {
			return *this;
		}
		clear();
		if (other.root != nullptr) {
			root = std::make_shared<AVLNode>(AVLNode(*other.root));
		}
		total = other.total;
		return *this;
	}
	~map() {
		clear();
	}
	T & at(const Key &key) {
		mypair<size_t, Pointer> ret = getNode(key);
		if (ret.second == nullptr) {
			throw index_out_of_bound();
		}
		return (*(ret.second)).data.second;;
	}
	const T & at(const Key &key) const {
		mypair<size_t, Pointer> ret = getNode(key);
		if (ret.second == nullptr) {
			throw index_out_of_bound();
		}
		return (*(ret.second)).data.second;;
	}
	T & operator[](const Key &key) {
		pair<iterator, bool> ret = insert(value_type(key, T()));
		return (*ret.first).second;
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
		root = nullptr;
		total = 0;
	}
	pair<iterator, bool> insert(const value_type &value) {
		if (root == nullptr) {
			root = std::make_shared<AVLNode>(AVLNode(value));
			total++;
			return pair<iterator, bool>(iterator(this, 1), true);
		}
		mypair<size_t, bool> ret = insert(root, value, 0);
		total += ret.second;
		return pair<iterator, bool>(iterator(this, ret.first), ret.second);
	}
	void erase(iterator pos) {
		if (pos.address != this || pos.rank == total + 1) {
			throw invalid_iterator();
		}
		remove(root, pos.rank);
		total--;
	}
	size_t count(const Key &key)const {
		return getNode(key).second != nullptr;
	}
	iterator find(const Key &key) {
		mypair<size_t, Pointer> ret = getNode(key);
		if (ret.second == nullptr) {
			return end();
		} else {
			return iterator(this, ret.first);
		}
	}
	const_iterator find(const Key &key)const {
		mypair<size_t, Pointer> ret = getNode(key);
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
