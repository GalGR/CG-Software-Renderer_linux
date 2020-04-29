#pragma once

#include <map>

template <typename Key_T, typename Compare_KeyVal = std::less<Key_T>>
struct ToggleMap {
	template <bool value>
	struct ToggleBool {
		bool val = value;

		ToggleBool() {};
		ToggleBool(bool val) : val(val) {}
		operator bool() const { return val; }
		ToggleBool<value> &operator =(const ToggleBool<value> &toggleBool) { return *this = toggleBool; }
		bool &operator =(bool val) { return this->val = val; }
		bool &operator =(int val) { return this->val = !!val; }

		ToggleBool<value> &toggle() { this->val = !(this->val); return *this; }
	};

	ToggleMap() = default;
	ToggleMap(const ToggleMap<Key_T> &togg_map) { this->key_bool_map = togg_map.key_bool_map; }

	typedef std::map<Key_T, ToggleBool<false>, Compare_KeyVal> KeyBoolMap_T;
	KeyBoolMap_T key_bool_map;

	ToggleBool<false> &operator [](const Key_T &key) { return this->key_bool_map[key]; }
	KeyBoolMap_T &operator =(const KeyBoolMap_T &key_bool_map) { return this->key_bool_map = key_bool_map; }

	ToggleMap<Key_T> &operator =(const ToggleMap<Key_T> &togg_map) {
		this->key_bool_map = togg_map.key_bool_map;
		return *this;
	}
};