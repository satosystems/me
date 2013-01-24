#ifndef _ceb4ce0c_ede3_43f2_b197_3fd0cc0aedf2_
#define _ceb4ce0c_ede3_43f2_b197_3fd0cc0aedf2_

#include <string>

#include <boost/filesystem.hpp>
#include <boost/interprocess/file_mapping.hpp>

#include "Line.h"
#include "Utf8Utils.h"

class File {
	friend class Line;
public:
	struct Encoding {
		char *name;
		int32_t confidence;
	};
	class Iterator {
		friend class File;
	public:
		Iterator(const Iterator& that);
		Iterator& operator =(const Iterator& that);
		const char& operator *() const;
		Iterator *operator ->() const;
		Iterator& operator ++();
		Iterator operator ++(int);
		Iterator& operator --();
		Iterator operator --(int);

		const File *mFile;
		int mLineIndex;
		int mColumnIndex;
		int mLineFeedIndex;
	private:
		Iterator(File *file);
	};
	File();
	File(const char *fileName);
	File(std::string fileName);
	~File();

	Iterator begin();
	Iterator end();

	const std::string& getFileName() const;

	void load();

	int getLineCount() const;

	Line *getLine(int index) const;

private:
	std::string mFileName;
	boost::uintmax_t mFileSize;
	Line::LineFeed mFileLineFeed;
	GapBuffer<Line *> mLines;
	/*
	 * Candidate charactor encoding of this file.
	 */
	std::vector<Encoding> mFileEncodingCandidate;
	Bom mBom;
	Iterator mIteratorBegin;
	Iterator mIteratorEnd;
};

bool operator ==(const File::Iterator& x, const File::Iterator& y);
bool operator !=(const File::Iterator& x, const File::Iterator& y);

#endif
