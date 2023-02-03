
MAKEFLAGS  += --jobs=$(shell nproc)

CC			= g++
LD			= g++

TARGET		= $(shell cat build/current_arch)
VERSION		= $(shell cat version)
SOURCES		= $(shell find -name '*.c' -o -name '*.cpp')
HEADERS		= $(shell find -name '*.h' -o -name '*.hpp')
OBJECTS		= $(shell cd src; find -name '*.c' -o -name '*.cpp' -printf 'build/%p.o ')
SRC_DEBUG	= $(SOURCES)
HDR_DEBUG	= $(HEADERS)
OBJ_DEBUG	= $(shell cd src; find -name '*.c' -o -name '*.cpp' -printf 'build/%p.debug.o ')
INCLUDES	= -Isrc -Ijson/include

OUTFILE		= hsdserver
CCFLAGS		= $(INCLUDES)
FLAGS_DEBUG	= $(CCFLAGS) -ggdb
LDFLAGS		= -lpaho-mqttpp3 -lpaho-mqtt3a

.PHONY: all run debug debug-run clean libs

# Commands for the user.
all: ./build/main.o
	@$(LD) ./build/main.o -o $(OUTFILE) $(LDFLAGS)
	@echo LD $(OUTFILE)

debug: ./build/debug.o
	@$(LD) -ggdb ./build/debug.o -o $(OUTFILE) $(LDFLAGS)
	@echo LD $(OUTFILE)

run: all
	./$(OUTFILE)

debug-run: debug
	./$(OUTFILE)

# Compilation
./build/main.o: $(OBJECTS)
	@$(LD) -r $^ -o $@
	@echo LD $@
	
./build/debug.o: $(OBJ_DEBUG)
	@$(LD) -ggdb -r $^ -o $@
	@echo LD $@

./build/%.o: ./src/% $(HEADERS) Makefile
	@mkdir -p $(shell dirname $@)
	@$(CC) -c $< $(CCFLAGS) -o $@
	@echo CC $<

./build/%.debug.o: ./src/% $(HDR_DEBUG) Makefile
	@mkdir -p $(shell dirname $@)
	@$(CC) -c $< $(FLAGS_DEBUG) -o $@
	@echo CC $<

# Other
clean:
	rm -f $(OBJECTS) $(OBJ_DEBUG) $(OUTFILE)
	rm -rf $(shell find build/* -type d)
