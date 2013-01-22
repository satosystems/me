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
#include "Logger.h"

#define CTRL_X 0x18
#define KEY_ESC 0x1b

static void setSignalHandler(int signame);
static void signalHandler(int signame);

class Buffer {
public:
	Buffer() : mFile(NULL), x(0), y(0) {
	}
	~Buffer() {
		delete mFile;
	}
	File *mFile;
	int x;
	int y;
};

std::vector<int> gKeyBuffer;
std::vector<Buffer *> gBuffers;

static void setSignalHandler(int signame) {
	if (signal(signame, signalHandler) == SIG_ERR) {
		Logger::e("failed to set signal handler");
		putchar(CTRL_X);
	}
}

static void signalHandler(int signame) {
	if (signame == SIGINT) {
		Logger::d("signal:SIGINT (%d)", signame);
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
		Logger::d("input:[%s] normalized[%s]", hexOrg.c_str(), hexNor.c_str());
	}
	addstr(normalized.c_str());
	return normalized.size();
}

static void loop() {
	int ch;
	std::string mbcbuf;
	int mbcl = 0; // multi byte char length

	setSignalHandler(SIGINT);

	Buffer *buffer = gBuffers[0];
	buffer->mFile->load();
	int lineCount = buffer->mFile->getLineCount();
	for (int i = 0; i < lineCount; i++) {
		Line *line = buffer->mFile->getLine(i);
		addnstr(line->head(), line->head_size());
		addnstr(line->tail(), line->tail_size());
		getyx(stdscr, buffer->y, buffer->x);
		move(++buffer->y, 0);
		if (buffer->y == LINES - 1) {
			break;
		}
	}

	move(0, 0);

	while ((ch = getch()) != CTRL_X) {
		Logger::d("ch:0x%x (%d)", ch, ch);
		if (mbcl <= 0) {
			switch (ch) {
			case KEY_LEFT:
				buffer->x -= 1;
				gKeyBuffer.clear();
				break;
			case KEY_RIGHT:
				buffer->x += 1;
				gKeyBuffer.clear();
				break;
			case KEY_UP:
				buffer->y -= 1;
				gKeyBuffer.clear();
				break;
			case KEY_DOWN:
				buffer->y += 1;
				gKeyBuffer.clear();
				break;
			case KEY_ESC:
				if (gKeyBuffer.size() == 1 && gKeyBuffer.back() == KEY_ESC) {
					gKeyBuffer.clear();
					mvaddstr(LINES - 1, 0, ":");
					getyx(stdscr, buffer->y, buffer->x);
				} else {
					gKeyBuffer.push_back(ch);
				}
				break;
			default:
				if (mbcl <= 0) {
					mbcl = guessUtf8SequenceLength(ch);
					Logger::d("mbcl:%d", mbcl);
					if (mbcl == -1) {
						Logger::w("Invalid UTF-8 sequence:%x", ch);
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
				Logger::d("[%s]", mbcbuf.c_str());
				int width = 1;
				if (mbcbuf.size() > 1) {
					icu::StringPiece sp(mbcbuf);
					icu::UnicodeString us(icu::UnicodeString::fromUTF8(sp));
					assert(1 == us.length());
					width = wcwidth(us[0]);
					Logger::d("width:%d", width);
				}
				buffer->x += width;
				mbcbuf.clear();
			}
		}

		if (buffer->x <= 0) {
			buffer->x = 0;
		} else if (buffer->x > COLS - 1) {
			buffer->x = COLS - 1;
		} else if (buffer->y <= 0) {
			buffer->y = 0;
		} else if (buffer->y >= LINES - 1) {
			buffer->y = LINES - 1;
		}

		move(buffer->y, buffer->x);
	}
	for (std::vector<Buffer *>::iterator it = gBuffers.begin(); it != gBuffers.end(); ++it) {
		delete *it;
	}
	gBuffers.clear();
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
				Buffer *buffer = new Buffer;
				buffer->mFile = new File(*it);
				gBuffers.push_back(buffer);
			}
		} else {
			Buffer *buffer = new Buffer;
			buffer->mFile = new File;
			gBuffers.push_back(buffer);
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
