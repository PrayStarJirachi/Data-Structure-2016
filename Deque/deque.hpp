#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"
#include <cstddef>
#include <algorithm>

namespace sjtu { 

template<class T>
class deque{
private:
	template<class FT, class ST>
	class Pair{
	public:
		FT first;
		ST second;

		Pair(const Pair &rhs) : first(rhs.first), second(rhs.second) {}
		Pair(const FT &first, const ST &second) : first(first), second(second) {}

	};
	template<class D>
	struct List{
		struct ListNode;
		struct StructNode{
			ListNode *back, *next;
			StructNode() : back(NULL), next(NULL) {}; 
		}*head, *tail;

		struct ListNode : public StructNode{
			D data;
			ListNode(const D &data) : data(data) {}
		};

		size_t total;

		List() : head(new StructNode), tail(new StructNode), total(0) {
			head -> next = static_cast<ListNode *>(tail);
			tail -> back = static_cast<ListNode *>(head);
		}
		~List() {
			clear();
			delete head;
			delete tail;
		}

		inline void clear() {
			for (ListNode *it = head -> next; it != tail; ) {
				ListNode *tmp = it -> next;
				remove(it);
				it = tmp;
			}
		}
		inline void addHead(ListNode *p, const D &data) {
			ListNode *newNode = new ListNode(data);
			total++;
			newNode -> next = p;
			newNode -> back = p -> back;
			p -> back -> next = newNode;
			p -> back = newNode;
		}
		inline void addTail(ListNode *p, const D &data) {
			ListNode *newNode = new ListNode(data);
			total++;
			newNode -> back = p;
			newNode -> next = p -> next;
			p -> next -> back = newNode;
			p -> next = newNode;
		}
		inline void remove(ListNode *p) {
			p -> next -> back = p -> back;
			p -> back -> next = p -> next;
			delete p;
			total--;
			p = NULL;
		}
		List(const List<T> &rhs) : head(new StructNode), tail(new StructNode), total(rhs.total) {
			head -> next = static_cast<ListNode *>(tail);
			tail -> back = static_cast<ListNode *>(head);
			for (ListNode *it = rhs.head -> next; it != rhs.tail; it = it -> next) {
				addHead(static_cast<ListNode *>(tail), it -> data);
			}
		}
		List<T> & operator =(const List<T> &rhs) {
			if (this == &rhs) {
				return *this;
			}
			clear();
			total = rhs.total;
			for (ListNode *it = rhs.head -> next; it != rhs.tail; it = it -> next) {
				addHead(static_cast<ListNode *>(tail), it -> data);
			}
			return *this;
		}

	};
	struct AuxPtr{
		typename List<T>::ListNode *data;
		size_t size;
		AuxPtr(typename List<T>::ListNode *data, const size_t &size) : data(data), size(size) {}

	};

	typedef Pair<typename List<T>::ListNode *, typename List<AuxPtr>::ListNode *> PtrPair;

