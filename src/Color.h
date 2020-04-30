#pragma once

#include "basetsd.h"

enum COLORS_ENUMS {
	WHITE	= (UINT32)0xff'ff'ff'ff,
	RED		= (UINT32)0xff'00'00'ff,
	GREEN	= (UINT32)0xff'00'ff'00,
	BLUE	= (UINT32)0xff'ff'00'00,
	BLACK	= (UINT32)0xff'00'00'00,
	YELLOW	= (UINT32)0xff'00'ff'ff,
	MAGENTA	= (UINT32)0xff'ff'00'ff,
	CYAN	= (UINT32)0xff'ff'ff'00,
};

union Color {
	UINT32 data;
	struct {
		UINT8 r;
		UINT8 g;
		UINT8 b;
		UINT8 a;
	};

	inline Color() = default;
	inline Color(UINT32 data) : data(data) {}
	inline Color(COLORS_ENUMS data) : data((UINT32)data) {}
	inline Color& operator=(UINT32 data) { this->data = data; return *this; }
	inline operator UINT32() const { return data; }

	inline const UINT8 &operator[](int i) const { return (&r)[i]; }
	inline UINT8 &operator[](int i) { return (&r)[i]; }
};
