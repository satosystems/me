#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <curses.h>

#define CTRL_X 0x18

class Tracer {
public:
	Tracer() {
		mFile = fopen("trace.log", "a");
		assert(mFile);
	}

	~Tracer() {
		fclose(mFile);
	}

	void d(const char *format, ...) {
		va_list arg;
		va_start(arg, format);
		fprintf(mFile, "D: ");
		vfprintf(mFile, format, arg);
		fflush(mFile);
		va_end(arg);
	}

private:
	FILE *mFile;
};

static void loop() {
	int ch;
	int x = 0, y = 0;
	Tracer tracer;

	mvaddstr(0, 0, "Press Ctrl-x to exit.");
	getyx(stdscr, y, x);

	while ((ch = getch()) != CTRL_X) {
		tracer.d("ch:0x%x (%d)\n", ch, ch);
		switch (ch) {
		case KEY_LEFT:
			x -= 1;
			break;
		case KEY_RIGHT:
			x += 1;
			break;
		case KEY_UP:
			y -= 1;
			break;
		case KEY_DOWN:
			y += 1;
			break;
		default:
			break;
		}

		if (x <= 0) {
			x = 0;
		} else if (x > COLS - 1) {
			x = COLS - 1;
		} else if (y <= 0) {
			y = 0;
		} else if (y >= LINES - 1) {
			y = LINES - 1;
		}

		move(y, x);
	}
}

int main(int argc, char *argv[]) {
	initscr();
	cbreak();
	noecho();
	nonl();
	keypad(stdscr, TRUE);

	loop();

	endwin();
	return EXIT_SUCCESS;
}

