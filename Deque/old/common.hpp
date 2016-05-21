#ifndef SJTU_COMMON_HPP
#define SJTU_COMMON_HPP

#include <ctime>

class Int{
private:
	int data;

public:
	Int() = default;
	Int(const int &data) : data(data) {}
	Int & operator =(const Int &rhs) = default;
	bool operator <(const Int &rhs) = delete;
	bool operator >(const Int &rhs) = delete;
	bool operator <=(const Int &rhs) = delete;
	bool operator >=(const Int &rhs) = delete;
	bool operator ==(const Int &rhs)const {
		return data == rhs.data;
	}
	bool operator !=(const Int &rhs)const {
		return data != rhs.data;
	}

};

class DynamicType {
public:
    int *pct;
    double *data;
    DynamicType (int *p) : pct(p) , data(new double[2]) {
        (*pct)++;
    }
    DynamicType (const DynamicType &other) : pct(other.pct), data(new double[2]) {
        (*pct)++;
    }
    DynamicType &operator =(const DynamicType &other) {
        if (this == &other) return *this;
        (*pct)--;
        pct = other.pct;
        (*pct)++;
        delete [] data;
        data = new double[2];
        return *this;
    }
    ~DynamicType() {
        delete [] data;
        (*pct)--;
    }
    bool operator != (const DynamicType &rhs) const { return false; }
};

class Timer{
private:
	long dfnStart, dfnEnd;

public:
	void init() {
		dfnEnd = dfnStart = clock();
	}
	void stop() {
		dfnEnd = clock();
	}
	double getTime() {
		return 1.0 * (dfnEnd - dfnStart) / CLOCKS_PER_SEC;
	}

};

#endif
