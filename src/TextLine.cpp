#include "TextLine.h"
#include "TextFile.h"
#include "Line.h"
#include "Logger.h"

TextLine::TextLine() :
		Line<TextFile>(NULL, 0),
		mLineFeed(LineFeedNone),
		mCharCountCache(0) {
}

TextLine::TextLine(File<TextLine, TextFileIterator> *file) :
		Line<TextFile>(dynamic_cast<TextFile *>(file)),
		mLineFeed(dynamic_cast<TextFile *>(file)->mFileLineFeed),
		mCharCountCache(-1) {
}

TextLine::TextLine(TextFile *file) :
		Line<TextFile>(file),
		mLineFeed(file->mFileLineFeed),
		mCharCountCache(-1) {
}

TextLine::TextLine(TextFile *file, std::string& str) :
		Line<TextFile>(file),
		mLineFeed(file->mFileLineFeed),
		mCharCountCache(-1) {
	const char *data = str.data();
	insert(0, data, data + str.size());
}

TextLine::TextLine(TextFile *file, std::string& str, LineFeed lineFeedCode) :
		Line<TextFile>(file),
		mLineFeed(lineFeedCode),
		mCharCountCache(-1) {
	const char *data = str.data();
	insert(0, data, data + str.size());
}

void TextLine::clear() {
	mCharCountCache = -1;
	GapBuffer<char> *super = this;
	super->clear();
}

int TextLine::insert(int pos, const char val) {
	mCharCountCache = -1;
	GapBuffer<char> *super = this;
	return super->insert(pos, val);
}

int TextLine::insert(int pos, int num, const char val) {
	mCharCountCache = -1;
	GapBuffer<char> *super = this;
	return super->insert(pos, num, val);
}

template<class InputIterator>
int TextLine::insert(int pos, InputIterator begin, InputIterator end) {
	mCharCountCache = -1;
	GapBuffer<char> *super = this;
	return super->insert(pos, begin, end);
}

void TextLine::erase(int begin, int end) {
	mCharCountCache = -1;
	GapBuffer<char> *super = this;
	super->erase(begin, end);
}

void TextLine::erase(int pos) {
	mCharCountCache = -1;
	GapBuffer<char> *super = this;
	super->erase(pos);
}

void TextLine::push_back(const char val) {
	mCharCountCache = -1;
	GapBuffer<char> *super = this;
	super->push_back(val);
}

const char *TextLine::getLineFeed() const {
	Logger::d("mLineFeed:%s", mLineFeed == LineFeedNone ?
			"LineFeedNone" : mLineFeed  == LineFeedCR ?
			"LineFeedCR" : mLineFeed  == LineFeedLF ?
			"LineFeedLF" : mLineFeed  == LineFeedCRLF ?
			"LineFeedCRLF" : "ERROR!!!");
	if (mLineFeed == LineFeedNone) {
		return NULL;
	}

	const char *encodingName;
	if (mFile->mFileEncodingCandidate.size() == 0) {
		encodingName = "UTF-8";
	} else {
		encodingName = mFile->mFileEncodingCandidate[0].name;
	}
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
	return LineFeedBytes[mLineFeed + offset];
}

int TextLine::getCharCount() {
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

const char *TextLine::LineFeedBytes[] = {
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
