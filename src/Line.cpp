#include "Line.h"
#include "File.h"

Line::Line(std::string& str, LineFeed lineFeedCode) :
		GapBuffer<char>(),
		mLineFeed(lineFeedCode),
		mCharCountCache(-1) {
	const char *data = str.data();
	insert(0, data, data + str.size());
}

Line::Line() :
		GapBuffer<char>(1),
		mLineFeed(LineFeedDefault),
		mCharCountCache(0) {
}

void Line::clear() {
	mCharCountCache = -1;
	GapBuffer<char> *super = this;
	super->clear();

}

int Line::insert(int pos, const char val) {
	mCharCountCache = -1;
	GapBuffer<char> *super = this;
	return super->insert(pos, val);
}

int Line::insert(int pos, int num, const char val) {
	mCharCountCache = -1;
	GapBuffer<char> *super = this;
	return super->insert(pos, num, val);
}

template<class InputIterator>
int Line::insert(int pos, InputIterator begin, InputIterator end) {
	mCharCountCache = -1;
	GapBuffer<char> *super = this;
	return super->insert(pos, begin, end);
}

void Line::erase(int begin, int end) {
	mCharCountCache = -1;
	GapBuffer<char> *super = this;
	super->erase(begin, end);
}

void Line::erase(int pos) {
	mCharCountCache = -1;
	GapBuffer<char> *super = this;
	super->erase(pos);
}

void Line::push_back(const char val) {
	mCharCountCache = -1;
	GapBuffer<char> *super = this;
	super->push_back(val);
}

const char *Line::getLineFeed(const File& file) const {
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

int Line::getCharCount() {
	if (mCharCountCache != -1) {
		return mCharCountCache;
	}
	int len = size();
	int count = 0;
	for (int i = 0; i < len; ) {
		int sl = guessUtf8SequenceLength((*this)[i]);
		if (sl == -1) {
			sl = 1;
		}
		i += sl;
		count++;
	}
	mCharCountCache = count;
	return count;
}

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
