#include <boost/filesystem.hpp>
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>

#include <unicode/unistr.h>
#include <unicode/ucsdet.h>

#include "File.h"
#include "Utf8Utils.h"

File::File() :
		mFileSize(static_cast<boost::uintmax_t>(-1)),
		mIteratorBegin(NULL),
		mIteratorEnd(NULL) {
}

File::File(const char *fileName) :
		mFileName(fileName),
		mFileSize(static_cast<boost::uintmax_t>(-1)),
		mIteratorBegin(NULL),
		mIteratorEnd(NULL) {
	namespace bfs = boost::filesystem;
	namespace bip = boost::interprocess;

	const bfs::path path(fileName);
	boost::system::error_code ec;
	const bfs::file_status fs = bfs::status(path, ec);
	if (fs.type() != bfs::status_error) {
		// TODO: symlink
		const bool isFile = bfs::is_regular_file(fs);
		if (isFile) {
			const boost::uintmax_t fileSize = bfs::file_size(path, ec);
			if (fileSize != static_cast<boost::uintmax_t>(-1) && !ec) {
				mFileSize = fileSize;

				bip::file_mapping map(fileName, bip::read_only);
				bip::mapped_region view(map, bip::read_only); // TODO: lazy read
				const char *ptr = (const char *) view.get_address();
				const size_t size = view.get_size();

				UErrorCode errorCode = U_ZERO_ERROR;
				UCharsetDetector *ucsd = ucsdet_open(&errorCode);
				if (U_FAILURE(errorCode)) {
					// TODO:
				}

				errorCode = U_ZERO_ERROR;
				ucsdet_setText(ucsd, ptr, size - 1, &errorCode);
				if (U_FAILURE(errorCode)) {
					// TODO:
				}
				errorCode = U_ZERO_ERROR;
				int matchesFound;
				const UCharsetMatch **ucsm = ucsdet_detectAll(ucsd, &matchesFound, &errorCode);
				if (U_FAILURE(errorCode)) {
					// TODO:
				}

				for (int i = 0; i < matchesFound; i++) {
					Encoding encoding;
					errorCode = U_ZERO_ERROR;
					encoding.name = ucsdet_getName(ucsm[i], &errorCode);
					if (U_FAILURE(errorCode)) {
						// TODO:
					}
					errorCode = U_ZERO_ERROR;
					encoding.confidence = ucsdet_getConfidence(ucsm[i], &errorCode);
					if (U_FAILURE(errorCode)) {
						// TODO:
					}
					mFileEncodingCandidate.push_back(encoding);
				}
				assert(mFileEncodingCandidate.size() > 0);

				char *lineBegin = const_cast<char *>(ptr);
				const char *fileEnd = ptr + size;
				const char *encodingName = mFileEncodingCandidate[0].name;
				char *nextLineBegin;
				char *lineEnd;
				mFileLineFeed = Line::LineFeedDefault;
				while (lineBegin != NULL) {
					Line::LineFeed lf =
						Line::searchLineFeed(lineBegin, fileEnd, encodingName, &lineEnd, &nextLineBegin);
					if (mFileLineFeed == Line::LineFeedDefault) {
						mFileLineFeed = lf;
					}
					Line *line;
					if (lineBegin == lineEnd && lf == mFileLineFeed) {
						line = Line::blankLine();
					} else {
						std::string normalized = toUtf8(lineBegin, lineEnd, encodingName);
						if (lf == mFileLineFeed) {
							line = new Line(normalized);
						} else {
							line = new Line(normalized, lf);
						}
					}
					mLines.push_back(line);
					lineBegin = nextLineBegin;
				}
				if (mFileLineFeed == Line::LineFeedDefault) {
					// TODO: this file have no line feed
				}
			}
		}
	}
}

#if 0
// TODO: I don't know this method is usable or not.
File::File(const File& that) :
		mFileName(that.mFileName),
		mFileSize(that.mFileSize),
		mFileLineFeed(that.mFileLineFeed),
		mLines(that.mLines),
		mFileEncodingCandidate(that.mFileEncodingCandidate),
		mWithBOM(that.mWithBOM),
		mIteratorBegin(that.mIteratorBegin),
		mIteratorEnd(that.mIteratorEnd) {
}
#endif

File::~File() {
	int headSize = mLines.head_size();
	Line * const *head = mLines.head();
	for (int i = 0; i < headSize; i++) {
		const Line *line = head[i];
		if (line != Line::blankLine()) {
			delete line;
		}
	}
	int tailSize = mLines.tail_size();
	Line * const *tail = mLines.tail();
	for (int i = 0; i < tailSize; i++) {
		const Line *line = tail[i];
		if (line != Line::blankLine()) {
			delete line;
		}
	}
	delete mIteratorBegin;
	delete mIteratorEnd;
}

