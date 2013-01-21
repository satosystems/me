#ifndef _f6bb38f6_3f7c_4911_8dfe_f2333430c212_
#define _f6bb38f6_3f7c_4911_8dfe_f2333430c212_

#include <boost/regex.hpp>

#include <string>

#include "GapBuffer.h"

class File;

class Line : public GapBuffer<char> {
public:
	enum LineFeed {
		LineFeedDefault = -1,
		LineFeedCR = 0,
		LineFeedLF = 1,
		LineFeedCRLF = 2,
		LineFeedNone = 3,
	};

	static const char *LineFeedBytes[];

	Line(std::string& str, LineFeed lineFeedCode = LineFeedDefault);

#if 0
// TODO: I don't know this method is usable or not.
	Line(Line& that);
#endif

	static Line *blankLine() {
		static Line line(1);
		return &line;
	}

	static LineFeed searchLineFeed(const char *lineBegin, const char *fileEnd, const char *encodingName,
			char **lineEnd, char **nextLineBegin) {
		*nextLineBegin = NULL;
		*lineEnd = NULL;
		if (strstr(encodingName, "UTF-16") == encodingName ||
				strstr(encodingName, "UTF-32") == encodingName) {
			// TODO:
		} else {
			boost::regex re("(\\r|\\n)");
			boost::match_results<const char *> m;
			if (boost::regex_search(lineBegin, fileEnd, m, re)) {
				size_t pos = m.position();
				*lineEnd = const_cast<char *>(lineBegin) + pos;
				*nextLineBegin = *lineEnd + 1;
				if ((*lineEnd)[0] == '\r') {
					if (fileEnd > lineBegin + pos && lineBegin[pos + 1] == '\n') {
						(*nextLineBegin)++;
						return LineFeedCRLF;
					}
					return LineFeedCR;
				}
				return LineFeedLF;
			} else {
				*lineEnd = const_cast<char *>(fileEnd);
			}
		}
		return LineFeedNone;
	}

	const char *getLineFeed(File& file) const;

#if 0
// TODO: I don't know this method is usable or not.
	Line& operator =(const Line& that);
#endif

private:
	LineFeed mLineFeed;

	/*
	 * Constructor for blankLine.
	 */
	explicit Line(int capacity = 64);
};

#endif

