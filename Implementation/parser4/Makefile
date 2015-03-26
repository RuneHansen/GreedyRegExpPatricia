EXECUTABLE=TestSimpleI

.phony: all
all: $(EXECUTABLE)

.phony: benchmarks
benchmarks: $(BENCHMARKS)

# Configuration

CC=g++
AR=ar
ARFLAGS=rsc
YACC=bison
LEX=flex
CONF_CFLAGS=-O0
CFLAGS=-c -I. -Wall -g -D_POSIX_SOURCE -fpermissive $(CONF_CFLAGS)
LDFLAGS=$(CONF_LDFLAGS)


# Project input

HEADERS=\
	parser.h\
	lex.h\
	regex.h\
  Simple_aNFASimulator.h\
  Patricia_aNFASimulator.h\
  aNFAgen.h

OBJECTS=\
	obj/regex.o\
	obj/parser.o\
	obj/lex.o\
  obj/Simple_aNFASimulator.o\
  obj/Patricia_aNFASimulator.o\
  obj/aNFAgen.o

# Code generation

lex.c lex.h: lex.l
	$(LEX) $<

parser.c parser.h: parser.y lex.h
	$(YACC) --name-prefix="BitC_YY" $<

obj/%.o: %.cpp $(HEADERS)
	@mkdir -p $$(dirname $@)
	$(CC) -o $@ $(CFLAGS) $*.cpp


# Linking

$(EXECUTABLE): obj/TestSimpleI.o $(OBJECTS)
	@mkdir -p $$(dirname $@)
	$(CC) $(LDFLAGS) -o $@ obj/TestSimpleI.o $(OBJECTS)

.phony: clean
clean:
	rm -rf obj
	rm -f $(EXECUTABLE) parser.c lex.c parser.h lex.h 
