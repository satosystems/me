#ifndef _0215dffd_5085_4f25_9d88_a847271217d9_
#define _0215dffd_5085_4f25_9d88_a847271217d9_

#include <assert.h>
#include <stdlib.h>

template<class T>
class GapBuffer {
public:
	explicit GapBuffer(int capacity = 64) :
			GAP_GROWTH_SIZE(capacity),
			mCapacity(capacity), mGapBegin(0), mGapEnd(capacity),
			mBuffer((T *) malloc(capacity * sizeof(T))) {
		assert(capacity >= 1);
		if (mBuffer == NULL) {
			throw std::bad_alloc();
		}
	}

	~GapBuffer() {
		free(mBuffer);
	}

	int size() const {
		assert(mGapEnd >= mGapBegin);
		assert(mCapacity >= (mGapEnd - mGapBegin));
		return mCapacity - (mGapEnd - mGapBegin);
	}

	int capacity() const {
		return mCapacity;
	}

	void clear() {
		mGapBegin = 0;
		mGapEnd = mCapacity;
	}

	int insert(int pos, const T& val) {
		assert(pos <= size());
		return insert(pos, 1, val);
	}

	int insert(int pos, int num, const T& val) {
		assert(pos <= size());
		assert(num > 0);
		moveGap(pos);
		if (mGapEnd - mGapBegin < num) {
			appendGap(GAP_GROWTH_SIZE + num);
		}
		for (int i = 0; i < num; i++) {
			mBuffer[mGapBegin++] = val;
		}
		assert(pos + num == mGapBegin);
		return mGapBegin;
	}

	template<class InputIterator>
	int insert(int pos, InputIterator begin, InputIterator end) {
		assert(pos <= size());
		int len = ((intptr_t)end - (intptr_t)begin) / sizeof(T);
		assert(len > 0);

		moveGap(pos);
		if (mGapEnd - mGapBegin <= len) {
			appendGap(GAP_GROWTH_SIZE + len);
		}
		while (begin != end) {
			mBuffer[mGapBegin++] = *begin++;
		}
		assert(pos + len == mGapBegin);
		return mGapBegin;
	}

	void erase(int begin, int end) {
		int len = end - begin;

		if (begin <= mGapBegin && mGapBegin < end) {
			len -= mGapBegin - begin;
			mGapBegin = begin;
		} else {
			moveGap(begin);
		}
		mGapEnd += len;
	}

	void erase(int pos) {
		erase(pos, pos + 1);
	}

	/*	
	 * Adds an element to the end of the gap buffer.
	 * The status of this object will change after calling this function.
	 *
	 * Case 1: mCapacity == mGapEnd
	 *  push_back(x)
	 *  [ ][ ][ ][ ][ ][ ][ ][ ][ ][ ]
	 *   ^                             ^
	 *   |                             |
	 *   |                             + mGapEnd
	 *   + mGapBegin
	 *                  | 
	 *                  v
	 *  [x][ ][ ][ ][ ][ ][ ][ ][ ][ ]
	 *      ^                          ^
	 *      |                          |
	 *      |                          + mGapEnd
	 *      + mGapBegin
	 *
	 *  push_back(x)
	 *  [a][ ][ ][ ][ ][ ][ ][ ][ ][ ]
	 *      ^                          ^
	 *      |                          |
	 *      |                          + mGapEnd
	 *      + mGapBegin
	 *                  | 
	 *                  v
	 *  [a][x][ ][ ][ ][ ][ ][ ][ ][ ]
	 *      ^                          ^
	 *      |                          |
	 *      |                          + mGapEnd
	 *      + mGapBegin
	 * In this case, new element is saved to mGapBegin.
	 * 
	 * Case 2: mCapacity != mGapEnd
	 *  push_back(x)
	 *  [a][ ][ ][ ][ ][ ][ ][ ][ ][b]
	 *      ^                       ^
 	 *      |                       |
 	 *      |                       + mGapEnd
 	 *      + mGapBegin
 	 *                  | 
 	 *                  v
 	 *  [a][b][x][ ][ ][ ][ ][ ][ ][ ]
 	 *         ^                       ^
 	 *         |                       |
 	 *         |                       + mGapEnd
 	 *         + mGapBegin
 	 *
 	 *  push_back(x)
 	 *  [a][ ][ ][ ][ ][ ][b][c][d][e]
 	 *      ^              ^
 	 *      |              |
 	 *      |              + mGapEnd
 	 *      + mGapBegin
 	 *                  | 
 	 *                  v
 	 *  [a][b][c][d][e][x][ ][ ][ ][ ]
 	 *                     ^           ^
 	 *                     |           |
 	 *                     |           + mGapEnd
 	 *                     + mGapBegin
	 * In this case, fast gap is moved to mBuffer tail,
	 * after that it is same case 1.
 	 *
	 * Case 3: mCapacity == mGapBegin or mGapBegin == mGapEnd
 	 *  push_back(x)
 	 *  [a][b][c][d][e][f][g][h][i][j]
 	 *                                 ^  
 	 *                                 |
 	 *                                 + mGapEnd
 	 *                                 + mGapBegin
	 * In this case, there are no gap area in mBuffer,
	 * so must to allocate new gap area at mBuffer tail.
	 * After that, this is same case 1.
	 */
	void push_back(const T& val) {
		if (mCapacity == mGapBegin) {
			assert(mCapacity == mGapEnd);
			appendGap(GAP_GROWTH_SIZE + 1);
		}
		if (mCapacity != mGapEnd) {
			int gapSize = mGapEnd - mGapBegin;
			moveGap(mCapacity - gapSize);
		}
		if (mCapacity == mGapEnd) {
			mBuffer[mGapBegin++] = val;
		}
	}

