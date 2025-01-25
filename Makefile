CC = gcc

CFLAGS = -c -g -Wall -Iexternal/yyjson/src/ 
#CFLAGS += -fsanitize=undefined
#CFLAGS += -fsanitize=address

SRCDIR=src

SRCS := $(wildcard $(SRCDIR)/*.c) external/yyjson/src/yyjson.c
OBJS = $(patsubst %.c,%.o,$(SRCS))

LDFLAGS = -lm

#LDFLAGS += -fsanitize=undefined
#LDFLAGS += -fsanitize=address

DESTDIR ?= /
PREFIX ?= $(DESTDIR)/usr

all: sl

sl: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

external/yyjson/src/yyjson.o:
	$(CC) $(CFLAGS) -DYYJSON_DISABLE_READER -DYYJSON_DISABLE_UTILS -DYYJSON_DISABLE_FAST_FP_CONV -DYYJSON_DISABLE_NON_STANDARD -DYYJSON_DISABLE_UTF8_VALIDATION -o $@ external/yyjson/src/yyjson.c

$(SRCDIR)/%.o:$(SRCDIR)/%.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(SRCDIR)/*.o sl

all-clean: clean
	rm -f external/yyjson/src/yyjson.o

onefetch:
	onefetch --exclude external/*

install:
	cp sl $(PREFIX)/bin