CC := /usr/bin/gcc
CFLAGS := -Wall -Wextra -O3 -I/usr/local/include
LDFLAGS := -lflint

ARCH := $(shell uname -m)
ifeq ($(ARCH),x86_64)
	CFLAGS += -mavx2
	ifeq ($(shell grep -q '\<avx512' /proc/cpuinfo && echo yes),yes)
		CFLAGS += -mavx512f
	endif
else ifeq ($(ARCH),aarch64)
	CFLAGS += -march=armv8-a+simd
endif

SRCDIR := src
OBJDIR := obj
TARGET := pcca7

SRCS := $(shell find $(SRCDIR) -name "*.c")
OBJS := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))
COREOBJS := $(filter-out $(OBJDIR)/main.o,$(OBJS))

TESTSRCS := tests/test_prime.c tests/test_param.c tests/test_shoup.c tests/test_equality.c tests/test_options.c
TESTOBJS := $(patsubst tests/%.c,tests/obj/%,$(TESTSRCS))

.PHONY: all check clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

tests/obj/%: tests/%.c $(COREOBJS)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(SRCDIR) -o $@ $< $(COREOBJS) $(LDFLAGS)

check: $(TESTOBJS)
	@failed=0; \
	for test in $(TESTOBJS); do \
		./$$test || failed=1; \
	done; \
	exit $$failed

clean:
	rm -rf $(OBJDIR)
	rm -f $(TARGET)
	rm -rf tests/obj
	rm -rf latex
