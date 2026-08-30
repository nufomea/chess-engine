#pragma once

#include <intrin.h>
#include <cstdint>

typedef uint64_t U64;
typedef uint8_t U8;

static inline U8 lsb_index(U64 mask) {
	unsigned long index;
	_BitScanForward64(&index, mask);
	return (U8)index;
}

static inline int pop_lsb(U64& mask) {
	unsigned long index;
	_BitScanForward64(&index, mask);
	mask &= mask - 1;
	return (int)index;
}
