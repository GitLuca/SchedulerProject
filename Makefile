CC = gcc
CFLAGS = -Wall -ggdb3 -pedantic
LDFLAGS =-pthread


all: Main 
	

Main: Main.c Not_Preemptive.c Preemptive.c Job.h
	gcc -pthread -o simulator Job.h Main.c Not_Preemptive.c Preemptive.c 

clean: 
	find . -maxdepth 1 -type f -executable -delete