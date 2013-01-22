#include <boost/filesystem.hpp>
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>

#include <unicode/unistr.h>
#include <unicode/ucsdet.h>

#include "File.h"
#include "Utf8Utils.h"

static char gUtf16BE[] = "UTF-16BE";
static char gUtf16LE[] = "UTF-16LE";
static char gUtf32BE[] = "UTF-32BE";
static char gUtf32LE[] = "UTF-32LE";

File::File() :
		mFileSize(static_cast<boost::uintmax_t>(-1)),
		mBom(BomNone),
		mIteratorBegin(NULL),
		mIteratorEnd(NULL) {
	// TODO: read default encoding and line feed from properties file.
}

File::File(const char *fileName) :
		mFileName(fileName),
		mFileSize(static_cast<boost::uintmax_t>(-1)),
		mBom(BomNone),
		mIteratorBegin(NULL),
		mIteratorEnd(NULL) {
	// TODO: read default encoding and line feed from properties file.
}

File::File(std::string fileName) :
		mFileName(fileName),
		mFileSize(static_cast<boost::uintmax_t>(-1)),
		mBom(BomNone),
		mIteratorBegin(NULL),
		mIteratorEnd(NULL) {
	// TODO: read default encoding and line feed from properties file.
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

void File::load() {
	namespace bfs = boost::filesystem;
	namespace bip = boost::interprocess;

	if (mFileName.empty()) {
		return;
	}
	const bfs::path path(mFileName);
	boost::system::error_code ec;
	const bfs::file_status fs = bfs::status(path, ec);
	if (fs.type() != bfs::status_error) {
		// TODO: symlink
		// TODO: directory
		const bool isFile = bfs::is_regular_file(fs);
		if (isFile) {
			const boost::uintmax_t fileSize = bfs::file_size(path, ec);
			if (fileSize != static_cast<boost::uintmax_t>(-1) && !ec) {
				mFileSize = fileSize;

				bip::file_mapping map(mFileName.c_str(), bip::read_only);
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
					encoding.name = const_cast<char *>(ucsdet_getName(ucsm[i], &errorCode));
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
				// BOM and byte order check
				char *encodingName = mFileEncodingCandidate[0].name;
				char **encodingRename = &mFileEncodingCandidate[0].name;
				if (size >= 2 && strstr(encodingName, "UTF-16") == encodingName) {
					assert(fileSize % 2 == 0);
					if (lineBegin[0] == 0xFE && lineBegin[1] == 0xFF) {
						mBom = BomBE;
						assert(strcmp(encodingName, "UTF-16BE") == 0);
						lineBegin += 2;
					} else if (lineBegin[0] == 0xFF && lineBegin[1] == 0xFE) {
						mBom = BomLE;
						assert(strcmp(encodingName, "UTF-16LE") == 0);
						lineBegin += 2;
					} else {
						for (boost::uintmax_t i = 0; i < fileSize; i += 2) {
							if (lineBegin[i] == 0x00 && lineBegin[i + 1] != 0x00) {
								mBom = BomBE;
								*encodingRename = gUtf16BE;
								break;
							}
							if (lineBegin[i] != 0x00 && lineBegin[i + 1] == 0x00) {
								mBom = BomLE;
								*encodingRename = gUtf16LE;
								break;
							}
						}
					}
				} else if (size >= 3 && strstr(encodingName, "UTF-8") == encodingName) {
					if (lineBegin[0] == 0xEF && lineBegin[1] == 0xBB && lineBegin[2] == 0xBF) {
						mBom = BomZWNBSP;
						lineBegin += 3;
					}
				} else if (size > 4 && strstr(encodingName, "UTF-32") == encodingName) {
					assert(fileSize % 4 == 0);
					if (lineBegin[0] == 0x00 && lineBegin[1] == 0x00 &&
							lineBegin[2] == 0xFE && lineBegin[3] == 0xFF) {
						mBom = BomBE;
						assert(strcmp(encodingName, "UTF-32BE") == 0);
						lineBegin += 4;
					} else if (lineBegin[0] == 0xFF && lineBegin[1] == 0xFE &&
							lineBegin[2] == 0x00 && lineBegin[3] == 0x00) {
						mBom = BomLE;
						assert(strcmp(encodingName, "UTF-32LE") == 0);
						lineBegin += 4;
					} else {
						for (boost::uintmax_t i = 0; i < fileSize; i += 4) {
							if (lineBegin[i] == 0x00 && lineBegin[i + 1] == 0x00 &&
									lineBegin[i + 2] != 0x00 && lineBegin[i + 3] != 0x00) {
								mBom = BomBE;
								*encodingRename = gUtf32BE;
								break;
							}
							if (lineBegin[i] != 0x00 && lineBegin[i + 1] != 0x00 &&
									lineBegin[i + 2] == 0x00 && lineBegin[i + 3] == 0x00) {
								mBom = BomLE;
								*encodingRename = gUtf32LE;
								break;
							}
						}
					}
				}

				const char *fileEnd = ptr + size;
				mFileLineFeed = Line::LineFeedDefault;
				while (lineBegin != NULL) {
					char *nextLineBegin = NULL;
					char *lineEnd = NULL;
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

int File::getLineCount() const {
	return mLines.size();
}

Line *File::getLine(int index) const {
	if (mLines.size() > index) {
		return mLines[index];
	}
	return NULL;
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
