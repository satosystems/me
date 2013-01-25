#include <assert.h>

#include <unicode/unistr.h>
#include <unicode/ucsdet.h>

#include "TextFileIterator.h"
#include "TextLine.h"
#include "TextFile.h"
#include "Utf8Utils.h"

TextFileIterator::TextFileIterator() {
}

TextFileIterator::TextFileIterator(const TextFileIterator& that) :
		mFile(that.mFile),
		mLineIndex(that.mLineIndex),
		mColumnIndex(that.mColumnIndex),
		mLineFeedIndex(that.mLineFeedIndex) {
}

TextFileIterator::~TextFileIterator() {
}

TextFileIterator& TextFileIterator::operator =(const TextFileIterator& that) {
	mFile = that.mFile;
	mLineIndex = that.mLineIndex;
	mColumnIndex = that.mColumnIndex;
	mLineFeedIndex = that.mLineFeedIndex;
	return *this;
}

const char& TextFileIterator::operator *() const {
	if (mLineIndex == mFile->getLineCount()) {
		return *"\0";
	}
	TextLine *line = mFile->mLines[mLineIndex];
	if (mLineFeedIndex != -1) {
		const char *lfBytes = line->getLineFeed();
		assert((size_t) mLineFeedIndex < sizeof(lfBytes) - 1);
		return lfBytes[mLineFeedIndex];
	}
	return (*line)[mColumnIndex];
}

TextFileIterator *TextFileIterator::operator ->() const {
	return const_cast<TextFileIterator *>(this);
}

TextFileIterator& TextFileIterator::operator ++() {
	TextLine *line = mFile->mLines[mLineIndex];
	if (line->size() - 1 > mColumnIndex) {
		mColumnIndex++;
	} else {
		if (mFile->mLines.size() > mLineIndex) {
			bool needChangeLine = false;
			const char *lfBytes = line->getLineFeed();
			int lfBytesLen = (int) sizeof(lfBytes) - 1;
			mLineFeedIndex++;
			if (mLineFeedIndex == lfBytesLen) {
				mLineFeedIndex = -1;
				needChangeLine = true;
			}
			if (needChangeLine) {
				mLineIndex++;
				mColumnIndex = 0;
			}
		}
	}
	return *this;
}

TextFileIterator TextFileIterator::operator ++(int) {
	TextFileIterator old = *this;
	this->operator ++();
	return old;
}

TextFileIterator& TextFileIterator::operator --() {
	if (0 < mColumnIndex) {
		mColumnIndex--;
	} else {
		if (0 < mLineIndex) {
			mLineIndex--;
			TextLine *beforeLine = mFile->mLines[mLineIndex];
			mColumnIndex = beforeLine->size() - 1;
		}
	}
	return *this;
}

TextFileIterator TextFileIterator::operator --(int) {
	TextFileIterator old = *this;
	this->operator --();
	return old;
}

bool operator ==(const TextFileIterator& x, const TextFileIterator& y) {
	if (x.mFile == y.mFile) {
		if (x.mLineIndex == y.mLineIndex) {
			if (x.mColumnIndex == y.mColumnIndex) {
				if (x.mLineFeedIndex == y.mLineFeedIndex) {
					return true;
				}
			}
		}
	}
	return false;
}

bool operator !=(const TextFileIterator& x, const TextFileIterator& y) {
	return !(x == y);
}

