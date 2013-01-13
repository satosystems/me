VPATH = src test/gtest

INCLUDES = -Isrc

ifeq ($(OS),Windows_NT)
INCLUDES += -I/usr/include/ncurses
endif

LIBS = -lncursesw

OBJS = Main.o

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