	size_t total, threshold;
	List<T> data;

public:
	List<AuxPtr> auxList;

private:
	inline void reAllocate() {
		auxList.clear();
		threshold = static_cast<size_t>(ceil(sqrt(total)));
		typename List<T>::ListNode *it = data.head -> next;
		for (int i = 0; i < total; i++) {
			if (i % threshold == 0) {
				auxList.addHead(static_cast<typename List<AuxPtr>::ListNode *>(auxList.tail), 
						        AuxPtr(it, std::min(threshold, total - i)));
			}
			it = it -> next;
		}
		auxList.addHead(static_cast<typename List<AuxPtr>::ListNode *>(auxList.tail), AuxPtr(it, 0));
	}
	inline void maintain(typename List<AuxPtr>::ListNode *&auxptr) {
		size_t tmp = static_cast<size_t>(ceil(sqrt(total)));
		if (threshold * 2 < tmp || threshold > 2 * tmp) {
			reAllocate();
			return;
		}
		while (auxptr -> data.size * 2 < threshold && auxptr -> next != auxList.tail -> back) {
			auxptr -> data.size += auxptr -> next -> data.size;
			auxList.remove(auxptr -> next);
		}
		while (auxptr -> data.size * 2 < threshold && auxptr -> back != auxList.head) {
			auxptr -> back -> data.size += auxptr -> data.size;
			typename List<AuxPtr>::ListNode *it = auxptr;
			auxptr = auxptr -> back;
			auxList.remove(it);
		}
		if (auxptr -> data.size >= 2 * threshold) {
			typename List<T>::ListNode *now = auxptr -> data.data;
			for (int i = 0; i < threshold; i++) {
				now = now -> next;
			}
			auxList.addTail(auxptr, AuxPtr(now, (auxptr -> data.size) - threshold));
			auxptr -> data.size = threshold;
		}
	}

public:
	class const_iterator;
	class iterator {
	friend class deque<T>;
	private:
		deque<T> *address;
		size_t id, rank;
		PtrPair ptr;
		explicit iterator(deque<T> * const address, const PtrPair &ptr,
						  const size_t &id, const size_t &rank) : address(address), ptr(ptr), id(id), rank(rank) {}
		void rMove(const size_t &n) {
			if (id + n > address -> total) {
				throw invalid_iterator();
			}
			size_t tmp = n;
			while (tmp != 0) {
				if (tmp + 1 <= (ptr.second -> data.size) - rank) {
					for (int i = 0; i < tmp; i++) {
						ptr.first = ptr.first -> next;
					}
					rank += tmp;
					tmp = 0;
				} else {
					tmp -= (ptr.second -> data.size) - rank;
					ptr.second = ptr.second -> next;
					ptr.first = ptr.second -> data.data;
					rank = 0;
				}
			}
			id += n;
		}
		void lMove(const size_t &n) {
			if (id < n) {
				throw invalid_iterator();
			}
			size_t tmp = n;
			while (tmp != 0) {
				if (tmp <= rank) {
					for (int i = 0; i < tmp; i++) {
						ptr.first = ptr.first -> back;
					}
					rank -= tmp;
					tmp = 0;
				} else {
					tmp -= rank + 1;
					ptr.first = ptr.second -> data.data -> back;
					ptr.second = ptr.second -> back;
					rank = ptr.second -> data.size - 1;
				}
			}
			id -= n;
		}

