#ifndef SJTU_COMMON_HPP
#define SJTU_COMMON_HPP

template<class T>
class DynamicType {
private:
    int *pct;
    T *data;

public:
    DynamicType (int *p) : pct(p) , data(new T) {
        (*pct)++;
    }
    DynamicType (const DynamicType &other) : pct(other.pct), data(new T) {
        (*pct)++;
    }
    DynamicType &operator =(const DynamicType &other) {
        if (this == &other) return *this;
        (*pct)--;
        pct = other.pct;
        (*pct)++;
        delete [] data;
        data = new T;
        return *this;
    }
    ~DynamicType() {
        delete [] data;
        (*pct)--;
    }
    bool operator != (const DynamicType &rhs) const {
		return false;
	}
};

#endif
