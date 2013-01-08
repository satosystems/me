VPATH = src out

ifeq ($(OS),Windows_NT)
INCLUDES = -I/usr/include/ncurses
endif

LIBS = -lncurses

OBJS = Main.o

CXXFLAGS = -O2 -g -Wall $(INCLUDES)

TARGET = me

$(TARGET): $(patsubst %,out/%,$(OBJS))
	$(CXX) -o $(TARGET) $^ $(LIBS)

all: $(TARGET)

out/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(patsubst %,out/%,$(OBJS)) $(TARGET) $(TARGET).exe

