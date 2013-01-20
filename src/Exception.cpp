#include <string.h>
#include <stdlib.h>

#include "Exception.h"

me::encoding_error::encoding_error() {
	mDescription = NULL;
}

me::encoding_error::encoding_error(const char *description) {
	mDescription = strdup(description);
}

me::encoding_error::~encoding_error() throw () {
	free(mDescription);
}

const char* me::encoding_error::what() const throw () {
	return mDescription;
}