File::Iterator *File::begin() {
	delete mIteratorBegin;
	mIteratorBegin = Iterator::begin(this);
	return mIteratorBegin;
}
File::Iterator *File::end() {
	delete mIteratorEnd;
	mIteratorEnd = Iterator::end(this);
	return mIteratorEnd;
}

const std::string& File::getFileName() const {
	return mFileName;
}

#if 0
// TODO: I don't know this method is usable or not.
File& File::operator =(const File& that) {
	mFileName = that.mFileName;
	mFileSize = that.mFileSize;
	mFileLineFeed = that.mFileLineFeed;
	mLines = that.mLines;
	mFileEncodingCandidate = that.mFileEncodingCandidate;
	mWithBOM = that.mWithBOM;
	mIteratorBegin = that.mIteratorBegin;
	mIteratorEnd = that.mIteratorEnd;
	return *this;
}
#endif

File::Iterator::Iterator(const File::Iterator& that) :
		mFile(that.mFile),
		mLineIndex(that.mLineIndex),
		mColumnIndex(that.mColumnIndex),
		mLineFeedIndex(that.mLineFeedIndex) {
}

File::Iterator::Iterator(File *file, bool isBegin) :
		mFile(file) {
	if (isBegin) {
		mLineIndex = 0;
	} else {
		mLineIndex = file->mLines.size();
	}
	mColumnIndex = 0;
	mLineFeedIndex = -1;
}

File::Iterator& File::Iterator::operator =(const File::Iterator& that) {
	mFile = that.mFile;
	mLineIndex = that.mLineIndex;
	mColumnIndex = that.mColumnIndex;
	mLineFeedIndex = that.mLineFeedIndex;
	return *this;
}

const char& File::Iterator::operator *() const {
	if (mLineIndex == mFile->mLines.size()) {
		return *"\0";
	}
	Line *line = mFile->mLines[mLineIndex];
	if (mLineFeedIndex != -1) {
		const char *lfBytes = line->getLineFeed(*mFile);
		assert((size_t) mLineFeedIndex < sizeof(lfBytes) - 1);
		return lfBytes[mLineFeedIndex];
	}
	return (*line)[mColumnIndex];
}

File::Iterator *File::Iterator::operator ->() const {
	return const_cast<Iterator *>(this);
}

File::Iterator& File::Iterator::operator ++() {
	Line *line = mFile->mLines[mLineIndex];
	if (line->size() - 1 > mColumnIndex) {
		mColumnIndex++;
	} else {
		if (mFile->mLines.size() > mLineIndex) {
			bool needChangeLine = false;
			const char *lfBytes = line->getLineFeed(*mFile);
			int lfBytesLen = (int) sizeof(lfBytes) - 1;
			mLineFeedIndex++;
			if (mLineFeedIndex == lfBytesLen) {
				mLineFeedIndex = -1;
				needChangeLine = true;
			}
			if (needChangeLine) {
				mLineIndex++;
				mColumnIndex = 0;
			}
		}
	}
	return *this;
}

File::Iterator File::Iterator::operator ++(int) {
	Iterator old = *this;
	this->operator ++();
	return old;
}

File::Iterator& File::Iterator::operator --() {
	if (0 < mColumnIndex) {
		mColumnIndex--;
	} else {
		if (0 < mLineIndex) {
			mLineIndex--;
			Line *beforeLine = mFile->mLines[mLineIndex];
			mColumnIndex = beforeLine->size() - 1;
		}
	}
	return *this;
}

File::Iterator File::Iterator::operator --(int) {
	Iterator old = *this;
	this->operator --();
	return old;
}

#if 0
// TODO: I don't know this method is usable or not.
bool operator ==(const File& x, const File& y) {
	return x.getFileName() == y.getFileName();
}

bool operator !=(const File& x, const File& y) {
	return !(x == y);
}
#endif

bool operator ==(const File::Iterator& x, const File::Iterator& y) {
	if (x.mFile == y.mFile) {
		if (x.mLineIndex == y.mLineIndex) {
			if (x.mColumnIndex == y.mColumnIndex) {
				if (x.mLineFeedIndex == y.mLineFeedIndex) {
					return true;
				}
			}
		}
	}
	return false;
}

bool operator !=(const File::Iterator& x, const File::Iterator& y) {
	return !(x == y);
}
