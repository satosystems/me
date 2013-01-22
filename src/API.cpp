#include <curses.h>

#include <vector>

#include "API.h"
#include "File.h"
#include "Buffer.h"
#include "Logger.h"

extern std::vector<Buffer *> gBuffers;

void me_up() {
}

void me_down() {

}

void me_left() {

}

void me_right() {
	Buffer *buffer = gBuffers[0];
	Logger::d("before y:%d x:%d", buffer->y, buffer->x);
	Line *line = buffer->mFile->getLine(buffer->lineno);
	int ccount = line->getCharCount();
	if (ccount > buffer->column) {
		buffer->column++;
		buffer->x++;
		if (buffer->x == COLS) {
			buffer->x = 0;
			buffer->y++;
		}
	} else {
		assert(ccount == buffer->column);
		if (buffer->mFile->getLineCount() > buffer->lineno) {
			buffer->column = 0;
			buffer->lineno++;
			buffer->x = 0;
			buffer->y++;
		}
	}
	Logger::d("after y:%d x:%d", buffer->y, buffer->x);
	move(buffer->y, buffer->x);
}