	public:
		inline iterator operator +(const int &n) const {
			iterator ret = *this;
			if (n < 0) ret.lMove(-n);
			if (n > 0) ret.rMove(n);
			return ret;
		}
		inline iterator operator -(const int &n) const {
			iterator ret = *this;
			if (n < 0) ret.rMove(-n);
			if (n > 0) ret.lMove(n);
			return ret;
		}
		inline int operator -(const iterator &rhs) const {
			return static_cast<int>(id) - static_cast<int>(rhs.id);
		}
		inline iterator & operator +=(const int &n) {
			if (n < 0) lMove(-n);
			if (n > 0) rMove(n);
			return *this;
		}
		inline iterator & operator -=(const int &n) {
			if (n < 0) rMove(-n);
			if (n > 0) lMove(n);
			return *this;
		}
		inline iterator operator ++(int) {
			iterator ret = *this;
			*this = *this + 1;
			return ret;
		}
		inline iterator & operator ++() {
			return *this = *this + 1;
		}
		inline iterator operator --(int) {
			iterator ret = *this;
			*this = *this - 1;
			return ret;
		}
		inline iterator & operator --() {
			return *this = *this - 1;
		}
		inline T & operator *() const {
			return ptr.first -> data;
		}
		inline T * operator->() const noexcept {
			return &(ptr.first -> data);
		}
		inline bool operator ==(const iterator &rhs) const {
			return ptr.first == rhs.ptr.first;
		}
		inline bool operator ==(const const_iterator &rhs) const {
			return ptr.first == rhs.ptr.first;
		}
		inline bool operator !=(const iterator &rhs) const {
			return ptr.first != rhs.ptr.first;
		}
		inline bool operator !=(const const_iterator &rhs) const {
			return ptr.first != rhs.ptr.first;
		}

	};
	deque() : total(0), threshold(0) {
		auxList.addTail(static_cast<typename List<AuxPtr>::ListNode *>(auxList.head), 
				 AuxPtr(static_cast<typename List<T>::ListNode *>(data.tail), 0));
	}
	deque(const deque &other) : data(other.data), total(other.total) {
		reAllocate();
	}
	deque & operator =(const deque &other) {
		if (this == &other) {
			return *this;
		}
		clear();
		data = other.data;
		total = other.total;
		reAllocate();
		return *this;
	}
	inline T & at(const size_t &pos) {
		return *(begin() + pos);
	}
	inline const T & at(const size_t &pos) const {
		return *(cbegin() + pos);
	}
	inline T & operator[](const size_t &pos) {
		return at(pos);
	}
	inline const T & operator[](const size_t &pos) const {
		return at(pos);
	}
	inline const T & front() const {
		return data.head -> next -> data;
	}
	inline const T & back() const {
		return data.tail -> back -> data;
	}
	inline iterator begin() {
		return iterator(this, PtrPair(data.head -> next, auxList.head -> next), 0, 0);
	}
	inline const_iterator cbegin() const {
		return const_iterator(this, PtrPair(data.head -> next, auxList.head -> next), 0, 0);
	}
	inline iterator end() {
		return iterator(this, PtrPair(static_cast<typename List<T>::ListNode *>(data.tail), 
					    auxList.tail -> back), total, 0);
	}
	inline const_iterator cend() const {
		return const_iterator(this, PtrPair(static_cast<typename List<T>::ListNode *>(data.tail), 
					          auxList.tail -> back), total, 0);
	}
	inline bool empty() const {
		return total == 0;
	}
	inline size_t size() const {
		return total;
	}
	inline void clear() {
		data.clear();
		auxList.clear();
		total = 0;
		threshold = 0;
	}
	iterator insert(iterator pos, const T &value) {
		if (pos.address != this) {
			throw invalid_iterator();
		}
		data.addHead(pos.ptr.first, value);
		total++;
		if (total == 1) {
			auxList.addTail(static_cast<typename List<AuxPtr>::ListNode *>(auxList.head), AuxPtr(pos.ptr.first -> back, 1));
			threshold = 1;
			return iterator(this, PtrPair(pos.ptr.first -> back, auxList.tail -> back), 0, 0);
		}
		if (pos.ptr.second -> data.data == pos.ptr.first) {
			auxList.addHead(pos.ptr.second, AuxPtr(pos.ptr.first -> back, pos.ptr.second -> data.size));
			typename List<AuxPtr>::ListNode *tmp = pos.ptr.second -> back;
			if (pos.ptr.second != auxList.tail -> back) {
				auxList.remove(pos.ptr.second);
			}
			pos.ptr.second = tmp;
		}
		pos.ptr.first = pos.ptr.first -> back;
		pos.ptr.second -> data.size++;
		maintain(pos.ptr.second);
		return pos;
	}
	iterator erase(iterator pos) {
		if (pos.address != this || pos.ptr.first == data.tail) {
			throw invalid_iterator();
		}
		total--;
		if (pos.ptr.second -> data.data == pos.ptr.first) {
			auxList.addTail(pos.ptr.second, AuxPtr(pos.ptr.first -> next, pos.ptr.second -> data.size));
			typename List<AuxPtr>::ListNode *tmp = pos.ptr.second -> next;
			auxList.remove(pos.ptr.second);
			pos.ptr.second = tmp;
		}
		pos.ptr.first = pos.ptr.first -> next;
		pos.ptr.second -> data.size--;
		if (pos.ptr.second != auxList.tail -> back) {
			if (pos.ptr.second == 0) {
				typename List<AuxPtr>::ListNode *tmp = pos.ptr.second -> next;
				auxList.remove(pos.ptr.second);
				pos.ptr.second = tmp;
			}
			data.remove(pos.ptr.first -> back);
			maintain(pos.ptr.second);
		}
		return pos;
	}
	inline void push_back(const T &value) {
		insert(end(), value);
	}
	inline void pop_back() {
		erase(--end());
	}
	inline void push_front(const T &value) {
		insert(begin(), value);
	}
	inline void pop_front() {
		erase(begin());
	}

	virtual ~deque() {}

};

}

#endif