	T& operator[](int pos) {
		assert(pos >= 0 && pos < size());
		return mBuffer[(pos < mGapBegin) ? pos : pos + (mGapEnd - mGapBegin)];
	}

	const T& operator[](int pos) const {
		assert(pos >= 0 && pos < size());
		return mBuffer[(pos < mGapBegin) ? pos : pos + (mGapEnd - mGapBegin)];
	}

private:
	const int GAP_GROWTH_SIZE;
	int mCapacity, mGapBegin, mGapEnd;
	T *mBuffer;

	/**
	 * Move gap to specified position.
	 *
	 * Case 1: pos < mGapBegin
	 *  [#][a][b][c][ ][ ][ ][ ][#][#]
	 *      ^
	 *      |
	 *      + pos
 	 *                  | 
 	 *                  v
	 *  [#][ ][ ][ ][ ][a][b][c][#][#]
	 *
	 *  [#][a][b][c][d][ ][ ][#][#][#]
	 *      ^
	 *      |
	 *      + pos
 	 *                  | 
 	 *                  v
	 *  [#][ ][ ][a][b][c][d][#][#][#]
	 *
	 *  [#][a][ ][ ][ ][#][#][#][#][#]
	 *      ^
	 *      |
	 *      + pos
 	 *                  | 
 	 *                  v
	 *  [#][ ][ ][ ][a][#][#][#][#][#]
	 *
	 * Case 2: pos > mGapBegin 
	 *  [#][#][ ][ ][ ][a][b][c][d][#]
	 *                     ^
	 *                     |
	 *                     + pos
 	 *                  | 
 	 *                  v
	 *  [#][#][a][b][c][d][ ][ ][ ][#]
	 *
	 *  [#][#][#][#][ ][ ][a][b][#][#]
	 *                     ^
	 *                     |
	 *                     + pos
 	 *                  | 
 	 *                  v
	 *  [#][#][#][#][a][b][ ][ ][#][#]
	 *
	 *  [#][#][#][ ][ ][ ][ ][a][b][c]
	 *                     ^
	 *                     |
	 *                     + pos
 	 *                  | 
 	 *                  v
	 *  [#][#][#][a][b][c][ ][ ][ ][ ]
	 *
	 * Case 3: pos == mGapBegin 
	 * In this case, does not need to move gap.
	 */
	void moveGap(int pos) {
		if (mGapBegin == pos) {
			return;
		}
		if (mGapBegin == mGapEnd) {
			mGapBegin = mGapEnd = pos;
			return;
		}
		int gapSize = mGapEnd - mGapBegin;
		assert(gapSize >= 0);
		assert(gapSize + pos <= mCapacity);
		size_t len, size;
		T *dest;
		T *src = &mBuffer[pos];

		if (pos < mGapBegin) {
			len = mGapBegin - pos;
			size = len * sizeof(T);
			dest = &mBuffer[pos + gapSize];
		} else /*if (pos > mGapBegin)*/ { // it commented out that mean silence compiler warning 
			len = gapSize + (pos - mGapEnd);
			size = len * sizeof(T);
			dest = &mBuffer[pos - len];
			assert(mBuffer + mCapacity >= dest + len);
		}
		memmove(dest, src, size);
		mGapEnd = pos + gapSize;
		mGapBegin = pos;
	}

	void appendGap(int size) {
// it commented out that mean silence compiler warning
//		assert(size >= GAP_GROWTH_SIZE);
		assert(mGapEnd >= mGapBegin && mGapEnd <= mCapacity);
		T *newBuffer = (T *) realloc(mBuffer, (mCapacity + size) * sizeof(T));
		if (newBuffer == NULL) {
			throw std::bad_alloc();
		}
		mBuffer = newBuffer;	
		int gapBeginBackup = mGapBegin;
		int gapEndBackup = mGapEnd;
		mGapBegin = mCapacity;
		mGapEnd = mCapacity + size;
		mCapacity += size;
		moveGap(gapEndBackup);
		mGapBegin = gapBeginBackup;
		assert(mGapEnd - mGapBegin >= size);
		assert(mGapEnd <= mCapacity);
	}
};

#endif

