#ifndef _ceb4ce0c_ede3_43f2_b197_3fd0cc0aedf2_
#define _ceb4ce0c_ede3_43f2_b197_3fd0cc0aedf2_

#include <string>

#include <boost/filesystem.hpp>
#include <boost/interprocess/file_mapping.hpp>

#include "Line.h"

class File {
public:
	struct Encoding {
		const char *name;
		int32_t confidence;
	};
	File();
	File(const char * const fileName);
	~File();

private:
	std::string mFileName;
	boost::uintmax_t mFileSize;
	Line::LineFeedCode mFileLineFeed;
	GapBuffer<Line *> mLines;
	/*
	 * Candidate charactor encoding of this file.
	 */
	std::vector<Encoding> mFileEncodingCandidate;
	bool mWithBOM;
};

#endif

