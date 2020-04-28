#pragma once

#if defined(__GNUC__) || defined(__clang__) || defined(__MINGW32__) || defined(__MINGW64__)
#define PACK(STRUCT_) STRUCT_ __attribute__((packed))
#elif defined(_MSC_VER)
#define PACK(STRUCT_) __pragma(pack(push, 1)) STRUCT_ __pragma(pack(pop))
#endif