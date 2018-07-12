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
CFLAGS=-Wall -std=gnu99 -Werror -I$(DINC)
LDFLAGS=-lncurses
ifeq ($(DEBUG),1)
	CFLAGS+=-DDEBUG -g
endif
ifeq ($(RELEASE),1)
	CFLAGS+=-DRELEASE -O3
endif

# Targets
JCFB=$(DBUILD)/libjcfb.a
SAMPLE=$(DBUILD)/sample.exe

# Rules
all: $(DBUILD) $(JCFB) $(SAMPLE) tests benchmarks


$(DOBJ)/%.o: $(DSRC)/%.c
	$(CC) $(CFLAGS) -c $^ -o $@


$(JCFB): $(DOBJ)/pixel.o \
         $(DOBJ)/jcfb.o \
         $(DOBJ)/bitmap.o \
         $(DOBJ)/bitmap-io.o
	$(AR) rvs $@ $^


$(SAMPLE): $(JCFB) $(DSAMPLE)/sample.c
	$(CC) $(CFLAGS) -L$(DBUILD) $^ -o $@ -ljcfb $(LDFLAGS)
	cp sample/tiger.bmp $(DBUILD)


tests: $(DBUILD)/pixel.test


$(DBUILD)/pixel.test: $(DSRC)/pixel.c
	$(CC) $(CFLAGS) -DTEST $^ -o $@


benchmarks: $(DBUILD)/pixel-conversion.bench \
            $(DBUILD)/bitmap-blit.bench \
            $(DBUILD)/jcfb-refresh.bench


$(DBUILD)/%.bench: $(DBENCH)/%.c
	$(CC) $(CFLAGS) $^ -o $@ -L$(DBUILD) -ljcfb -lncurses


$(DBUILD):
	mkdir -p $(DBUILD)
	mkdir -p $(DOBJ)


install: $(JCFB)
	cp $(JCFB) $(INSTALLATION_PATH)/lib
	cp -r $(DINC)/jcfb $(INSTALLATION_PATH)/include


uninstall:
	rm $(INSTALLATION_PATH)/lib/$(JCFB)
	rm -rf $(INSTALLATION_PATH)/include/jcfb


clean:
	rm -rf $(DBUILD)
