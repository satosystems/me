#ifndef _ab255763_1a22_451d_9f7b_98129a67bfac_
#define _ab255763_1a22_451d_9f7b_98129a67bfac_

#include <stddef.h>

#include "File.h"

class Buffer {
public:
	Buffer() : mFile(NULL), x(0), y(0) {
	}
	~Buffer() {
		delete mFile;
	}
	File *mFile;
	int x;
	int y;
};

#endif
