#ifndef _7e9a89b0_c03a_4fd3_abcf_d9901b9b1b22_
#define _7e9a89b0_c03a_4fd3_abcf_d9901b9b1b22_

#include <unicode/normalizer2.h>

#include "Exception.h"

/*
 * This header file provides useful functions for handling UTF-8 string.
 *
 * Valid UTF-8 sequences.
 *
 * +------------------+------+------+------+------+
 * |    Code point    |byte 1|byte 2|byte 3|byte 4|
 * +------------------+------+------+------+------+
 * |U+000000..U+00007F|00..7F|      |      |      |
 * |U+000080..U+0007FF|C2..DF|80..BF|      |      |
 * |U+000800..U+000FFF|E0    |A0..BF|80..BF|      |
 * |U+001000..U+00CFFF|E1..EC|80..BF|80..BF|      |
 * |U+00D000..U+00D7FF|ED    |80..9F|80..BF|      |
 * |U+00E000..U+00FFFF|EE..EF|80..BF|80..BF|      |
 * |U+010000..U+03FFFF|F0    |90..BF|80..BF|80..BF|
 * |U+040000..U+0FFFFF|F1..F3|80..BF|80..BF|80..BF|
 * |U+100000..U+10FFFF|F4    |80..8F|80..BF|80..BF|
 * +------------------+------+------+------+------+
 *
 * This is specified by RFC-3629. The following BNF is RFC-3629 speficied.
 *
 * > UTF8-octets = *( UTF8-char )
 * > UTF8-char   = UTF8-1 / UTF8-2 / UTF8-3 / UTF8-4
 * > UTF8-1      = %x00-7F
 * > UTF8-2      = %xC2-DF UTF8-tail
 * > UTF8-3      = %xE0 %xA0-BF UTF8-tail / %xE1-EC 2( UTF8-tail ) /
 * >               %xED %x80-9F UTF8-tail / %xEE-EF 2( UTF8-tail )
 * > UTF8-4      = %xF0 %x90-BF 2( UTF8-tail ) / %xF1-F3 3( UTF8-tail ) /
 * >               %xF4 %x80-8F 2( UTF8-tail )
 * > UTF8-tail   = %x80-BF
 *
 * - RFC-3629: UTF-8, a transformation format of ISO 10646
 */

inline int getUtf8CodePointLength(const int c) {
	if (c < 0x80) return 1;
	if (c < 0x800) return 2;
	if (c < 0x10000) return 3;
	if (c < 0x200000) return 4;
	return -1;
}

inline int guessUtf8SequenceLength(const int byte1) {
#define range(start, end) (byte1 >= 0x##start && byte1 <= 0x##end)
#define eq(expected) (byte1 == 0x##expected)
#define le(expected) (byte1 <= 0x##expected)
	if (le(7F)) {
		return 1;
	} else if (range(C2, DF)) {
		return 2;
	} else if (eq(E0) || range(E1, EC) || eq(ED) || range(EE, EF)) {
		return 3;
	} else if (eq(F0) || range(F1, F3) || eq(F4)) {
		return 4;
	}
	return -1;
#undef range
#undef eq
#undef le
}

inline std::string toUtf8(const char *begin, const char *end, const char *encodingName) {
	icu::UnicodeString src(begin, end - begin, encodingName);
	UErrorCode errorCode;
	errorCode = U_ZERO_ERROR;
	const icu::Normalizer2 *n2 = icu::Normalizer2::getNFKCInstance(errorCode);
	if (U_FAILURE(errorCode)) {
		throw me::encoding_error(u_errorName(errorCode));
	}
	errorCode = U_ZERO_ERROR;
	icu::UnicodeString dest = n2->normalize(src, errorCode);
	if (U_FAILURE(errorCode)) {
		throw me::encoding_error(u_errorName(errorCode));
	}
	std::string normalized;
	dest.toUTF8String(normalized);
	return normalized;
}

#endif

