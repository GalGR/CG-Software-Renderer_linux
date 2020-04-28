#pragma once

#include <map>
#include <queue>
#include <tuple>
#include <mutex>

#include "TypeDefVal.h"

template <typename Key_T, typename Val_T, Key_T defKey, Val_T defVal, typename Compare_KeyVal = std::less<Key_T>, typename Compare_ValKey = std::less<Val_T>>
class KeyBind {
public:
	typedef std::map<Key_T, TypeDefVal<Val_T, defVal>, Compare_KeyVal> Key2Val;
	typedef std::map<Val_T, TypeDefVal<Key_T, defKey>, Compare_ValKey> Val2Key;

private:
	Key2Val key2val;
	Val2Key val2key;

public:
	Val_T atKey(const Key_T &key) { return this->key2val[key]; }
	Key_T atVal(const Val_T &val) { return this->val2key.operator[](val); }

	void set(const Key_T &key, const Val_T &val) {
		this->key2val[key] = val;
		this->val2key[val] = key;
	}
};

template <typename Key_T>
class KeyPress {
public:
	struct KeyAndPress {
		Key_T key;
		bool press;
	};

	typedef std::map<Key_T, TypeDefVal<bool, false>> KeyPressMap;
	typedef std::queue<KeyAndPress> KeyPressList;

private:
	KeyPressMap *prevMap = &pressMaps[1];
	KeyPressMap *currMap = &pressMaps[0];
	std::array<KeyPressMap, 2> pressMaps;
	KeyPressList pressList;
	std::mutex mutex_;

public:
	bool atKey(const Key_T &key) const {
		bool val;
		mutex_.lock();
		{
			val = (*this->currMap)[key];
		}
		mutex_.unlock();
		return val;
	}
	const KeyAndPress &operator [](size_t i) const {
		const KeyAndPress *key_and_press;
		mutex_.lock();
		{
			key_and_press = &(this->pressList[i]);
		}
		mutex_.unlock();
		return *key_and_press;
	}

	void press(const Key_T &key) {
		mutex_.lock();
		{
			(*this->currMap)[key] = true;
			KeyAndPress tuple = KeyAndPress{ key, true };
			/*int i = find(tuple);
			if (i < 0) */this->pressList.push(tuple);
		}
		mutex_.unlock();
	}
	void release(const Key_T &key) {
		mutex_.lock();
		{
			(*this->currMap)[key] = false;
			KeyAndPress tuple = KeyAndPress{ key, false };
			/*int i = find(tuple);
			if (i < 0) */this->pressList.push(tuple);
		}
		mutex_.unlock();
	}

	bool press_edge(const Key_T &key) {
		bool edge;
		mutex_.lock();
		{
			edge = (*this->currMap)[key] && !(*this->prevMap)[key];
		}
		mutex_.unlock();
		return edge;
	}
	bool release_edge(const Key_T &key) {
		bool edge;
		mutex_.lock();
		{
			edge = !(*this->currMap)[key] && (*this->prevMap)[key];
		}
		mutex_.unlock();
		return edge;
	}

	void clear() {
		mutex_.lock();
		{
			this->prevMap->clear();
			this->currMap->clear();
			//this->pressList.clear();
			this->pressList = KeyPressList();
		}
		mutex_.unlock();
	}

	void clear_list() {
		mutex_.lock();
		{
			this->pressList = KeyPressList();
		}
		mutex_.unlock();
	}

	bool hasNext() {
		bool has_next;
		mutex_.lock();
		{
			has_next = !pressList.empty();
		}
		mutex_.unlock();
		return has_next;
	}
	
	KeyAndPress next() {
		KeyAndPress tuple;
		mutex_.lock();
		{
			tuple = this->pressList.front();
			this->pressList.pop()
		}
		mutex_.unlock();
		return tuple;
	}

	void swap() {
		mutex_.lock();
		{
			std::swap(this->prevMap, this->currMap);
		}
		mutex_.unlock();
	}
};