output: init.c
		gcc init.c -o init -lrt
		gcc prod.c -o prod -lrt
		gcc cons.c -o cons -lrt

clean:
	rm init
	rm prod
	rm cons