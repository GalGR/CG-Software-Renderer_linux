#pragma once

#include <map>
#include <queue>
#include <tuple>

#include "TypeDefVal.h"

template <typename Key_T, typename Val_T, Key_T defKey, Val_T defVal, typename Compare_KeyVal = std::less<Key_T>, typename Compare_ValKey = std::less<Val_T>>
class KeyboardBind {
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
class KeyboardPress {
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

	//int find(const Key_T &key) {
	//	int listSize = (int)pressList.size();
	//	for (int i = 0; i < listSize; ++i) {
	//		if (key == std::get<0>(pressList[i])) return i;
	//	}
	//	return -1;
	//}
	//int find(std::tuple<Key_T, bool> &tuple) {
	//	int listSize = (int)pressList.size();
	//	for (int i = 0; i < listSize; ++i) {
	//		/*if (std::get<0>(tuple) == std::get<0>(this->pressList[i]) && std::get<1>(tuple) == std::get<1>(this->pressList[i])) {*/
	//		if (tuple == this->pressList.) {
	//			return i;
	//		}
	//	}
	//	return -1;
	//}

public:
	bool atKey(const Key_T &key) const { return (*this->currMap)[key]; }
	const KeyAndPress &operator [](size_t i) const { return this->pressList[i]; }

	void press(const Key_T &key) {
		(*this->currMap)[key] = true;
		KeyAndPress tuple = KeyAndPress{ key, true };
		/*int i = find(tuple);
		if (i < 0) */this->pressList.push(tuple);
	}
	void release(const Key_T &key) {
		(*this->currMap)[key] = false;
		KeyAndPress tuple = KeyAndPress{ key, false };
		/*int i = find(tuple);
		if (i < 0) */this->pressList.push(tuple);
	}

	bool press_edge(const Key_T &key) {
		return (*this->currMap)[key] && !(*this->prevMap)[key];
	}
	bool release_edge(const Key_T &key) {
		return !(*this->currMap)[key] && (*this->prevMap)[key];
	}

	void clear() {
		this->prevMap->clear();
		this->currMap->clear();
		//this->pressList.clear();
		this->pressList = KeyPressList();
	}

	void clear_list() {
		this->pressList = KeyPressList();
	}

	bool hasNext() { return !pressList.empty(); }
	
	KeyAndPress next() {
		KeyAndPress tuple = this->pressList.front();
		this->pressList.pop();
		return tuple;
	}

	void swap() {
		std::swap(this->prevMap, this->currMap);
	}
};