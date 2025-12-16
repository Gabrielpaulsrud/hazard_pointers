CC          := gcc
STACK       ?= plain
SRC_DIR     := src
BUILD_DIR   := build
TARGET_NAME := experiment_$(STACK)
TARGET      := $(BUILD_DIR)/$(TARGET_NAME)

WARNINGS := -Wall -Wextra -Wpedantic
THREADS  := -pthread
SANFLAGS := -fsanitize=address -fno-omit-frame-pointer

GLIB_CFLAGS := $(shell pkg-config --cflags glib-2.0)
GLIB_LIBS   := $(shell pkg-config --libs glib-2.0)

CFLAGS   := -std=c11 $(WARNINGS) -O3 -MMD -MP $(THREADS) -Iinclude $(GLIB_CFLAGS)
LDFLAGS  := $(THREADS) $(GLIB_LIBS)

STACK_SRCS := $(SRC_DIR)/lock_free_stack_$(STACK).c
STACK_DEFS :=
STACK_SRCS += $(SRC_DIR)/hp.c

ifeq ($(STACK),hp)
# STACK_DEFS += -DUSE_HP_STACK
STACK_SRCS += $(SRC_DIR)/thread_data_hc.c
endif

ifeq ($(STACK),tagged)
# STACK_DEFS += -DUSE_TAGGED_STACK
STACK_SRCS += $(SRC_DIR)/thread_data_tagged.c
# STACK_SRCS += $(SRC_DIR)/h.c
endif

SRCS := experiment.c $(STACK_SRCS)
OBJS := $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

.PHONY: all run clean debug dbg asan
.PHONY: x86

# Build for gem5: static x86_64 Linux binary
.PHONY: x86

# Build on an x86_64 Linux machine for gem5 (adds libatomic for 16-byte atomics)
x86: LDFLAGS := $(LDFLAGS) -latomic -static
x86: $(TARGET)

all: $(TARGET)

run: $(TARGET)
	$(TARGET) $(ARGS)

debug: CFLAGS := $(filter-out -O2,$(CFLAGS))
debug: CFLAGS += -O0 -g -fno-omit-frame-pointer
debug: clean $(TARGET)

dbg: debug
	lldb $(TARGET)

asan: CFLAGS := $(filter-out -O2,$(CFLAGS))
asan: CFLAGS += -O1 -g $(SANFLAGS)
asan: LDFLAGS += -fsanitize=address
asan: clean $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(STACK_DEFS) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(STACK_DEFS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

-include $(DEPS)
