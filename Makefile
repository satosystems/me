VPATH = src test/gtest

# How to detect OS type.
# Windows:
#  ifeq ($(OS),Windows_NT)
#  ...
#  endif
# Mac:
#  ifef (${shell uname},Darwin)
#  ...
#  endif
# Linux:
#  ifeq (${shell uname},Linux)
#  ...
#  endif
# FreeBSD:
#  ifeq ($(OS),FreeBSD)
#  ...
#  endif
UNAME = ${shell uname}

INCLUDES = -Isrc

ifeq ($(OS),Windows_NT)
INCLUDES += -I/usr/include/ncurses
endif

LIBS = -L/usr/local/lib -lboost_system -lboost_filesystem -lboost_regex

ifeq ($(UNAME),Darwin)
LIBS += -lncurses
else
LIBS += -lncursesw
endif

ifeq ($(OS),Windows_NT)
LIBS += `pkg-config --libs --cflags icu-uc icu-io`
else
LIBS += -licui18n -licuuc -licuio
endif

OBJS = Main.o Exception.o File.o Line.o

GTEST_OBJS = out/test/gtest-all.o out/test/gtest_main.o

CXXFLAGS = -O2 -g -Wall $(INCLUDES)

TARGET = me

$(TARGET): $(patsubst %,out/%,$(OBJS))
	$(CXX) -o $(TARGET) $^ $(LIBS)

all: $(TARGET)

out/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

out/test/GapBufferTest.o: test/GapBufferTest.cpp src/GapBuffer.h
	$(CXX) $(CXXFLAGS) -Itest -c -o $@ test/GapBufferTest.cpp

out/test/%.o: %.cc
	$(CXX) $(CXXFLAGS) -Itest -c -o $@ $<

clean:
	rm -f $(patsubst %,out/%,$(OBJS)) $(TARGET) $(TARGET).exe out/test/*

out/test/test_GapBuffer: $(GTEST_OBJS) out/test/GapBufferTest.o
	$(CXX) $(CXXFLAGS) -lpthread -Itest -o $@ $^

build_test: out/test/test_GapBuffer

test: build_test
	out/test/test_GapBuffer

