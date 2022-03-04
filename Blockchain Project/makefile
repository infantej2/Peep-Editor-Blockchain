# -----------------------------------------------------------------
# Student's Name: Filip Dosenovic, Jethro Infante, Damion Shillinglaw
# Makefile for Assignment 3
# Lab Section: X01L, Fall 2021
# CMPUT 201, Fall 2021
# Lab Instructor's Name: Nesrine Abbas


# Purpose: This is a makefile for assignment 3 which allows the
# user to compile as properly needed, clean up the directroy from
# executables and .o files, and contains the testing files for the
# programs in order to run and demonstrate functionality
# ----------------------------------------------------------------*/


CC = gcc
CFLAGS =  -Wall -std=c99
HASH = -lcrypto -lssl -lm 

all: peep testing1 testing2 testing3 testing4

peep: peep.c blockChain.o peepEditor.o
	$(CC) $(CFLAGS) -o peep peep.c blockChain.o peepEditor.o $(HASH)

testing1: ./Test/test1.c blockChain.o peepEditor.o
	$(CC) $(CFLAGS) -o test1 ./Test/test1.c blockChain.o peepEditor.o $(HASH)

testing2: ./Test/test2.c blockChain.o peepEditor.o
	$(CC) $(CFLAGS) -o test2 ./Test/test2.c blockChain.o peepEditor.o $(HASH)

testing3: ./Test/test3.c blockChain.o peepEditor.o
	$(CC) $(CFLAGS) -o test3 ./Test/test3.c blockChain.o peepEditor.o $(HASH)  

testing4: ./Test/test4.c blockChain.o peepEditor.o
	$(CC) $(CFLAGS) -o test4 ./Test/test4.c blockChain.o peepEditor.o $(HASH)

test: ./Test/test5.c blockChain.o peepEditor.o
	$(CC) $(CFLAGS) -o test5 ./Test/test5.c blockChain.o peepEditor.o $(HASH)

peepEditor.o: peepEditor.c peepEditor.h
	$(CC) $(CFLAGS) -c peepEditor.c  $(HASH)


blockChain.o: blockChain.c blockChain.h
	$(CC) $(CFLAGS) -c blockChain.c 

clean:
	rm -f *.o;rm test1;rm test2;rm test3;rm test4;rm peep

#testing done here
test1: test1
	./test1

test2: test2
	./test2

#make tarball
A3.tar.gz: peep.c peepEditor.* blockChain.*  makefile Test/
	tar -czvf A3.tar.gz peep.c peepEditor.* blockChain.* makefile Test/
#to uncompress
#tar -zcvf A3.tar.gz
