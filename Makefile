ARCH := $(shell uname -m)
AVX512 := $(shell grep -q '\<avx512' /proc/cpuinfo && echo yes)

ifeq ($(ARCH),x86_64)
    CFLAGS := -Wall -Wextra -O3 -mavx2
	ifeq ($(AVX512),yes)
		CFLAGS += -mavx512f
	endif
    LDFLAGS := -lflint
else
	CFLAGS := -Wall -Wextra -O3
	LDFLAGS := -lflint
endif
CC := /usr/bin/gcc
SRC_DIR := src
OBJ_DIR := obj
TARGET := pcca7

SRCS := $(shell find $(SRC_DIR) -name "*.c")
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

LIB_OBJS := $(filter-out $(OBJ_DIR)/main.o,$(OBJS))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

TEST_SRCS := $(wildcard tests/*.c)
TEST_BINS := $(patsubst tests/%.c,tests/obj/%,$(TEST_SRCS))

tests/obj/%: tests/%.c $(LIB_OBJS)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(SRC_DIR) -o $@ $< $(LIB_OBJS) $(LDFLAGS)

check: $(TEST_BINS)
	@for test in $(TEST_BINS); do \
		./$$test || exit 1; \
	done

clean:
	rm -rf $(OBJ_DIR)
	rm -f $(TARGET)
	rm -rf tests/obj

.PHONY: all clean check