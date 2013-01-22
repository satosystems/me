#ifndef _ceb4cae6_6daf_410c_ad84_b64261378255_
#define _ceb4cae6_6daf_410c_ad84_b64261378255_

#include <stdio.h>
#include <stdarg.h>

class Logger {
public:
	static void w(const char *format, ...) {
		va_list arg;
		va_start(arg, format);
		p("W: ", format, &arg);
		va_end(arg);
	}

	static void d(const char *format, ...) {
		va_list arg;
		va_start(arg, format);
		p("D: ", format, &arg);
		va_end(arg);
	}

	static void e(const char *format, ...) {
		va_list arg;
		va_start(arg, format);
		p("E: ", format, &arg);
		va_end(arg);
	}

private:
	static void p(const char *tag, const char *format, va_list *arg) {
		FILE *fp = fopen("trace.log", "a");
		assert(fp);
		fprintf(fp, "%s", tag);
		vfprintf(fp, format, *arg);
		fprintf(fp, "\n");
		fflush(fp);
		fclose(fp);
	}
};

#endif
