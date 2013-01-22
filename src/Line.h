#ifndef _f6bb38f6_3f7c_4911_8dfe_f2333430c212_
#define _f6bb38f6_3f7c_4911_8dfe_f2333430c212_

#include <string.h>

#include <string>

#include "GapBuffer.h"
#include "Utf8Utils.h"

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
		if (strstr(encodingName, "UTF-16BE") == encodingName) {
			for (char *cp = const_cast<char *>(lineBegin); cp < fileEnd ; cp += 2) {
				if (cp[0] == 0x00 && cp[1] == '\r') {
					*lineEnd = cp;
					*nextLineBegin = *lineEnd + 2;
					if (cp + 2 < fileEnd && cp[2] == 0x00 && cp[3] == '\n') {
						*nextLineBegin += 2;
						return LineFeedCRLF;
					}
					return LineFeedCR;
				}
				if (cp[0] == 0x00 && cp[1] == '\n') {
					*lineEnd = cp;
					*nextLineBegin = *lineEnd + 2;
					return LineFeedLF;
				}
			}
			*lineEnd = const_cast<char *>(fileEnd);
		} else if (strstr(encodingName, "UTF-16LE") == encodingName) {
			for (char *cp = const_cast<char *>(lineBegin); cp < fileEnd ; cp += 2) {
				if (cp[0] == 'r' && cp[1] == 0x00) {
					*lineEnd = cp;
					*nextLineBegin = *lineEnd + 2;
					if (cp + 2 < fileEnd && cp[2] == '\n' && cp[3] == 0x00) {
						*nextLineBegin += 2;
						return LineFeedCRLF;
					}
					return LineFeedCR;
				}
				if (cp[0] == '\n' && cp[1] == 0x00) {
					*lineEnd = cp;
					*nextLineBegin = *lineEnd + 2;
					return LineFeedLF;
				}
			}
			*lineEnd = const_cast<char *>(fileEnd);
		} else if (strstr(encodingName, "UTF-32BE") == encodingName) {
			for (char *cp = const_cast<char *>(lineBegin); cp < fileEnd ; cp += 4) {
				if (cp[0] == 0x00 && cp[1] == 0x00 && cp[2] == 0x00 && cp[3] == '\r') {
					*lineEnd = cp;
					*nextLineBegin = *lineEnd + 4;
					if (cp + 4 < fileEnd &&
							cp[4] == 0x00 && cp[5] == 0x00 && cp[6] == 0x00 && cp[7] == '\n') {
						*nextLineBegin += 4;
						return LineFeedCRLF;
					}
					return LineFeedCR;
				}
				if (cp[0] == 0x00 && cp[1] == 0x00 && cp[2] == 0x00 && cp[3] == '\n') {
					*lineEnd = cp;
					*nextLineBegin = *lineEnd + 4;
					return LineFeedLF;
				}
			}
			*lineEnd = const_cast<char *>(fileEnd);
		} else if (strstr(encodingName, "UTF-32LE") == encodingName) {
			for (char *cp = const_cast<char *>(lineBegin); cp < fileEnd ; cp += 4) {
				if (cp[0] == '\r' && cp[1] == 0x00 && cp[2] == 0x00 && cp[3] == 0x00) {
					*lineEnd = cp;
					*nextLineBegin = *lineEnd + 4;
					if (cp + 4 < fileEnd &&
							cp[4] == '\n' && cp[5] == 0x00 && cp[6] == 0x00 && cp[7] == 0x00) {
						*nextLineBegin += 4;
						return LineFeedCRLF;
					}
					return LineFeedCR;
				}
				if (cp[0] == '\n' && cp[1] == 0x00 && cp[2] == 0x00 && cp[3] == 0x00) {
					*lineEnd = cp;
					*nextLineBegin = *lineEnd + 4;
					return LineFeedLF;
				}
			}
			*lineEnd = const_cast<char *>(fileEnd);
		} else {
			for (char *cp = const_cast<char *>(lineBegin); cp < fileEnd; cp++) {
				if (cp[0] == '\r') {
					*lineEnd = cp;
					*nextLineBegin = *lineEnd + 1;
					if (cp + 1  < fileEnd && cp[1] == '\n') {
						*nextLineBegin += 1;
						return LineFeedCRLF;
					}
					return LineFeedCR;
				}
				if (cp[0] == '\n') {
					*lineEnd = cp;
					*nextLineBegin = *lineEnd + 1;
					return LineFeedLF;
				}
			}
			*lineEnd = const_cast<char *>(fileEnd);
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

