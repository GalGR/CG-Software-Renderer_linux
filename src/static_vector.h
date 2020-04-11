#pragma once

#include <assert.h>

template <typename T, typename T_arr>
class static_vector {
	T_arr &arr_;
	size_t size_;
	size_t capacity_;

public:
	static_vector(T_arr &arr, size_t capacity) : arr_(arr), size_(0), capacity_(capacity) {}

	static_vector() = delete;
	static_vector(const static_vector &) = delete;
	static_vector &operator =(const static_vector &) = delete;

	size_t size() const {
		return this->size_;
	}
	size_t capacity() const {
		return this->capacity_;
	}
	void reserve(size_t capacity) {
		assert(this->capacity_ >= capacity);
	}

	void resize(size_t i) {
		assert(i < this->capacity_);
		this->size_ = i;
	}

	void clear() {
		this->size_ = 0;
	}

	T &operator [](size_t i) {
		assert(i < this->size_);
		return this->arr_[i];
	}
	const T &operator [](size_t i) const {
		assert(i < this->size_);
		return this->arr_[i];
	}

	void push_back(T obj) {
		assert(size < this->capacity_);
		this->arr_[size++] = obj;
	}
};