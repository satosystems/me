#include "Line.h"
#include "File.h"

Line::Line(std::string& str, LineFeed lineFeedCode) :
		GapBuffer<char>(),
		mLineFeed(lineFeedCode) {
	const char *data = str.data();
	insert(0, data, data + str.size());
}

#if 0
// TODO: I don't know this method is usable or not.
Line::Line(Line& that) :
		GapBuffer<char>(that.GAP_GROWTH_SIZE),
		mLineFeed(that.mLineFeed) {
	mCapacity = that.mCapacity;
	mGapBegin = that.mGapBegin;
	mGapEnd = that.mGapEnd;
	mBuffer = (char *) realloc(mBuffer, mCapacity);
	if (mBuffer == NULL) {
		throw std::bad_alloc();
	}
	memcpy(mBuffer, that.mBuffer, mCapacity);
}
#endif

Line::Line(int capacity) :
		GapBuffer<char>(capacity),
		mLineFeed(LineFeedDefault) {
}

const char *Line::getLineFeed(File& file) const {
	LineFeed lf = mLineFeed == LineFeedDefault ? file.mFileLineFeed : mLineFeed;
	if (lf == LineFeedNone) {
		return "";
	}
	const char *encodingName = file.mFileEncodingCandidate[0].name;
	int offset = 0;
	if (strstr(encodingName, "UTF-16") == encodingName) {
		if (strstr(encodingName, "BE") != NULL) {
			offset = 3;
		} else if (strstr(encodingName, "LE") != NULL) {
			offset = 6;
		} else {
			// TODO: need detect endian of file
		}
	} else if (strstr(encodingName, "UTF-32") == encodingName) {
		if (strstr(encodingName, "BE") != NULL) {
			offset = 9;
		} else if (strstr(encodingName, "LE") != NULL) {
			offset = 12;
		} else {
			// TODO: need detect endian of file
		}
	}
	return LineFeedBytes[lf + offset];
}

#if 0
// TODO: I don't know this method is usable or not.
Line& Line::operator =(const Line& that) {
	mLineFeed = that.mLineFeed;
	return *this;
}
#endif

const char *Line::LineFeedBytes[] = {
	// UTF-8 and multibyte encodings
	"\r", "\n", "\r\n",
	// UTF-16BE
	"\0\r", "\0\n", "\0\r\0\n",
	// UTF-16LE
	"\r\0", "\n\0", "\r\0\n\0",
	// UTF-32BE
	"\0\0\0\r", "\0\0\0\n", "\0\0\0\r\0\0\0\n",
	// UTF-32LE
	"\r\0\0\0", "\n\0\0\0", "\r\0\0\0\n\0\0\0",
};
