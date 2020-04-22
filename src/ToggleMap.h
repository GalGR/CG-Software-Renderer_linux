#pragma once

template <typename Key_T>
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

	typedef std::map<Key_T, ToggleBool<false>> KeyBoolMap_T;
	KeyBoolMap_T key_bool_map;

	ToggleBool<false> &operator [](const Key_T &key) { return this->key_bool_map[key]; }
	KeyBoolMap_T &operator =(const KeyBoolMap_T &key_bool_map) { return this->key_bool_map = key_bool_map; }
};