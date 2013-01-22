#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <signal.h>
#include <locale.h>
#include <curses.h>

#include <stdint.h>

#include <unicode/normalizer2.h>
#include <unicode/stringpiece.h>
#include <unicode/unistr.h>

#include <boost/program_options.hpp>

#include <string>
#include <vector>
#include <iostream>
#include <exception>

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE
#endif
#include <wchar.h>

#include "Utf8Utils.h"
#include "File.h"

#define CTRL_X 0x18
#define KEY_ESC 0x1b

static void setSignalHandler(int signame);
static void signalHandler(int signame);
class Tracer;

Tracer *gTracer;
std::vector<int> gKeyBuffer;
std::vector<File *> gFiles;

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

	void w(const char *format, ...) {
		va_list arg;
		va_start(arg, format);
		p("W: ", format, &arg);
		va_end(arg);
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
		fprintf(mFile, "\n");
		fflush(mFile);
	}
};

static void setSignalHandler(int signame) {
	if (signal(signame, signalHandler) == SIG_ERR) {
		if (gTracer) {
			gTracer->e("failed to set signal handler");
		}
		putchar(CTRL_X);
	}
}

static void signalHandler(int signame) {
	if (signame == SIGINT) {
		if (gTracer) {
			gTracer->d("signal:SIGINT (%d)", signame);
		}
		setSignalHandler(signame);
	}
}

int me_addstr(const char *utf8str) {
	size_t len = strlen(utf8str);
	if (len == 1) {
		addstr(utf8str);
		return 1;
	}
	std::string normalized = toUtf8(utf8str, utf8str + len, "UTF-8");
	if (normalized.compare(utf8str) != 0) {
		std::string hexOrg, hexNor;
		for (size_t i = 0; i < strlen(utf8str); i++) {
			char buf[16];
			sprintf(buf, "%02X", utf8str[i] & 0xFF);
			hexOrg.append(buf);
		}
		for (size_t i = 0; i < normalized.size(); i++) {
			char buf[16];
			sprintf(buf, "%02X", normalized[i] & 0xFF);
			hexNor.append(buf);
		}
		gTracer->d("input:[%s] normalized[%s]", hexOrg.c_str(), hexNor.c_str());
	}
	addstr(normalized.c_str());
	return normalized.size();
}

static void loop() {
	int ch;
	int x = 0, y = 0;
	std::string mbcbuf;
	int mbcl = 0; // multi byte char length
	Tracer tracer;

	gTracer = &tracer;
	setSignalHandler(SIGINT);

	File *file = gFiles[0];
	file->load();
	int lineCount = file->getLineCount();
	for (int i = 0; i < lineCount; i++) {
		Line *line = file->getLine(i);
		addnstr(line->head(), line->head_size());
		addnstr(line->tail(), line->tail_size());
		getyx(stdscr, y, x);
		move(++y, x);
	}

	getyx(stdscr, y, x);

	while ((ch = getch()) != CTRL_X) {
		tracer.d("ch:0x%x (%d)", ch, ch);
		if (mbcl <= 0) {
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
				if (mbcl <= 0) {
					mbcl = guessUtf8SequenceLength(ch);
					tracer.d("mbcl:%d", mbcl);
					if (mbcl == -1) {
						tracer.w("Invalid UTF-8 sequence:%x", ch);
						continue;
					}
				}
				break;
			}
		}
		if (mbcl > 0) {
			assert(ch < 0xFE);
			mbcbuf.push_back((char) ch);
			mbcl--;
			if (mbcl == 0) {
				me_addstr(mbcbuf.c_str());
				tracer.d("[%s]", mbcbuf.c_str());
				int width = 1;
				if (mbcbuf.size() > 1) {
					icu::StringPiece sp(mbcbuf);
					icu::UnicodeString us(icu::UnicodeString::fromUTF8(sp));
					assert(1 == us.length());
					width = wcwidth(us[0]);
					tracer.d("width:%d", width);
				}
				x += width;
				mbcbuf.clear();
			}
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

static void parseOption(int argc, char *argv[]) {
	namespace bpo = boost::program_options;

	bpo::options_description desc("Option");
	typedef std::vector<std::string> InputFiles;
	desc.add_options()
		("help,h", "help")
		("version", "version");

	bpo::options_description hidden("Hidden");
	hidden.add_options()
		("input-file", bpo::value<InputFiles>(), "input file");

	bpo::options_description opt;
	opt.add(desc).add(hidden);

	bpo::positional_options_description pos;
	pos.add("input-file", 64);

	try {
		bpo::variables_map params;
		bpo::store(bpo::command_line_parser(argc, argv)
			.options(opt)
			.positional(pos)
			.run(), params);
		bpo::notify(params);

		if (params.count("help")) {
			std::cout << desc << std::endl;
			exit(EXIT_SUCCESS);
		} else if (params.count("version")) {
			std::cout << "me version 0.0.1" << std::endl;
			exit(EXIT_SUCCESS);
		}

		if (params.count("input-file")) {
			InputFiles files(params["input-file"].as<InputFiles>());
			for (InputFiles::iterator it = files.begin(); it != files.end(); ++it) {
				gFiles.push_back(new File(*it));
			}
		} else {
			gFiles.push_back(new File());
		}
	} catch (const std::exception& error) {
		std::cerr << error.what() << std::endl;
		std::cerr << desc << std::endl;
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char *argv[]) {
	setlocale(LC_ALL, "");
	parseOption(argc, argv);

	initscr();
	cbreak();
	noecho();
	nonl();
	keypad(stdscr, TRUE);

	loop();

	endwin();
	return EXIT_SUCCESS;
}
