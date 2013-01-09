#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <signal.h>
#include <curses.h>

#include <vector>

#define CTRL_X 0x18
#define KEY_ESC 0x1b

static void setSignalHandler(int signame);
static void signalHandler(int signame);
class Tracer;

Tracer *gTracer;
std::vector<int> gKeyBuffer;

class Tracer {
public:
	Tracer() {
		mFile = fopen("trace.log", "a");
		assert(mFile);
	}

	~Tracer() {
		gTracer = NULL;
		fclose(mFile);
	}

	void d(const char *format, ...) {
		va_list arg;
		va_start(arg, format);
		p("D: ", format, &arg);
		va_end(arg);
	}

	void e(const char *format, ...) {
		va_list arg;
		va_start(arg, format);
		p("E: ", format, &arg);
		va_end(arg);
	}

private:
	FILE *mFile;

	void p(const char *tag, const char *format, va_list *arg) {
		fprintf(mFile, "%s", tag);
		vfprintf(mFile, format, *arg);
		fflush(mFile);
	}
};

static void setSignalHandler(int signame) {
	if (signal(signame, signalHandler) == SIG_ERR) {
		if (gTracer) {
			gTracer->e("failed to set signal handler\n");
		}
		putchar(CTRL_X);
	}
}

static void signalHandler(int signame) {
	if (signame == SIGINT) {
		if (gTracer) {
			gTracer->d("signal:SIGINT (%d)\n", signame);
		}
		setSignalHandler(signame);
	}
}

static void loop() {
	int ch;
	int x = 0, y = 0;
	Tracer tracer;

	gTracer = &tracer;
	setSignalHandler(SIGINT);
	mvaddstr(0, 0, "Press Ctrl-x to exit.");
	getyx(stdscr, y, x);

	while ((ch = getch()) != CTRL_X) {
		tracer.d("ch:0x%x (%d)\n", ch, ch);
		switch (ch) {
		case KEY_LEFT:
			x -= 1;
			gKeyBuffer.clear();
			break;
		case KEY_RIGHT:
			x += 1;
			gKeyBuffer.clear();
			break;
		case KEY_UP:
			y -= 1;
			gKeyBuffer.clear();
			break;
		case KEY_DOWN:
			y += 1;
			gKeyBuffer.clear();
			break;
		case KEY_ESC:
			if (gKeyBuffer.size() == 1 && gKeyBuffer.back() == KEY_ESC) {
				gKeyBuffer.clear();
				mvaddstr(LINES - 1, 0, ":");
				getyx(stdscr, y, x);
			} else {
				gKeyBuffer.push_back(ch);
			}
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

