#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <bits/stdc++.h>
#include <cstddef>

namespace sjtu {

template<class T>
class deque {
public:
	std::list<std::list<T>> list;
	int ss;
	
	int __partSize() const {
		return (int)std::sqrt((double)ss + 10) + 3;
	}
	
	std::pair<typename std::list<std::list<T>>::iterator, typename std::list<T>::iterator> __getListIterator(int rank) {
		if (rank < 0 || rank > ss) {
			throw index_out_of_bound();
		}
		rank ++;
		int t = 0;
		for (typename std::list<std::list<T>>::iterator it = list.begin(); it != list.end(); it ++) {
			if (t + (int)it->size() >= rank) {
				typename std::list<T>::iterator it1 = it->begin();
				for (int i = 1; i <= rank - t - 1; i ++) {
					it1 ++;
				}
				return std::make_pair(it, it1);
			} else {
				t += it->size();
			}
		}
		typename std::list<std::list<T>>::iterator it = list.end();
		it --;
		return std::make_pair(it, list.back().end());
	}
	
	std::pair<typename std::list<std::list<T>>::const_iterator, typename std::list<T>::const_iterator> __getListConstIterator(int rank) const {
		if (rank < 0 || rank > ss) {
			throw index_out_of_bound();
		}
		rank ++;
		int t = 0;
		for (typename std::list<std::list<T>>::const_iterator it = list.cbegin(); it != list.cend(); it ++) {
			if (t + (int)it->size() >= rank) {
				typename std::list<T>::const_iterator it1 = it->cbegin();
				for (int i = 1; i <= rank - t - 1; i ++) {
					it1 ++;
				}
				return std::make_pair(it, it1);
			} else {
				t += it->size();
			}
		}
		typename std::list<std::list<T>>::const_iterator it = list.end();
		it --;
		return std::make_pair(it, list.back().cend());
	}

	void __split(typename std::list<std::list<T>>::iterator it) {
		std::list<T> tmp;
		for (int i = 1; i < __partSize(); i ++) {
			tmp.push_back(it->front());
			it->pop_front();
		}
		list.insert(it, tmp);
	}
	
	void __reBuild() {
		std::list<T> all;
		for (typename std::list<std::list<T>>::iterator it = list.begin(); it != list.end(); it ++) {
			all.insert(all.end(), it->begin(), it->end());
		}
		list.clear();
		for (typename std::list<T>::iterator it = all.begin(); it != all.end(); it ++) {
			if (list.empty() || (int)list.back().size() == 2 * __partSize()) {
				list.push_back(std::list<T>(1, *it));
			} else {
				list.back().push_back(*it);
			}
		}
	}
	
	void __maintain() {
		if (ss == 0) {
			list.clear();
			list.push_back(std::list<T>());
		} else if ((int)list.size() > 5 * __partSize() || (int)list.size() < __partSize() / 10) {
			__reBuild();
		}
	}
	
	class iterator;
	class const_iterator;
	
	friend class iterator;
	friend class const_iterator;
	
	class iterator {
	public:
		deque *belong;
		std::pair<typename std::list<std::list<T>>::iterator, typename std::list<T>::iterator> pos;
		int rank;
		/**
		 * TODO add data members
		 *   just add whatever you want.
		 */
	public:
		iterator() {}
		iterator(deque *belong, std::pair<typename std::list<std::list<T>>::iterator, typename std::list<T>::iterator> pos, int rank):belong(belong), pos(pos), rank(rank) {}
		/**
		 * return a new iterator which pointer n-next elements
		 *   even if there are not enough elements, the behaviour is **undefined**.
		 * as well as operator-
		 */
		iterator operator+(const int &n) const {
			//TODO
			return iterator(belong, belong->__getListIterator(rank + n), rank + n);
		}
		iterator operator-(const int &n) const {
			//TODO
			return iterator(belong, belong->__getListIterator(rank - n), rank - n);
		}
		// return th distance between two iterator,
		// if these two iterators points to different vectors, throw invaild_iterator.
		int operator-(const iterator &rhs) const {
			//TODO
			return rank - rhs.rank;
		}
		iterator operator+=(const int &n) {
			//TODO
			rank += n;
			pos = belong->__getListIterator(rank);
			return *this;
		}
		iterator operator-=(const int &n) {
			//TODO
			rank -= n;
			pos = belong->__getListIterator(rank);
			return *this;
		}
		/**
		 * TODO iter++
		 */
		iterator operator++(int) {
			iterator tmp = *this;
			rank ++;
			if (rank > belong->ss) {
				throw index_out_of_bound();
			} else if (rank == belong->ss) {
				pos.first = belong->list.end();
				pos.first --;
				pos.second = pos.first->end();
				return tmp;	
			}
			pos.second ++;
			if (pos.second == pos.first->end()) {
				pos.first ++;
				pos.second = pos.first->begin();
			}
			return tmp;
		}
		/**
		 * TODO ++iter
		 */
		iterator& operator++() {
			rank ++;
			if (rank > belong->ss) {
				throw index_out_of_bound();
			} else if (rank == belong->ss) {
				pos.first = belong->list.end();
				pos.first --;
				pos.second = pos.first->end();
				return *this;
			}
			pos.second ++;
			if (pos.second == pos.first->end()) {
				pos.first ++;
				pos.second = pos.first->begin();
			}
			return *this;
		}
		/**
		 * TODO iter--
		 */
		iterator operator--(int) {
			
			
			iterator tmp = *this;
			rank --;
			if (rank < 0) {
				throw index_out_of_bound();
			}
			if (pos.second == pos.first->begin()) {
				pos.first --;
				pos.second = pos.first->end();
				pos.second --;
			} else {
				pos.second --;
			}
			return tmp;
		}
		/**
		 * TODO --iter
		 */
		iterator& operator--() {
			rank --;
			if (rank < 0) {
				throw index_out_of_bound();
			}
			if (pos.second == pos.first->begin()) {
				pos.first --;
				pos.second = pos.first->end();
				pos.second --;
			} else {
				pos.second --;
			}
				
			return *this;
		}
		/**
		 * TODO *it
		 */
		T& operator*() const {
			return *pos.second;
		}
		/**
		 * TODO it->field
		 */
		T* operator->() const noexcept {
			return &(*pos.second);
		}
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory).
		 */
		bool operator==(const iterator &rhs) const {
			return belong == rhs.belong && pos == rhs.pos && rank == rhs.rank;
		}
		bool operator==(const const_iterator &rhs) const {
			return belong == rhs.belong && pos == rhs.pos && rank == rhs.rank;
		}
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const {
			return !(*this == rhs);
		}
		bool operator!=(const const_iterator &rhs) const {
			return !(*this == rhs);
		}
	};
	
