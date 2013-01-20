#ifndef _f6bb38f6_3f7c_4911_8dfe_f2333430c212_
#define _f6bb38f6_3f7c_4911_8dfe_f2333430c212_

#include <string>

#include "GapBuffer.h"

class Line {
public:
	enum LineFeed {
		LineFeedDefault = -1,
		LineFeedNone = 0,
		LineFeedCR = 1,
		LineFeedLF = 2,
		LineFeedCRLF = 3,
	};

	Line(std::string& str, LineFeed lineFeedCode = LineFeedDefault) :
			mLineFeed(lineFeedCode) {
		const char *data = str.data();
		mLineData.insert(0, data, data + str.size());
	}

	static Line *blankLine() {
		static Line line;
		return &line;
	}

	static LineFeed searchLineFeed(const char *lineBegin, const char *fileEnd, const char *encodingName,
			char **lineEnd, char **nextLineBegin) {
		*nextLineBegin = NULL;
		*lineEnd = NULL;
		if (strstr(encodingName, "UTF-1") == encodingName ||
				strstr(encodingName, "UTF-3") == encodingName) {
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

private:
	GapBuffer<char> mLineData;
	LineFeed mLineFeed;

	/*
	 * Constructor for blankLine.
	 */
	Line() : mLineData(1), mLineFeed(LineFeedDefault) {
	}
};

#endif

