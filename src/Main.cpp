#include <stdio.h>
#include <stdlib.h>
#include <curses.h>

static void loop() {
	int ch;
	int x = 0, y = 0;

	mvaddstr(0, 0, "Press Ctrl-c to exit.");

	while (true) {
		ch = getch();
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

