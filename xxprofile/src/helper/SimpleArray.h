//////////////////////////////////////////////////////////////////////////
// 文件: SimpleArray.h
// 作者: BianChui
// 时间: 2010-09-06 10:06
// 说明: 这是一个快速Array，使用这个数组必须具有如下属性
//       1.不需要构造、复制构造和析构，比如基本类型
//       2.必须声明SIMPLEARRAY_ACCEPT(T)
//////////////////////////////////////////////////////////////////////////
#ifndef SimpleArray_h__
#define SimpleArray_h__
#include "SimpleConfig.h"
#include <string.h>

SIMPLE_SPACE_BEGIN;

#if SIMPLEARRAY_ACCEPTCHECK
template<typename T>
struct TArrayAcceptType;

#  define SIMPLEARRAY_ACCEPT(T) template<> struct TArrayAcceptType<T> { typedef T Type; }
#else//SIMPLEARRAY_ACCEPTCHECK
#  define SIMPLEARRAY_ACCEPT(T)
#endif//SIMPLEARRAY_ACCEPTCHECK

struct Allocator {
	static FORCEINLINE void* Alloc(size_t InSize, size_t &InCount) {
		return malloc(InSize * InCount);
	}

	static FORCEINLINE void Free(void* Memory) {
		free(Memory);
	}
};

template<typename T, typename Al>
class TArray {
#if SIMPLEARRAY_ACCEPTCHECK
	typedef typename TArrayAcceptType<T>::Type ArrayIsAcceptType;
#endif//SIMPLEARRAY_ACCEPTCHECK
public:
	FORCEINLINE TArray()
		: _data(NULL)
		, _size(0)
		, _sizeMax(0) {
	}

	FORCEINLINE ~TArray() {
		if (_data != NULL) {
			Al::Free(_data);
		}
	}

#if SIMPLEARRAY_HAVECOPY
	FORCEINLINE TArray(const TArray& Other)
		: _data(NULL)
		, _size(0)
		, _sizeMax(0)
	{
		*this = Other;
	}

	FORCEINLINE TArray &operator =(const TArray& Other) {
		if (&Other != this) {
			resize(Other.size());
			if (Other.size() != 0) {
				memcpy(_data, Other._data, _size * sizeof(T));
			}
		}

		return *this;
	}
#endif//SIMPLEARRAY_HAVECOPY

	void reserve(size_t InMax) {
		if (_sizeMax < InMax) {
			T *pOldData = _data;

			_data = (T*)Al::Alloc(sizeof(T), InMax);
			if (_data == NULL) {
				_data = pOldData;
#if SIMPLEARRAY_THROW
				throw std::bad_alloc("allocator");
#endif//SIMPLEARRAY_THROW
				return;
			}
			if (_size != 0) {
				memcpy(_data, pOldData, _size * sizeof(T));
			}
			if (pOldData != NULL) {
				Al::Free(pOldData);
			}

			_sizeMax = InMax;
		}
	}

	FORCEINLINE void resize(size_t stSize) {
		if (_sizeMax < stSize) {
			reserve(stSize);
		}
		_size = stSize;
	}

#if SIMPLEARRAY_NEEDMORE
	FORCEINLINE void ResizeZeroed(size_t stSize) {
		size_t stOldSize = _size;
		resize(stSize);
		if (stOldSize < stSize) {
			memset(_data + stOldSize, 0, (stSize - stOldSize) * sizeof(T));
		}
	}

	FORCEINLINE void FillAll(int chFill) {
		if (_size != 0) {
			memset(_data, chFill, _size * sizeof(T));
		}
	}

	FORCEINLINE void ResizeFillAll(size_t stSize, int chFill) {
		resize(stSize);
		FillAll(chFill);
	}

	FORCEINLINE void ResizeZeroedAll(size_t stSize) {
		ResizeFillAll(stSize, 0);
	}
#endif//SIMPLEARRAY_NEEDMORE

	FORCEINLINE void clear() {
		_size = 0;
	}

	FORCEINLINE size_t size() const {
		return _size;
	}

	FORCEINLINE void push_back(const T &t) {
		size_t OldSize = _size;
		if (OldSize+1 > _sizeMax) {
			reserve((OldSize + 1) < 4 ? 4 : (OldSize + (OldSize>>1)));
		}
		_data[OldSize] = t;
		++_size;
	}

	FORCEINLINE size_t capacity() const {
		return _sizeMax;
	}

	FORCEINLINE const T *begin() const {
		return _data;
	}

	FORCEINLINE const T *end() const {
		return _data + _size;
	}

	FORCEINLINE T *begin() {
		return _data;
	}

	FORCEINLINE T *end() {
		return _data + _size;
	}

	FORCEINLINE T &operator[](size_t stIndex) {
		return _data[stIndex];
	}

	FORCEINLINE const T &operator[](size_t stIndex) const {
		return _data[stIndex];
	}

private:
#if !SIMPLEARRAY_HAVECOPY
	DISALLOW_COPY_AND_ASSIGN(TArray);
#endif//SIMPLEARRAY_HAVECOPY
	T *_data;
	size_t _size;
	size_t _sizeMax;
};

SIMPLEARRAY_ACCEPT(float);
SIMPLEARRAY_ACCEPT(char);
SIMPLEARRAY_ACCEPT(signed char);
SIMPLEARRAY_ACCEPT(unsigned char);
SIMPLEARRAY_ACCEPT(int);
SIMPLEARRAY_ACCEPT(short);
SIMPLEARRAY_ACCEPT(unsigned short);
SIMPLEARRAY_ACCEPT(long);
SIMPLEARRAY_ACCEPT(unsigned long);
SIMPLEARRAY_ACCEPT(double);
#ifdef _NATIVE_WCHAR_T_DEFINED
SIMPLEARRAY_ACCEPT(wchar_t);
#endif//_NATIVE_WCHAR_T_DEFINED

template<size_t Size>
static FORCEINLINE void SwapMem(void* t1, void* t2) {
	char Temp[Size];
	memcpy(Temp, t1, Size);
	memcpy(t1, t2, Size);
	memcpy(t2, Temp, Size);
}

SIMPLE_SPACE_END;

#endif//SimpleArray_h__
