#ifndef _f6bb38f6_3f7c_4911_8dfe_f2333430c212_
#define _f6bb38f6_3f7c_4911_8dfe_f2333430c212_

#include <string>

#include "GapBuffer.h"

class Line : public GapBuffer<char> {
public:
	Line(int capacity = 64) :
			GapBuffer<char>(capacity) {
	}
};

#endif

