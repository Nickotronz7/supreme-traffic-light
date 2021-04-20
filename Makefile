clean:
	rm *.o

cleane:
	rm init
	rm cons
	rm prod

init: source/init.c source/cJSON.c
	gcc -c source/init.c -o init.o -lrt
	gcc -c source/cJSON.c -o cJSON.o -lrt
	gcc init.o cJSON.o -o init -lrt

cons: source/cons.c source/cJSON.c
	gcc -c source/cons.c -o cons.o -lrt
	gcc -c source/cJSON.c -o cJSON.o -lrt
	gcc cons.o cJSON.o -o cons -lrt

prod: source/prod.c source/cJSON.c
	gcc -c source/prod.c -o prod.o -lrt
	gcc -c source/cJSON.c -o cJSON.o -lrt
	gcc prod.o cJSON.o -o prod -lrt

