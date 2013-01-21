#ifndef _ceb4ce0c_ede3_43f2_b197_3fd0cc0aedf2_
#define _ceb4ce0c_ede3_43f2_b197_3fd0cc0aedf2_

#include <string>

#include <boost/filesystem.hpp>
#include <boost/interprocess/file_mapping.hpp>

#include "Line.h"

class File {
	friend class Line;
public:
	struct Encoding {
		const char *name;
		int32_t confidence;
	};
	class Iterator {
		friend class File;
	public:
		Iterator(const Iterator& that);
		Iterator& operator =(const Iterator& that);
		const char& operator *() const;
		File::Iterator *operator ->() const;
		Iterator& operator ++();
		Iterator operator ++(int);
		Iterator& operator --();
		Iterator operator --(int);

		File *mFile;
		int mLineIndex;
		int mColumnIndex;
		int mLineFeedIndex;
	private:
		Iterator(File *file, bool isBegin);

		static Iterator *begin(File *file) {
			return new Iterator(file, true);
		}

		static Iterator *end(File *file) {
			return new Iterator(file, false);
		}

	};
	File();
	File(const char *fileName);
#if 0
// TODO: I don't know this method is usable or not.
	File(const File& that);
#endif
	~File();

	Iterator *begin();
	Iterator *end();

	const std::string& getFileName() const;

#if 0
// TODO: I don't know this method is usable or not.
	File& operator =(const File& that);
#endif

private:
	std::string mFileName;
	boost::uintmax_t mFileSize;
	Line::LineFeed mFileLineFeed;
	GapBuffer<Line *> mLines;
	/*
	 * Candidate charactor encoding of this file.
	 */
	std::vector<Encoding> mFileEncodingCandidate;
	bool mWithBOM;
	Iterator *mIteratorBegin;
	Iterator *mIteratorEnd;
};

#if 0
// TODO: I don't know this method is usable or not.
bool operator ==(const File& x, const File& y);
bool operator !=(const File& x, const File& y);
#endif

bool operator ==(const File::Iterator& x, const File::Iterator& y);
bool operator !=(const File::Iterator& x, const File::Iterator& y);

#endif
