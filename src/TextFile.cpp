#include <boost/filesystem.hpp>
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>

#include <unicode/unistr.h>
#include <unicode/ucsdet.h>

#include <iostream>
#include <fstream>

#include "TextFile.h"
#include "TextLine.h"
#include "Utf8Utils.h"
#include "Settings.h"
#include "Logger.h"

TextFile::TextFile() :
		File<TextLine, TextFileIterator>(),
		mFileLineFeed(gSettings->getDefaultLineFeed()),
		mBom(BomNone) {
	mIteratorBegin.mFile = this;
	mIteratorEnd.mFile = this;
}

TextFile::TextFile(const char *fileName) :
		File<TextLine, TextFileIterator>(fileName),
		mFileLineFeed(gSettings->getDefaultLineFeed()),
		mBom(BomNone) {
	mIteratorBegin.mFile = this;
	mIteratorEnd.mFile = this;
}

TextFile::TextFile(const std::string& fileName) :
		File<TextLine, TextFileIterator>(fileName),
		mFileLineFeed(gSettings->getDefaultLineFeed()),
		mBom(BomNone) {
	mIteratorBegin.mFile = this;
	mIteratorEnd.mFile = this;
}

TextFile::~TextFile() {
	int headSize = mLines.head_size();
	TextLine * const *head = mLines.head();
	for (int i = 0; i < headSize; i++) {
		const TextLine *line = head[i];
		if (line != TextLine::blankLine()) {
			delete line;
		}
	}
	int tailSize = mLines.tail_size();
	TextLine * const *tail = mLines.tail();
	for (int i = 0; i < tailSize; i++) {
		const TextLine *line = tail[i];
		if (line != TextLine::blankLine()) {
			delete line;
		}
	}
}

TextFileIterator TextFile::begin() {
	mIteratorBegin.mLineIndex = 0;
	mIteratorBegin.mColumnIndex = 0;
	mIteratorBegin.mLineFeedIndex = -1;
	return mIteratorBegin;
}

TextFileIterator TextFile::end() {
	mIteratorBegin.mLineIndex = mLines.size();
	mIteratorBegin.mColumnIndex = 0;
	mIteratorBegin.mLineFeedIndex = -1;
	return mIteratorEnd;
}

void TextFile::load() {
	Logger::d("load:[%s]", mFileName.c_str());
	namespace bfs = boost::filesystem;
	namespace bip = boost::interprocess;

	if (mFileName.empty()) {
		return;
	}
	const bfs::path path(mFileName);
	boost::system::error_code ec;
	const bfs::file_status fs(bfs::status(path, ec));
	if (fs.type() == bfs::regular_file || fs.type() == bfs::symlink_file) {
		const boost::uintmax_t fileSize = bfs::file_size(path, ec);
		Logger::d("ec:%s", ec ? "error" : "success");
		Logger::d("fileSize:%zd", fileSize);
		if (fileSize != static_cast<boost::uintmax_t>(-1) && !ec) {
			mFileSize = fileSize;

			bip::file_mapping map(mFileName.c_str(), bip::read_only);
			bip::mapped_region view(map, bip::read_only); // TODO: lazy read
			const char *ptr = (const char *) view.get_address();
			const size_t size = view.get_size();

			UErrorCode errorCode = U_ZERO_ERROR;
			UCharsetDetector *ucsd = ucsdet_open(&errorCode);
			if (U_FAILURE(errorCode)) {
				// TODO: error recovery
			}

			errorCode = U_ZERO_ERROR;
			ucsdet_setText(ucsd, ptr, size - 1, &errorCode);
			if (U_FAILURE(errorCode)) {
				// TODO: error recovery
			}
			errorCode = U_ZERO_ERROR;
			int matchesFound;
			const UCharsetMatch **ucsm = ucsdet_detectAll(ucsd, &matchesFound, &errorCode);
			if (U_FAILURE(errorCode)) {
				// TODO: error recovery
			}

			for (int i = 0; i < matchesFound; i++) {
				Encoding encoding;
				errorCode = U_ZERO_ERROR;
				encoding.name = ucsdet_getName(ucsm[i], &errorCode);
				if (U_FAILURE(errorCode)) {
					// TODO: error recovery
				}
				errorCode = U_ZERO_ERROR;
				encoding.confidence = ucsdet_getConfidence(ucsm[i], &errorCode);
				if (U_FAILURE(errorCode)) {
					// TODO: error recovery
				}
				mFileEncodingCandidate.push_back(encoding);
			}
			assert(mFileEncodingCandidate.size() > 0);

			char *lineBegin = const_cast<char *>(ptr);
			// BOM and byte order check
			const char *encodingName = mFileEncodingCandidate[0].name;
			const char **encodingRename = &mFileEncodingCandidate[0].name;
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
							*encodingRename = "UTF-16BE";
							break;
						}
						if (lineBegin[i] != 0x00 && lineBegin[i + 1] == 0x00) {
							mBom = BomLE;
							*encodingRename = "UTF-16LE";
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
							*encodingRename = "UTF-32BE";
							break;
						}
						if (lineBegin[i] != 0x00 && lineBegin[i + 1] != 0x00 &&
								lineBegin[i + 2] == 0x00 && lineBegin[i + 3] == 0x00) {
							mBom = BomLE;
							*encodingRename = "UTF-32LE";
							break;
						}
					}
				}
			}

			const char *fileEnd = ptr + size;
			char *nextLineBegin = NULL;
			char *lineEnd = NULL;
			mFileLineFeed = TextLine::LineFeedNone;
			while (nextLineBegin != fileEnd || lineEnd == fileEnd) {
				TextLine::LineFeed lf =
					TextLine::searchLineFeed(lineBegin, fileEnd, encodingName, &lineEnd, &nextLineBegin);
				if (mFileLineFeed == TextLine::LineFeedNone) {
					mFileLineFeed = lf;
				}
				TextLine *line;
				if (lineBegin == lineEnd && lf == mFileLineFeed) {
					line = TextLine::blankLine();
				} else {
					std::string normalized = toUtf8(lineBegin, lineEnd, encodingName);
					line = new TextLine(this, normalized, lf);
				}
				mLines.push_back(line);
				lineBegin = nextLineBegin;
			}
			if (mFileLineFeed == TextLine::LineFeedNone) {
				mFileLineFeed = gSettings->getDefaultLineFeed();
			}
		}
	}
}

void TextFile::save() {
	std::ofstream ofs(mFileName.c_str());
	for (int i = 0; i < mLines.size(); i++) {
		// TODO: convert file encoding
		int size;
		size = mLines[i]->head_size();
		if (size > 0) {
			ofs.write((char *) mLines[i]->head(), size);
		}
		size = mLines[i]->tail_size();
		if (size > 0) {
			ofs.write((char *) mLines[i]->tail(), size);
		}
		const char *lineEnd = mLines[i]->getLineFeed();
		if (lineEnd != NULL) {
			size = sizeof(lineEnd);
			Logger::d("lineEndSize:%d", size);
			ofs.write(lineEnd, size);
		}
	}
	ofs.close();
}
