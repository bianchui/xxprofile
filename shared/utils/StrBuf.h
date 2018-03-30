// Copyright 2018 bianchui. All rights reserved.
#ifndef shared_utils_StrBuf_h__
#define shared_utils_StrBuf_h__
#include <stdio.h>

namespace shared {

class StrBufBase {
public:
	StrBufBase(char* buf, size_t capacity) : _sbuf(buf), _dbuf(buf) {
		buf[0] = 0;
		_capacity = capacity;
		_length = 0;
	}
	~StrBufBase() {
		if (_dbuf != _sbuf) {
			free(_dbuf);
		}
	}
	void printf(const char* format, ...) {
		va_list ap;
		va_start(ap, format);
		vprintf(format, ap);
		va_end(ap);
	}
	void vprintf(const char* format, va_list ap) {
		int n = (int)_capacity;
		while(1) {
			int final_n = vsnprintf(_dbuf, n, format, ap);
			if (final_n < 0 || final_n >= n) {
				int add = final_n - n + 1;
				if (add < 0) {
					add = -add;
				}
				if (add < n) {
					add = n;
				}
				_capacity = (size_t)(n += add);
				if (_dbuf != _sbuf) {
					free(_dbuf);
				}
				_dbuf = (char*)malloc(n);
			} else {
				_length = final_n;
				break;
			}
		}
	}
	void appendf(const char* format, ...) {
		va_list ap;
		va_start(ap, format);
		vappendf(format, ap);
		va_end(ap);
	}
	void vappendf(const char* format, va_list ap) {
		while(1) {
			int n = _capacity - _length;
			int final_n = vsnprintf(_dbuf + _length, n, format, ap);
			if (final_n < 0 || final_n >= n) {
				int add = final_n - n + 1;
				if (add < 0) {
					add = -add;
				}
				if ((size_t)add < _capacity) {
					_capacity <<= 1;
				} else {
					_capacity += (size_t)add;
				}
				char* new_buf = (char*)malloc(_capacity);
				if (_length) {
					memcpy(new_buf, _dbuf, _length);
				}
				if (_dbuf != _sbuf) {
					free(_dbuf);
				}
				_dbuf = new_buf;
			} else {
				_length += final_n;
				break;
			}
		}
	}
	void assign(const char* str, size_t len = -1) {
		if (len == -1) {
			len = strlen(str);
		}
		if (len) {
			if (len + 1 > _capacity) {
				_capacity <<= 1;
				if ((len + 1) > _capacity) {
					_capacity = len + 1;
				}
				if (_dbuf != _sbuf) {
					free(_dbuf);
				}
				_dbuf = (char*)malloc(_capacity);

			}
			memcpy(_dbuf, str, len);
		}
		_dbuf[len] = 0;
		_length = len;
	}
	void append(const char* str, size_t len = -1) {
		if (len == -1) {
			len = strlen(str);
		}
		if (!len) {
			return;
		}
		size_t n = _capacity - _length;
		if (n < len + 1) {
			size_t new_capacity = _capacity + (len + 1);
			_capacity <<= 1;
			if (new_capacity > _capacity) {
				_capacity = new_capacity;
			}
			char* buf = (char*)malloc(_capacity);
			if (_length) {
				memcpy(buf, _dbuf, _length);
			}
			if (_dbuf != _sbuf) {
				free(_dbuf);
			}
			_dbuf = buf;
		}
		memcpy(_dbuf + _length, str, len);
		_length += len;
		_dbuf[_length] = 0;
	}

	const char* c_str() const {
		return _dbuf;
	}
	const char* get() const {
		return _dbuf;
	}
	const char* data() const {
		return _dbuf;
	}
	char* buf() {
		return _dbuf;
	}
	operator const char* () const {
		return c_str();
	}
	size_t length() const {
		return _length;
	}
	size_t capacity() const {
		return _capacity;
	}
	void clear() {
		_length = 0;
	}
	
private:
	char* _sbuf;
	char* _dbuf;
	size_t _capacity;
	size_t _length;

private:
	StrBufBase(const StrBufBase&& other) = delete;
	StrBufBase(const StrBufBase& other) = delete;
	StrBufBase& operator=(const StrBufBase&& other) = delete;
	StrBufBase& operator=(const StrBufBase& other) = delete;
};

template <size_t Capacity>
class StrBufT : StrBufBase {
public:
	StrBufT() : StrBufBase(_buf, Capacity) {
	}
	StrBufT(const char* format, ...) : StrBufBase(_buf, Capacity) {
		va_list ap;
		va_start(ap, format);
		vprintf(format, ap);
		va_end(ap);
	}
	StrBufT(const char* format, va_list ap) : StrBufBase(_buf, Capacity) {
		vprintf(format, ap);
	}
	~StrBufT() {
	}

private:
	StrBufT(const StrBufT&& other) = delete;
	StrBufT(const StrBufT& other) = delete;
	StrBufT& operator=(const StrBufT&& other) = delete;
	StrBufT& operator=(const StrBufT& other) = delete;

private:
	char _buf[Capacity];
};

typedef StrBufT<512> StrBuf;

}

#endif//shared_utils_StrBuf_h__
