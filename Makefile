#!/usr/bin/make -f
.PHONY: all clean

TARGET = mog

# add encoder sources
SRCS        += source/encoder/flac.c
SRCS        += source/encoder/opus.c

# add spu sources
SRCS        += source/spu/spu.c

# add sound sources
SRCS        += source/sound/init.c
SRCS        += source/sound/driver.c
SRCS        += source/sound/command.c
SRCS        += source/sound/sequence.c

# add main sources
SRCS        += source/mog.c

OBJS        = $(SRCS:.c=.o)

# get paths from pkg-config
OPUS_INCDIR := $(shell pkg-config --cflags opus)

CFLAGS      = --std=gnu99 -O2 -Wall -Wextra -Wshadow -Isource $(OPUS_INCDIR)
#CFLAGS      += -g -O0 -fsanitize=address,undefined

LDFLAGS     = -lFLAC -lopusenc

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	-$(RM) $(TARGET)
	-$(RM) $(OBJS)
