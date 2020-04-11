#pragma once

template <typename Action_T>
struct ActionsDone {
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

	typedef std::map<Action_T, ToggleBool<false>> ActionsDoneMap_T;
	ActionsDoneMap_T actions_done_map;

	ToggleBool<false> &operator [](const Action_T &action) { return this->actions_done_map[action]; }
	ActionsDoneMap_T &operator =(const ActionsDoneMap_T &actions_done_map) { return this->actions_done_map = actions_done_map; }
};