//
//  Created by Matt Hartley on 21/03/2025.
//  Copyright 2025 GiantJelly. All rights reserved.
//

#ifndef __CORE_IMATH_HEADER__
#define __CORE_IMATH_HEADER__


#include <stdint.h>
#include <stdlib.h>


int idiv10(int input);
int ilog10(int input);


#endif


#ifdef CORE_IMPL
#	ifndef __CORE_IMATH_HEADER_IMPL__
#	define __CORE_IMATH_HEADER_IMPL__


// Max 32bit: 4,294,967,295
// Max 64bit: 18,446,744,073,709,551,615
int _log2to10_tbl[] = {
	0,0,0,0,1,1,1,2,2,2,3,3,3,3,
	4,4,4,5,5,5,6,6,6,6,7,7,7,8,8,8,9,9,
};
uint64_t _base10_tbl[] = {
	1,10,100,1000,10000,100000,1000000,10000000,100000000,1000000000,
	10000000000,100000000000,1000000000000,10000000000000,
	100000000000000,1000000000000000,10000000000000000,
	100000000000000000,1000000000000000000,//10000000000000000000,
};

int idiv10(int input) {
	uint32_t n = abs(input);
	uint64_t tmp = (uint64_t)n * 0xCCCCCCCD;
	uint32_t q = tmp >> 35;

	// uint32_t signMask = input & 0x80000000;
	// uint32_t asd = q | signMask;
	// int result = asd;
	// return result;

	int result = q;
	if (input & 0x80000000) {
		result = 0 - result;
	}
	return result;
}

int imod10(int input) {
	int div = idiv10(input) * 10;
	return input - div;
}

int ilog10(int input) {
	int value = abs(input);
	int base2 = 31 - __builtin_clz(value | 1);
	int base10TblIndex = _log2to10_tbl[base2] + 1;
	if (value < _base10_tbl[base10TblIndex]) {
		--base10TblIndex;
	}
	int base10 = _base10_tbl[base10TblIndex];
	if (input & 0x80000000) {
		// base10 = 0 - base10;
		base10TblIndex = 0 - base10TblIndex;
	}
	return base10TblIndex;
}


#	endif
#endif
