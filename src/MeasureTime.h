#pragma once

#if defined(_WIN32)
#include <Profileapi.h>
#elif defined(__linux__)
#include <stdint.h>

typedef uint8_t BYTE;
typedef uint32_t DWORD;
typedef int32_t LONG;
typedef int64_t LONGLONG;

// 64 bit integer type
typedef union _LARGE_INTEGER {
	struct {
		DWORD LowPart;
		LONG HighPart;
	};
	struct {
		DWORD LowPart;
		LONG HighPart;
	} u;
	LONGLONG QuadPart;
} LARGE_INTEGER, *PLARGE_INTEGER;

#include <time.h>

void QueryPerformanceFrequency(LARGE_INTEGER *lpFrequency) {
	lpFrequency->QuadPart = 1'000'000'000;
}

void QueryPerformanceCounter(LARGE_INTEGER *lpPerformanceCount) {
	struct timespec elapsed_time;
	clock_gettime(CLOCK_MONOTONIC_RAW, &elapsed_time);
	lpPerformanceCount->QuadPart = (LONGLONG)elapsed_time.tv_nsec;
}

#endif