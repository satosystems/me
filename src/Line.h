#ifndef _f6bb38f6_3f7c_4911_8dfe_f2333430c212_
#define _f6bb38f6_3f7c_4911_8dfe_f2333430c212_

#include <string>

#include "GapBuffer.h"

class Line {
public:
	enum LineFeedCode {
		LineFeedCodeDefault = -1,
		LineFeedCodeNone = 0,
		LineFeedCodeCR = 1,
		LineFeedCodeLF = 2,
		LineFeedCodeCRLF = 3,
	};

	Line(std::string& str, LineFeedCode lineFeedCode = LineFeedCodeDefault) :
			mLineFeedCode(lineFeedCode) {
		const char *data = str.data();
		mLineData.insert(0, data, data + str.size());
	}

	static Line *blankLine() {
		return &mBlankLine;
	}

	static LineFeedCode searchLineFeedCode(const char *lineBegin, const char *fileEnd, const char *encodingName,
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
						return LineFeedCodeCRLF;
					}
					return LineFeedCodeCR;
				}
				return LineFeedCodeLF;
			} else {
				*lineEnd = const_cast<char *>(fileEnd);
			}
		}
		return LineFeedCodeNone;
	}

private:
	GapBuffer<char> mLineData;
	LineFeedCode mLineFeedCode;

	/*
	 * Constructor for blankLine.
	 */
	Line() : mLineData(1), mLineFeedCode(LineFeedCodeDefault) {
	}

	static Line mBlankLine;
};

#endif

