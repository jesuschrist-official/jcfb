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

# C Compiler
CC=gcc
CFLAGS=-Wall -std=gnu99 -Werror -I$(DINC)
LDFLAGS=
ifeq ($(DEBUG),1)
	CFLAGS+=-DDEBUG -g
endif

# Targets
JCFB=$(DBUILD)/libjcfb.a
SAMPLE=$(DBUILD)/sample.exe

# Rules
all: $(DBUILD) $(JCFB) $(SAMPLE)

$(DOBJ)/%.o: $(DSRC)/%.c
	$(CC) $(CFLAGS) -c $^ -o $@

$(JCFB): $(DOBJ)/pixel.o \
         $(DOBJ)/jcfb.o
	$(AR) rvs $@ $^

$(SAMPLE): $(JCFB) $(DSAMPLE)/sample.c
	$(CC) $(CFLAGS) -L$(DBUILD) $^ -o $@ -ljcfb

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
