clean:
	rm init
	rm cons
	rm prod
	rm terminator

init: source/init.c source/cJSON.c
	gcc -c source/init.c -o init.o -lrt
	gcc -c source/cJSON.c -o cJSON.o -lrt
	gcc init.o cJSON.o -o init -lrt -lpthread
	rm *.o

cons: source/cons.c source/cJSON.c
	gcc -c source/cons.c -o cons.o -lrt
	gcc -c source/cJSON.c -o cJSON.o -lrt
	gcc cons.o cJSON.o -o cons -lrt -lpthread
	rm *.o

prod: source/prod.c source/cJSON.c
	gcc -c source/prod.c -o prod.o -lrt 
	gcc -c source/cJSON.c -o cJSON.o -lrt
	gcc prod.o cJSON.o -o prod -lrt -lpthread
	rm *.o

ter: source/terminator.c
	gcc -c source/terminator.c -o terminator.o -lrt
	gcc terminator.o -o terminator -lrt -lpthread
	rm *.o

all: source/prod.c source/init.c source/cons.c source/cJSON.c
	gcc -c source/prod.c -o prod.o -lrt
	gcc -c source/cJSON.c -o cJSON.o -lrt
	gcc prod.o cJSON.o -o prod -lrt -lpthread
	gcc -c source/cons.c -o cons.o -lrt
	gcc -c source/cJSON.c -o cJSON.o -lrt
	gcc cons.o cJSON.o -o cons -lrt -lpthread
	gcc -c source/init.c -o init.o -lrt
	gcc -c source/cJSON.c -o cJSON.o -lrt
	gcc init.o cJSON.o -o init -lrt -lpthread
	gcc -c source/terminator.c -o terminator.o -lrt
	gcc terminator.o -o terminator -lrt -lpthread
	rm *.o
