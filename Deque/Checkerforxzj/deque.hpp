#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include <cstddef>
#include "exceptions.hpp"
#include <iostream>
#include <cstdio>
namespace sjtu { 

template<class T>
class deque {
friend class iterator;
friend class const_iterator;
private:
	struct Block{
		T** storage;
		int head, tail, num, max_size;
		Block *pre, *next;
		Block(int size):max_size(size), num(0), head(0), tail(size - 1), pre(NULL), next(next){
			storage = new T*[size];
		}
		~Block(){
			int t = head;
			for(int i = 0; i < num; i++){
				delete storage[t];
				if(++t == max_size) t = 0;
			}
			delete []storage;	
		}
		T &front()const {
			return *(storage[head]);
		}
		T &back()const {
			return *(storage[tail]);
		}
		T &at(int x)const{
			return *(storage[x]);	
		}
	};
	struct Pos{
		Block *block;
		int pos;
		Pos():block(NULL), pos(0){}
		Pos(Block *a, int b):block(a), pos(b){}	
		bool operator == (const Pos &other)const {
			return (block == other.block && pos == other.pos);	
		}
	};
	Block *start;
	int tot_size, gen, num_block, max_size, tolerance;
	Pos End;
	Pos Begin()const {
		if(tot_size == 0) return End;
		return Pos(start, start -> head);
	}
	Pos Next(const Pos &x, int m){
		if(x.pos != x.block -> tail) return Pos(x.block, (x.pos == m - 1) ? 0 : (x.pos + 1));
		if(x.block == start -> pre) return End;
		return Pos(x.block -> next, x.block -> next -> head);
	}
	Pos Next(const Pos &x)const {
		if(x == End) return End;
		if(x.pos != x.block -> tail) return Pos(x.block, (x.pos == max_size - 1) ? 0 : (x.pos + 1));
		if(x.block == start -> pre) return End;
		return Pos(x.block -> next, x.block -> next -> head);
	}
	Pos Pre(const Pos &x)const {
		if(x == End) 
		{
			if(tot_size) return Pos(start -> pre, start -> pre -> tail);
				else return x;
		}
		if(x.pos != x.block -> head) return Pos(x.block, (x.pos == 0) ? (max_size - 1) : (x.pos - 1));
		if(x.block == start) return End;
		return Pos(x.block -> pre, x.block -> pre -> tail);
	}
	int Get_rank(const Pos &x)const{
		if(x.block == NULL) 
		{
			if(x.pos < 0)return x.pos;
				else return tot_size + x.pos;
		}
		if(x.block == NULL) return tot_size;
		int tmp = 0;
		for(Block *now = start;;now = now -> next){
			if(now == x.block) return tmp + ((x.pos - now -> head) < 0 ? (x.pos - now -> head + max_size) : (x.pos - now -> head));
			else tmp += now -> num;
		}
	}
	Pos Get_pos(int pos) const {
		if(pos < 0) return Pos(NULL, pos);
		if(pos >= tot_size) return Pos(NULL, pos - tot_size);
		Block *now_block = start;
		for(;pos >= now_block -> num; now_block  = now_block -> next) pos -= now_block -> num;
		return Pos(now_block, ((now_block -> head + pos) >= max_size ?  (now_block -> head + pos - max_size) : (now_block -> head + pos)));
	}
	void Insert_block(Block *x, Block *y){
		y -> next = x -> next;
		y -> pre = x;
		y -> next -> pre = y;
		y -> pre -> next = y;
	}
	void Split(Block *now){
		Block *other;
		other = new Block(max_size);
		for(int i = tolerance ; i < now -> num; i++){
			Push_back(other, *(now -> storage[(now -> head + i) >= max_size ? (now -> head + i - max_size) : (now -> head + i)]));
			delete now -> storage[(now -> head + i) >= max_size ? (now -> head + i - max_size) : (now -> head + i)];
		}
		now -> tail = now -> head + tolerance - 1;
		if(now -> tail >= max_size) now -> tail -= max_size;
		now -> num = tolerance;
		num_block++;
		Insert_block(now, other);
	}
	void Delete(Block *now){
		num_block --;
		if(num_block ==0){
			delete now;
			start = NULL;
		}
		else{
			now -> pre -> next = now -> next;
			now -> next -> pre = now -> pre;
			if(start == now) start = now -> next;
			delete now;	
		}
	}
	void Push_back(Block *now, const T &x){
		if(now -> num == max_size){
			Split(now);
			now = now -> next;
		}
		now -> num++;
		if(++now -> tail == max_size) now -> tail = 0;
		now -> storage[now -> tail] = new T(x);
	}
	void Push_front(Block *now, const T &x){
		if(now -> num == max_size){
			Split(now);
		}
		now -> num++;
		if(--now -> head < 0) now -> head = max_size - 1;
		now -> storage[now -> head] = new T(x);
	}
	void Pop_back(Block *now){
		now -> num--;
		delete now -> storage[now -> tail];
		if(--now -> tail < 0) now -> tail = max_size - 1;
		if(now -> num == 0) Delete(now);
	}
	void Pop_front(Block *now){
		now -> num--;
		delete now -> storage[now -> head];
		if(++now -> head == max_size) now -> head = 0;	
		if(now -> num == 0) {
			if(start == now) start = now -> next;
			Delete(now);
		}
	}
	void Insert(Pos p, const T &value){
		Block *b = p.block;
		b -> num++;
		int pos = p.pos;
		for(int i = b -> tail;;i = (i==0?(max_size - 1):(i-1))){
			b -> storage[(i + 1) == max_size ? (i + 1 - max_size) : (i + 1)] = b -> storage[i];
			if(i == pos) break;
		}
		if(++b -> tail == max_size) b -> tail = 0;
		b -> storage[pos] = new T(value);
	}
	void Update(){
		if((gen - 1) * (gen - 1) > tot_size) gen --;
		if(gen * gen < tot_size) gen ++;
	}
	void Check(){
		if(tot_size == 0) return;
		if(tolerance >= gen / 2 && tolerance <= gen * 2 && num_block <= max_size) return;
		//printf("rebuid  tot_size = %d  tolerance = %d  num_block = %d\n",tot_size,tolerance,num_block);
		Block *now = start;
		int initial_max_size = max_size, initial_num_block = num_block, cnt = 0;
		tolerance = gen;
		max_size = 2 * gen;
		start = new Block(max_size);
		Block *p = start;
		start -> pre = start -> next = start;
		num_block = 1;
		for(int j = 0; j < initial_num_block; j++){
			for(int i = 0; i < now -> num; i++){
				if(cnt == gen){
					Block *tmp = new Block(max_size);
					Insert_block(p, tmp);
					p = tmp;
					cnt = 0;
					num_block++;
				}
				cnt++;
				p -> num++;
				if(++p -> tail == max_size) p -> tail = 0;
				p -> storage[p -> tail] = new T(*(now -> storage[(now -> head + i) % initial_max_size]));
			}
			Block *tmp = now -> next;
			delete now;
			now = tmp;
		}
		//printf("new  tot_size = %d  tolerance = %d  num_block = %d\n",tot_size,tolerance,num_block);
	}
	Pos Last(){
		if(tot_size == 0) return End;
		return Pos(start -> pre, start -> pre -> tail);	
	}
public:
	class const_iterator;
	class iterator {
	public:
		deque *z;
		Pos p;
	public:
		iterator():z(NULL){}
		iterator(deque *z, const Pos &p):z(z), p(p){}
		iterator operator+(const int &n) {
			return iterator(z, z -> Get_pos(z -> Get_rank(p) + n));
		}
		iterator operator-(const int &n) {
			return iterator(z, z -> Get_pos(z -> Get_rank(p) - n));
		}
		int operator-(const iterator &rhs) const{
			if(z != rhs.z) throw invalid_iterator();
			return z -> Get_rank(p) - z -> Get_rank(rhs.p);
		}
		iterator operator+=(const int &n) {
			p = z -> Get_pos(z -> Get_rank(p) + n);
			return *this;
		}
		iterator operator-=(const int &n) {
			p = z -> Get_pos(z -> Get_rank(p) - n);
			return *this;	
		}
		iterator operator++(int) {	//it++
			iterator tmp = *this;
			p = z -> Next(p);
			return tmp;
		}
		iterator& operator++() {	//++it
			p = z -> Next(p);
			return *this;
		}
		iterator operator--(int) {	//it--
			iterator tmp = *this;
			p = z -> Pre(p);
			return tmp;
		}
		iterator& operator--() {	//--it
			p = z -> Pre(p);
			return *this;
		}
		T& operator*() const{
			if(p.block == NULL) throw invalid_iterator();
			return p.block -> at(p.pos);
		}
		T* operator->() const noexcept {
			if(p.block == NULL) throw invalid_iterator();
			return &(p.block -> at(p.pos));
		}
		bool operator==(const iterator &rhs) const {
			return z == rhs.z && p == rhs.p;
		}
		bool operator==(const const_iterator &rhs) const {
			return z == rhs.z && p == rhs.p;
		}
		bool operator!=(const iterator &rhs) const {
			return !(z == rhs.z && p == rhs.p);
		}
		bool operator!=(const const_iterator &rhs) const {
			return !(z == rhs.z && p == rhs.p);
		}
	};
	class const_iterator {
	public:
		const deque *z;
		Pos p;
	public:
		const_iterator():z(NULL){}
		const_iterator(const deque *z, const Pos &p):z(z), p(p){}
		const_iterator operator+(const int &n) {
			return const_iterator(z, z -> Get_pos(z -> Get_rank(p) + n));
		}
		const_iterator operator-(const int &n) const {
			return const_iterator(z, z -> Get_pos(z -> Get_rank(p) - n));
		}
		int operator-(const const_iterator &rhs) {
			if(z != rhs.z) throw invalid_iterator();
			return z -> Get_rank(p) - z -> Get_rank(rhs.p);
		}
		const_iterator operator+=(const int &n) {
			p = z -> Get_pos(z -> Get_rank(p) + n);
			return *this;
		}
		const_iterator operator-=(const int &n) {
			p = z -> Get_pos(z -> Get_rank(p) - n);
			return *this;	
		}
		const_iterator operator++(int) {	//it++
			const_iterator tmp = *this;
			p = z -> Next(p);
			return tmp;
		}
		const_iterator& operator++() {	//++it
			p = z -> Next(p);
			return *this;
		}
		const_iterator operator--(int) {	//it--
			const_iterator tmp = *this;
			p = z -> Pre(p);
			return tmp;
		}
		const_iterator& operator--() {	//--it
			p = z -> Pre(p);
			return *this;
		}
		const T& operator*() const{
			if(p.block == NULL) throw invalid_iterator();
			return p.block -> at(p.pos);
		}
		const T* operator->() const noexcept {
			if(p.block == NULL) throw invalid_iterator();
			return &(p.block -> at(p.pos));
		}
		bool operator==(const iterator &rhs) const {
			return z == rhs.z && p == rhs.p;
		}
		bool operator==(const const_iterator &rhs) const {
			return z == rhs.z && p == rhs.p;
		}
		bool operator!=(const iterator &rhs) const {
			return !(z == rhs.z && p == rhs.p);
		}
		bool operator!=(const const_iterator &rhs) const {
			return !(z == rhs.z && p == rhs.p);
		}
	};
	deque():tot_size(0), gen(0), max_size(0), num_block(0), start(NULL){}
	deque(const deque<T> &other):tot_size(0), gen(0), max_size(0), num_block(0), start(NULL){
		Pos now = other.Begin();
		for(int i = 0 ; i < other.tot_size; i++){
			push_back(now.block -> at(now.pos));
			now = Next(now, other.max_size);
		}
	}
	~deque() {
		clear();
	}
	deque<T> &operator=(const deque<T> &other) {
		if(this == &other) return *this;
		clear();
		Pos now = other.Begin();
		for(int i = 0 ; i < other.tot_size; i++){
			push_back(now.block -> at(now.pos));
			now = Next(now, other.max_size);
		}
		return *this;
	}
	T & at(const size_t &pos) {
		if(pos>=0 && pos<tot_size){
			Pos tmp = Get_pos(pos);
			return tmp.block -> at(tmp.pos);	
		}
		else throw index_out_of_bound();
	}
	const T & at(const size_t &pos) const {
		if(pos>=0 && pos<tot_size){
			Pos tmp = Get_pos(pos);
			return tmp.block -> at(tmp.pos);	
		}
		else throw index_out_of_bound();
	}
	T & operator[](const size_t &pos) {
		if(pos>=0 && pos<tot_size){
			Pos tmp = Get_pos(pos);
			return tmp.block -> at(tmp.pos);	
		}
		else throw index_out_of_bound();
	}
	const T & operator[](const size_t &pos) const {
		if(pos>=0 && pos<tot_size){
			Pos tmp = Get_pos(pos);
			return tmp.block -> at(tmp.pos);	
		}
		else throw index_out_of_bound();
	}
	const T & front() const {
		if(!tot_size) throw container_is_empty();
		return start -> front();
	}
	const T & back() const {
		if(!tot_size) throw container_is_empty();
		return start -> pre -> back();
	}
	iterator begin() {
		return iterator(this, Begin());
	}
	const_iterator cbegin() const {
		return const_iterator(this, Begin());
	}
	iterator end() {
		return iterator(this, End);
	}
	const_iterator cend() const {
		return const_iterator(this, End);
	}
	bool empty() const {
		return tot_size == 0;
	}
	size_t size() const {
		return tot_size;
	}
	void clear() {
		for(int i=0;i<num_block;i++){
			Block *tmp = start -> next;
			delete start;
			start = tmp;		
		}
		tot_size = gen = max_size = num_block = 0;
		start = NULL;
	}
	iterator insert(iterator pos, const T &value) {
		if(pos.z != this) throw invalid_iterator();
		if(pos.p == End){
			push_back(value);
			return iterator(this, Last());	
		}
		else if(pos.p == Begin()){
			push_front(value);
			return iterator(this, Begin());	
		}
		else{
			if(pos.p.block == NULL) throw invalid_iterator();
			int rank = Get_rank(pos.p);
			tot_size++;
			Update();
			if(pos.p.block -> num == max_size) {
				Split(pos.p.block);
				Insert(Get_pos(rank), value);
			}
			else{
				Insert(pos.p, value);
			}
			Check();
			return iterator(this, Get_pos(rank));
		}
	}
	iterator erase(iterator pos) {
		if(pos.z != this || pos.p.block == NULL) throw invalid_iterator();
		tot_size--;
		Update();
		int rank = Get_rank(pos.p);
		Block *b = pos.p.block;
		int t = pos.p.pos;
		delete b -> storage[t];
		while (t!=b -> tail){
			b -> storage[t] = b -> storage[(t + 1) == max_size ? (t + 1 - max_size) : (t + 1)];
			if(++t == max_size) t = 0;
		}
		if(--b -> tail < 0) b -> tail = max_size - 1;
		b -> num--;
		if(b -> num == 0) Delete(b);
		Check();
		return iterator(this, Get_pos(rank));
	}
	void push_back(const T &value) {
		tot_size++;
		Update();
		if(tot_size == 1){
			tolerance = 1;
			num_block = 1;
			max_size = 2;
			start = new Block(max_size);
			start -> pre = start -> next = start;
		}
		Push_back(start -> pre, value);
		Check();
	}
	void pop_back() {
		if(tot_size == 0) throw container_is_empty();	
		else{
			tot_size--;
			Update();
			Pop_back(start -> pre);
			if(tot_size) Check();
		}
	}
	void push_front(const T &value) {
		tot_size++;
		Update();
		if(tot_size == 1){
			tolerance = 1;
			num_block = 1;
			max_size = 2;
			start = new Block(max_size);
			start -> pre = start -> next = start;
		}
		Push_front(start, value);
		Check();
	}
	void pop_front() {
		if(tot_size == 0) throw container_is_empty();	
		else{
			tot_size--;
			Update();
			Pop_front(start);
			if(tot_size) Check();
		}
	}
};

}

#endif
