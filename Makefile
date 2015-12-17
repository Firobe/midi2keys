COMPILER = g++
ADD_CFLAGS = -Wall -D__LINUX_ALSA__
LIBS = rtmidi alsa
CXXFLAGS = $(ADD_CFLAGS)
LINKS = `pkg-config --static --libs $(LIBS)`
SOURCES = $(wildcard src/*.cpp)
EXEC_NAME = p2o
OBJECTS = $(SOURCES:.cpp=.o)
.PRECIOUS : %.o
.PHONY : clean
.PHONY : run

all: $(OBJECTS)
	$(COMPILER) -o $(EXEC_NAME) $(OBJECTS) $(LINKS)

%.o : %.c
	$(COMPILER) -c $(CXXFLAGS) $< -o $@

run: all
	./$(EXEC_NAME)

clean:
	rm -f $(OBJECTS) $(EXEC_NAME)
