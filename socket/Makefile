CC = gcc

VPATH = ..

INCLUDE_DIR = $(PWD) 

EXCLUDE_FILE = my_socket.h my_socket.c old_launcher.c	\
							 client.h client.c

C_SRCS := $(notdir $(wildcard *.c))
C_SRCS := $(filter-out $(EXCLUDE_FILE), $(C_SRCS))
C_OBJS := $(patsubst build/%.o, %.c, $(C_SRCS))

CFLAGS = -I $(INCLUDE_DIR) \
				 -g \
				 -pthread

PORT = 8888
launcher = build/launcher
client = build/client

all: $(launcher) $(client)

run: $(launcher) $(client)
	# @echo port: $(PORT)
	$(launcher) 

$(launcher): $(C_SRCS)
	mkdir -p $(shell dirname $@)
	$(CC) $(CFLAGS) -o $@ $^

$(client): client.c
	mkdir -p $(shell dirname $@)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	@rm -rf build

.PHONY: all clean 
