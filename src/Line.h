#ifndef _f6bb38f6_3f7c_4911_8dfe_f2333430c212_
#define _f6bb38f6_3f7c_4911_8dfe_f2333430c212_

#include <string>

#include "GapBuffer.h"

template<class T>
class Line : public GapBuffer<char> {
public:
	Line(T *file, int capacity = 64) :
			GapBuffer<char>(capacity),
			mFile(file) {
	}
protected:
	T *mFile;
};

#endif

