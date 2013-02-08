#ifndef _3979b74d_ab83_43e3_994e_175c3f6fe3a7_
#define _3979b74d_ab83_43e3_994e_175c3f6fe3a7_

#include "Line.h"
#include "GapBuffer.h"
#include "File.h"
#include "TextFileIterator.h"

class TextFile;

class TextLine : public Line<TextFile> {
public:
	enum LineFeed {
		LineFeedCR = 0,
		LineFeedLF = 1,
		LineFeedCRLF = 2,
		LineFeedNone = 3,
	};

	static const char *LineFeedBytes[];

	TextLine(File<TextLine, TextFileIterator> *file);

	TextLine(TextFile *file);

	TextLine(TextFile *file, std::string& str);

	TextLine(TextFile *file, std::string& str, LineFeed lineFeedCode);

	static TextLine *blankLine() {
		static TextLine line;
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

	bool isBlankLine() const;

	void clear();

	int insert(int pos, const char val);

	int insert(int pos, int num, const char val);

	template<class InputIterator>
	int insert(int pos, InputIterator begin, InputIterator end);

	void erase(int begin, int end);

	void erase(int pos);

	void push_back(const char val);

	const char *getLineFeed() const;

	int getCharCount();

private:
	LineFeed mLineFeed;
	int mCharCountCache;

	/*
	 * Constructor for blankLine.
	 */
	TextLine();
};

#endif

