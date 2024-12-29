CC = gcc

CFLAGS = -c -g -Wall

SRCDIR=src

SRCS := $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))

DESTDIR ?= /

all: sl

sl: $(OBJS)
	$(CC) -o $@ $^$(LDFLAGS)

$(SRCDIR)/%.o:$(SRCDIR)/%.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(SRCDIR)/*.o sl