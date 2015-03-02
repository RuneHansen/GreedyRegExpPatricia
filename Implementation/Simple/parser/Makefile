EXECUTABLE=example

.phony: all
all: $(EXECUTABLE)

.phony: benchmarks
benchmarks: $(BENCHMARKS)

# Configuration

CC=gcc
AR=ar
ARFLAGS=rsc
YACC=bison
LEX=flex
CONF_CFLAGS=-O0
CFLAGS=-std=c99 -c -I. -Wall -g -D_POSIX_SOURCE $(CONF_CFLAGS)
LDFLAGS=$(CONF_LDFLAGS)


# Project input

HEADERS=\
	parser.h\
	lex.h\
	regex.h

OBJECTS=\
	obj/regex.o\
	obj/parser.o\
	obj/lex.o

# Code generation

lex.c lex.h: lex.l
	$(LEX) $<

parser.c parser.h: parser.y lex.h
	$(YACC) --name-prefix="BitC_YY" $<

obj/%.o: %.c $(HEADERS)
	@mkdir -p $$(dirname $@)
	$(CC) -o $@ $(CFLAGS) $*.c


# Linking

$(EXECUTABLE): obj/example.o $(OBJECTS)
	@mkdir -p $$(dirname $@)
	$(CC) $(LDFLAGS) -o $@ obj/example.o $(OBJECTS)

.phony: clean
clean:
	rm -rf obj
	rm -f $(EXECUTABLE) parser.c lex.c parser.h lex.h
