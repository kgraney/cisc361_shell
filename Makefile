ARCH	:= $(shell uname -m)
#CC	    := cc
#CFLAGS	:= -g -DDEBUG -v -xcheck -I/usr/local/include -L/usr/local/lib -lreadline -lcurses
CC	:= gcc
CFLAGS	:= -g -DDEBUG -std=c99 -I/usr/local/include -L/usr/local/lib -lreadline -lcurses -lpthread
#OBJ	:= kgsh.o builtins.o get_path.o util.o alias.o wildcard.o
OBJDIR	:= tgt-$(ARCH)
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
	make -f doc/latex/Makefile

clean: 
	rm -rf $(OBJDIR)
	rm -rf doc/*
	

clean_all: clean
	rm -rf 'tgt-*'
