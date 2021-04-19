# output:
# 		gcc source/init.c -o init -lrt
# 		gcc source/prod.c -o prod -lrt
# 		gcc source/cons.c -o cons -lrt

# init: source/init.c headers/cJSON.h source/cJSON.c
#		gcc source/init.c -o init -lrt

# clean:
# 	rm init
# 	rm prod
# 	rm cons


# HDR_DIR = headers
# SRC_DIR = source
# OBJ_DIR = objects

# CFLAGS = -c -Wall -I.
# SRCS = $(SRC_DIR)/init.c $(SRC_DIR)/cJSON.c
# OBJS = $(OBJ_DIR)/init.o

# DEPS = $(HDR_DIR)/cJSON.h

# all: init

# init: $(OBJS)
# 	$(CC) $(OBJS) $(CFLAGS) -o init

# $(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
# 	$(CC) $(CFLAGS) -c $< -o $@

