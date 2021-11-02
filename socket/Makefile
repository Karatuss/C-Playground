CC = gcc

VPATH = ..

INCLUDE_DIR = $(PWD) \

C_SRCS = $(notdir $(wildcard *.c))
C_OBJS = $(patsubst build/%.o, $(C_SRCS))

CFLAGS = -I $(INCLUDE_DIR) \
				 -g \
				 -pthread

PORT = 8888
result = build/server

all: $(result)

$(result): $(C_SRCS)
	mkdir -p $(shell dirname $@)
	$(CC) $(CFLAGS) -o $@ $^

run: $(result)
	@echo port: $(PORT)
	$(result) $(PORT)	

clean:
	@rm -rf build

.PHONY: all clean 
