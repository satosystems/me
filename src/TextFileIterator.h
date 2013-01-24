#ifndef _825e4294_be4e_45ec_9845_95b383929ca9_
#define _825e4294_be4e_45ec_9845_95b383929ca9_

class TextFile;

class TextFileIterator {
	friend class TextFile;
	friend bool operator ==(const TextFileIterator& x, const TextFileIterator& y);
public:
	TextFileIterator(const TextFileIterator& that);
	~TextFileIterator();
	TextFileIterator& operator =(const TextFileIterator& that);
	const char& operator *() const;
	TextFileIterator *operator ->() const;
	TextFileIterator& operator ++();
	TextFileIterator operator ++(int);
	TextFileIterator& operator --();
	TextFileIterator operator --(int);

private:
	const TextFile *mFile;
	int mLineIndex;
	int mColumnIndex;
	int mLineFeedIndex;

	TextFileIterator();
};

bool operator ==(const TextFileIterator& x, const TextFileIterator& y);
bool operator !=(const TextFileIterator& x, const TextFileIterator& y);

#endif