	class const_iterator {
	public:
		const deque* belong;
		std::pair<typename std::list<std::list<T>>::const_iterator, typename std::list<T>::const_iterator> pos;
		int rank;
		/**
		 * TODO add data members
		 *   just add whatever you want.
		 */
	public:
		const_iterator() {}
		const_iterator(const deque *belong, std::pair<typename std::list<std::list<T>>::const_iterator, typename std::list<T>::const_iterator> pos, int rank):belong(belong), pos(pos), rank(rank) {}
		/**
		 * return a new iterator which pointer n-next elements
		 *   even if there are not enough elements, the behaviour is **undefined**.
		 * as well as operator-
		 */
		const_iterator operator+(const int &n) const {
			//TODO
			return const_iterator(belong, belong->__getListConstIterator(rank + n), rank + n);
		}
		const_iterator operator-(const int &n) const {
			//TODO
			return const_iterator(belong, belong->__getListConstIterator(rank - n), rank - n);
		}
		// return th distance between two iterator,
		// if these two iterators points to different vectors, throw invaild_iterator.
		int operator-(const const_iterator &rhs) const {
			//TODO
			return rank - rhs.rank;
		}
		const_iterator operator+=(const int &n) {
			//TODO
			rank += n;
			pos = belong->__getListConstIterator(rank);
			return *this;
		}
		const_iterator operator-=(const int &n) {
			//TODO
			rank -= n;
			pos = belong->__getListConstIterator(rank);
			return *this;
		}
		/**
		 * TODO iter++
		 */
		const_iterator operator++(int) {
			const_iterator tmp = *this;
			rank ++;
			if (rank > belong->ss) {
				throw index_out_of_bound();
			} else if (rank == belong->ss) {
				pos.first = belong->list.cend();
				pos.first --;
				pos.second = pos.first->cend();
				return tmp;	
			}
			pos.second ++;
			if (pos.second == pos.first->cend()) {
				pos.first ++;
				pos.second = pos.first->cbegin();
			}
			return tmp;
		}
		/**
		 * TODO ++iter
		 */
		const_iterator& operator++() {
			rank ++;
			if (rank > belong->ss) {
				throw index_out_of_bound();
			} else if (rank == belong->ss) {
				pos.first = belong->list.cend();
				pos.first --;
				pos.second = pos.first->cend();
				return *this;
			}
			pos.second ++;
			if (pos.second == pos.first->cend()) {
				pos.first ++;
				pos.second = pos.first->cbegin();
			}
			return *this;
		}
		/**
		 * TODO iter--
		 */
		const_iterator operator--(int) {
			const_iterator tmp = *this;
			rank --;
			if (rank < 0) {
				throw index_out_of_bound();
			}
			if (pos.second == pos.first->cbegin()) {
				pos.first --;
				pos.second = pos.first->cend();
				pos.second --;
			} else {
				pos.second --;
			}
			return tmp;
		}
		/**
		 * TODO --iter
		 */
		const_iterator& operator--() {
			rank --;
			if (rank < 0) {
				throw index_out_of_bound();
			}
			if (pos.second == pos.first->cbegin()) {
				pos.first --;
				pos.second = pos.first->cend();
				pos.second --;
			} else {
				pos.second --;
			}
			return *this;
		}
		/**
		 * TODO *it
		 */
		const T& operator*() const {
			return *pos.second;
		}
		/**
		 * TODO it->field
		 */
		const T* operator->() const noexcept {
			return &(*pos.second);
		}
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory).
		 */
		bool operator==(const const_iterator &rhs) const {
			return belong == rhs.belong && pos == rhs.pos && rank == rhs.rank;
		}
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const const_iterator &rhs) const {
			return !(*this == rhs);
		}
	};

	
	/**
	 * TODO Constructors
	 */
	deque() {
		list.clear();
		list.push_back(std::list<T>());
		ss = 0;
	}
	deque(const deque &other) {
		if (this == &other) {
			return ;
		}
		list = other.list;
		ss = other.ss;
	}
	/**
	 * TODO Deconstructor
	 */
	~deque() {}
	/**
	 * TODO assignment operator
	 */
	deque &operator=(const deque &other) {
		if (this == &other) {
			return *this;
		}
		list = other.list;
		ss = other.ss;
		return *this; 
	}
	/**
	 * access specified element with bounds checking
	 * throw index_out_of_bound if out of bound.
	 */
	T & at(const int &pos) {
		if (pos == ss) {
			throw index_out_of_bound();
		}
		return *__getListIterator(pos).second;
	}
	const T & at(const int &pos) const {
		if (pos == ss) {
			throw index_out_of_bound();
		}
		return *__getListConstIterator(pos).second;
	}
	T & operator[](const int &pos) {
		return at(pos);
	}
	const T & operator[](const int &pos) const {
		return at(pos);
	}
	/**
	 * access the first element
	 * throw container_is_empty when the container is empty.
	 */
	const T & front() const {
		if (empty()) {
			throw container_is_empty();
		}
		return list.front().front();
	}
	/**
	 * access the last element
	 * throw container_is_empty when the container is empty.
	 */
	const T & back() const {
		if (empty()) {
			throw container_is_empty();
		}
		return list.back().back();
	}
	/**
	 * returns an iterator to the beginning.
	 */
	iterator begin() {
		return iterator(this, __getListIterator(0), 0);
	}
	const_iterator cbegin() const {
		return const_iterator(this, __getListConstIterator(0), 0);
	}
	/**
	 * returns an iterator to the end.
	 */
	iterator end() {
		return iterator(this, std::make_pair(--list.end(), list.back().end()), ss);
	}
	const_iterator cend() const {
		return const_iterator(this, std::make_pair(--list.end(), list.back().cend()), ss);
	}
	/**
	 * checks whether the container is empty.
	 */
	bool empty() const {
		return ss == 0;
	}
	/**
	 * returns the number of elements
	 */
	size_t size() const {
		return (size_t)ss;
	}
	/**
	 * clears the contents
	 */
	void clear() {
		list.clear();
		list.push_back(std::list<T>());
		ss = 0;
	}
	/**
	 * inserts elements at the specified locat on in the container.
	 * inserts value before pos
	 * returns an iterator pointing to the inserted value
	 */
	iterator insert(iterator pos, const T &value) {
		ss ++;
		pos.pos.first->insert(pos.pos.second, value);
		if ((int)pos.pos.first->size() > 2 * __partSize()) {
			__split(pos.pos.first);
		}
		__maintain();
		return iterator(this, __getListIterator(pos.rank), pos.rank);
	}
	/**
	 * removes specified element at pos.
	 * removes the element at pos.
	 * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
	 */
	iterator erase(iterator pos) {
		if (pos == end()) {
			throw container_is_empty();
		}
		ss --;
		if (ss < 0) {
			throw container_is_empty();
		}
		pos.pos.first->erase(pos.pos.second);
		if (pos.pos.first->empty()) {
			list.erase(pos.pos.first);
		}
		__maintain();
		return iterator(this, __getListIterator(pos.rank), pos.rank);
	}
	/**
	 * adds an element to the end
	 */
	void push_back(const T &value) {
		ss ++;
		if (empty() || (int)list.back().size() == 2 * __partSize()) {
			list.push_back(std::list<T>(1, value));
			return ;
		}
		list.back().push_back(value);
		__maintain();
	}
	/**
	 * removes the last element
	 */
	void pop_back() {
		if (empty()) {
			throw container_is_empty();
		}
		ss --;
		list.back().pop_back();
		if (list.back().empty()) {
			list.pop_back();
		}
		__maintain();
	}
	/**
	 * inserts an element to the beginning.
	 */
	void push_front(const T &value) {
		ss ++;
		if (empty() || (int)list.front().size() == 2 * __partSize()) {
			list.push_front(std::list<T>(1, value));
			return ;
		}
		list.front().push_front(value);
		__maintain();
	}
	/**
	 * removes the first element.
	 */
	void pop_front() {
		if (empty()) {
			throw container_is_empty();
		}
		ss --;
		list.front().pop_front();
		if (list.front().empty()) {
			list.pop_front();
		}
		__maintain();
	}
};

}

#endif
