# jcfblib Makefile


# User variables
DEBUG=0
INSTALLATION_PATH=/usr/local

# Directories
DSRC=src
DINC=include
DTESTS=tests
DBUILD=build
DOBJ=$(DBUILD)/obj
DSAMPLE=sample
DBENCH=benchmarks

# C Compiler
CC=gcc
CFLAGS=-Wall -Werror -std=gnu11 -I$(DINC)
LDFLAGS=-lm
ifeq ($(DEBUG),1)
	CFLAGS+=-DDEBUG -g
endif
ifeq ($(RELEASE),1)
	CFLAGS+=-DRELEASE -O3 -flto
endif


# Targets
JCFB=$(DBUILD)/libjcfb.a

PRINT=$(DBUILD)/$(DSAMPLE)/print.exe
MANDELBROT=$(DBUILD)/$(DSAMPLE)/mandelbrot.exe
TETRIS=$(DBUILD)/$(DSAMPLE)/tetris.exe
TTF=$(DBUILD)/$(DSAMPLE)/ttf.exe
KEYBOARD=$(DBUILD)/$(DSAMPLE)/keyboard.exe
MOUSE=$(DBUILD)/$(DSAMPLE)/mouse.exe
MOVE=$(DBUILD)/$(DSAMPLE)/move.exe
CONVERT=$(DBUILD)/$(DSAMPLE)/convert.exe
PRIMITIVE=$(DBUILD)/$(DSAMPLE)/primitive.exe
FIREWORK=$(DBUILD)/$(DSAMPLE)/firework.exe


# Rules
all: $(DBUILD) $(JCFB) samples tests benchmarks


$(DOBJ)/%.o: $(DSRC)/%.c
	$(CC) $(CFLAGS) -c $^ -o $@


$(JCFB): $(DOBJ)/pixel.o \
         $(DOBJ)/jcfb.o \
         $(DOBJ)/bitmap.o \
         $(DOBJ)/bitmap-io.o \
         $(DOBJ)/primitive.o \
         $(DOBJ)/ttf.o \
         $(DOBJ)/keyboard.o \
		 $(DOBJ)/mouse.o
	$(AR) rvs $@ $^


samples: $(PRINT) $(MANDELBROT) $(TETRIS) $(TTF) $(MOVE) $(KEYBOARD) \
		 $(MOUSE) $(CONVERT) $(PRIMITIVE) $(FIREWORK)


$(PRINT): $(JCFB) $(DSAMPLE)/print.c
	$(CC) $(CFLAGS) -L$(DBUILD) $^ -o $@ -ljcfb $(LDFLAGS)


$(MANDELBROT): $(JCFB) $(DSAMPLE)/mandelbrot.c
	$(CC) $(CFLAGS) -L$(DBUILD) $^ -o $@ -ljcfb $(LDFLAGS) -lm


$(TETRIS): $(JCFB) $(DSAMPLE)/tetris.c
	$(CC) $(CFLAGS) -L$(DBUILD) $^ -o $@ -ljcfb $(LDFLAGS) -lm


$(TTF): $(JCFB) $(DSAMPLE)/ttf.c
	$(CC) $(CFLAGS) -L$(DBUILD) $^ -o $@ -ljcfb $(LDFLAGS) -lm


$(MOVE): $(JCFB) $(DSAMPLE)/move.c
	$(CC) $(CFLAGS) -L$(DBUILD) $^ -o $@ -ljcfb $(LDFLAGS) -lm


$(KEYBOARD): $(JCFB) $(DSAMPLE)/keyboard.c
	$(CC) $(CFLAGS) -L$(DBUILD) $^ -o $@


$(MOUSE): $(JCFB) $(DSAMPLE)/mouse.c
	$(CC) $(CFLAGS) -L$(DBUILD) $^ -o $@ -ljcfb


$(CONVERT): $(JCFB) $(DSAMPLE)/convert.c
	$(CC) $(CFLAGS) -L$(DBUILD) $^ -o $@ -ljcfb -lm


$(PRIMITIVE): $(JCFB) $(DSAMPLE)/primitive.c
	$(CC) $(CFLAGS) -L$(DBUILD) $^ -o $@ -ljcfb -lm

$(FIREWORK): $(JCFB) $(DSAMPLE)/firework.c
	$(CC) $(CFLAGS) -L$(DBUILD) $^ -o $@ -ljcfb -lm


tests: $(DBUILD)/$(DTESTS)/pixel.test


$(DBUILD)/$(DTESTS)/pixel.test: $(DSRC)/pixel.c
	$(CC) $(CFLAGS) -DTEST $^ -o $@


benchmarks: $(DBUILD)/$(DBENCH)/pixel-conversion.bench \
            $(DBUILD)/$(DBENCH)/bitmap-blit.bench \
            $(DBUILD)/$(DBENCH)/jcfb-refresh.bench


$(DBUILD)/$(DBENCH)/%.bench: $(DBENCH)/%.c
	$(CC) $(CFLAGS) $^ -o $@ -L$(DBUILD) -ljcfb


$(DBUILD):
	mkdir -p $(DBUILD)
	mkdir -p $(DBUILD)/$(DSAMPLE)
	mkdir -p $(DBUILD)/$(DBENCH)
	mkdir -p $(DBUILD)/$(DTESTS)
	mkdir -p $(DOBJ)
	cp -r data/ $(DBUILD)


install: $(JCFB)
	cp $(JCFB) $(INSTALLATION_PATH)/lib
	cp -r $(DINC)/jcfb $(INSTALLATION_PATH)/include
	cp -r $(DINC)/stb $(INSTALLATION_PATH)/include


uninstall:
	rm $(INSTALLATION_PATH)/lib/libjcfb.a
	rm -rf $(INSTALLATION_PATH)/include/jcfb
	rm -rf $(INSTALLATION_PATH)/include/stb


clean:
	rm -rf $(DBUILD)
