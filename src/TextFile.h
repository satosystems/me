#ifndef _775134f5_e0ee_4b00_81e2_8b7f352971f3_
#define _775134f5_e0ee_4b00_81e2_8b7f352971f3_

#include <string>

#include <boost/filesystem.hpp>
#include <boost/interprocess/file_mapping.hpp>

#include "File.h"
#include "TextLine.h"
#include "TextFileIterator.h"
#include "Utf8Utils.h"

class TextFile : public File<TextLine, TextFileIterator> {
	friend class TextLine;
	friend class TextFileIterator;
public:
	struct Encoding {
		char *name;
		int32_t confidence;
	};
	TextFile();
	TextFile(const char *fileName);
	TextFile(std::string& fileName);
	~TextFile();

	TextFileIterator begin();
	TextFileIterator end();

	void load();

private:
	TextFileIterator mIteratorBegin;
	TextFileIterator mIteratorEnd;
	TextLine::LineFeed mFileLineFeed;
	/*
	 * Candidate charactor encoding of this file.
	 */
	std::vector<Encoding> mFileEncodingCandidate;
	Bom mBom;
};

#endif

