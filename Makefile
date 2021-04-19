# output:
# 		gcc source/init.c -o init -lrt
# 		gcc source/prod.c -o prod -lrt
# 		gcc source/cons.c -o cons -lrt

# init: source/init.c headers/cJSON.h source/cJSON.c
#		gcc source/init.c -o init -lrt

clean:
	rm *.o

cleane:
	rm init

all: source/init.c source/cJSON.c
	gcc -c source/init.c -o init.o
	gcc -c source/cJSON.c -o cJSON.o
	gcc init.o cJSON.o -o init

