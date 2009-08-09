
ARCH	:= $(shell uname -m)
CC	:= cc
CFLAGS	:= -g -DDEBUG -v -xcheck -m64 
#CC	:= gcc
#CFLAGS	:= -g -DDEBUG -std=c99
#OBJ	:= kgsh.o builtins.o get_path.o util.o alias.o wildcard.o
OBJDIR	:= tgt-$(ARCH)
#SOURCES := $(shell ls -l src/*.c | awk '{print $9}' | sed 's/src\///g')
SOURCES := $(shell ls -t src/*.c | sed 's/src\///g')
OBJS	:= $(SOURCES:%.c=$(OBJDIR)/%.o)

default: $(OBJDIR)/kgsh

$(OBJDIR)/kgsh: $(OBJS) 
	$(CC) $(CFLAGS) $(OBJS) -o $@

$(OBJDIR)/%.o: src/%.c
	mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $<  -o $@

doc:
	doxygen
	make -f docs/latex/Makefile

clean: 
	rm -r $(OBJDIR)

clean_all:
	rm -r 'tgt-*'
