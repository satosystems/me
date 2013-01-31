#ifndef _ceb4ce0c_ede3_43f2_b197_3fd0cc0aedf2_
#define _ceb4ce0c_ede3_43f2_b197_3fd0cc0aedf2_

#include <string>

#include <boost/filesystem.hpp>

#include "GapBuffer.h"

template<class T, class BidirectionalIterator>
class File {
public:
	File() :
			mFileName(),
			mFileSize(static_cast<boost::uintmax_t>(-1)),
			mLines(1) {
	}

	File(const char *fileName) :
			mFileName(fileName),
			mFileSize(static_cast<boost::uintmax_t>(-1)),
			mLines() {
	}

	File(std::string fileName) :
		mFileName(fileName),
		mFileSize(static_cast<boost::uintmax_t>(-1)),
		mLines() {
	}

	virtual ~File() = 0;

	virtual BidirectionalIterator begin() = 0;
	virtual BidirectionalIterator end() = 0;

	virtual void load() = 0;

	const std::string& getFileName() const {
		return mFileName;
	}

	int getLineCount() const {
		return mLines.size();
	}

	T *getLine(int index) const {
		if (mLines.size() > index) {
			return mLines[index];
		}
		return NULL;
	}

	boost::uintmax_t getFileSize() const {
		return mFileSize;
	}

protected:
	/*
	 * Name of this file.
	 * It is new created file if file name is empty.
	 */
	std::string mFileName;
	/*
	 * The value is -1 if this instance has not been load file.
	 */
	boost::uintmax_t mFileSize;
	GapBuffer<T *> mLines;
};

template<class T, class BidirectionalIterator>
File<T, BidirectionalIterator>::~File<T, BidirectionalIterator>() {
}

#endif

