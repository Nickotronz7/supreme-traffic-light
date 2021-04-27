all: source/prod.c source/init.c source/cons.c source/cJSON.c
	gcc -c source/prod.c -o prod.o -lrt
	gcc -c source/cJSON.c -o cJSON.o -lrt
	gcc -c source/expo.c -o expo.o -lrt -lm
	gcc prod.o cJSON.o expo.o -o prod -lrt -lpthread -lm
	gcc -c source/cons.c -o cons.o -lrt
	gcc -c source/cJSON.c -o cJSON.o -lrt
	gcc -c source/poisson.c -o poisson.o -lrt -lm
	gcc cons.o cJSON.o poisson.o -o cons -lrt -lpthread -lm
	gcc -c source/init.c -o init.o -lrt
	gcc -c source/cJSON.c -o cJSON.o -lrt
	gcc init.o cJSON.o -o init -lrt -lpthread
	gcc -c source/terminator.c -o terminator.o -lrt
	gcc -c source/cJSON.c -o cJSON.o -lrt
	gcc terminator.o cJSON.o -o terminator -lrt -lpthread
	rm *.o

init: source/init.c source/cJSON.c
	gcc -c source/init.c -o init.o -lrt
	gcc -c source/cJSON.c -o cJSON.o -lrt
	gcc init.o cJSON.o -o init -lrt -lpthread
	rm *.o

cons: source/cons.c source/cJSON.c
	gcc -c source/cons.c -o cons.o -lrt
	gcc -c source/cJSON.c -o cJSON.o -lrt
	gcc -c source/poisson.c -o poisson.o -lrt -lm
	gcc cons.o cJSON.o poisson.o -o cons -lrt -lpthread -lm
	rm *.o

prod: source/prod.c source/cJSON.c
	gcc -c source/prod.c -o prod.o -lrt 
	gcc -c source/cJSON.c -o cJSON.o -lrt
	gcc -c source/expo.c -o expo.o -lrt -lm
	gcc prod.o cJSON.o expo.o -o prod -lrt -lpthread -lm
	rm *.o

ter: source/terminator.c
	gcc -c source/terminator.c -o terminator.o -lrt
	gcc -c source/cJSON.c -o cJSON.o -lrt
	gcc terminator.o cJSON.o -o terminator -lrt -lpthread
	rm *.o

clean:
	rm init
	rm cons
	rm prod
	rm terminator
