
ARCH:sh =uname -m
CC=cc
CFLAGS=-g -DDEBUG -v -xcheck -m64 
#CFLAGS=-std=c99
OBJ= kgsh.o builtins.o get_path.o util.o alias.o wildcard.o
OBJDIR=obj-$(ARCH)

default: kgsh

kgsh: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@

%.o: src/%.c
	$(CC) $(CFLAGS) -c $<  -o $@

clean: 
	rm -f kgsh *.o
