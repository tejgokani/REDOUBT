# Redoubt build. `make help` for targets.
CC       ?= cc
CFLAGS   ?= -O2 -g
CFLAGS   += -std=c11 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Wpedantic -Werror -Iinclude
BUILD    := build

# SANITIZE=1: ASan+UBSan build (CI). HARDEN=1: stack protector + fortify + PIE (release).
ifeq ($(SANITIZE),1)
CFLAGS += -fsanitize=address,undefined -fno-sanitize-recover=undefined -fno-omit-frame-pointer
LDFLAGS += -fsanitize=address,undefined
endif
ifeq ($(HARDEN),1)
CFLAGS += -fstack-protector-strong -D_FORTIFY_SOURCE=2 -fPIE
ifneq ($(shell uname -s),Darwin)
LDFLAGS += -pie -Wl,-z,relro,-z,now
endif
endif
FORMAT_FILES := $(shell find scanner tests kmod include -name '*.c' -o -name '*.h' 2>/dev/null)

SCANNER_SRC := $(filter-out scanner/main.c,$(wildcard scanner/*.c))
SCANNER_OBJ := $(SCANNER_SRC:%.c=$(BUILD)/%.o)
MAIN_OBJ    := $(BUILD)/scanner/main.o
TEST_SRC    := $(wildcard tests/unit/*.c)
TEST_OBJ    := $(TEST_SRC:%.c=$(BUILD)/%.o)

.PHONY: all scanner test kmod testrk clean help format format-check
all: scanner

scanner: $(BUILD)/redoubt

$(BUILD)/redoubt: $(SCANNER_OBJ) $(MAIN_OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

$(BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -MP -c -o $@ $<

test: $(BUILD)/unit_tests
	$(BUILD)/unit_tests

$(BUILD)/unit_tests: $(SCANNER_OBJ) $(TEST_OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

# Kernel module: Linux only (run inside the VM, see docs/DEV_ENVIRONMENT.md).
kmod:
ifeq ($(shell uname -s),Linux)
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(CURDIR)/kmod modules
else
	@echo "kmod builds only on Linux. Use tools/vm-sync-build.sh"; exit 1
endif

testrk:
	@echo "testrk arrives in Session 2 (see docs/ROADMAP.md)"

format:
	clang-format -i $(FORMAT_FILES)

format-check:
	clang-format --dry-run --Werror $(FORMAT_FILES)

clean:
	rm -rf $(BUILD)
ifeq ($(shell uname -s),Linux)
	-$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(CURDIR)/kmod clean 2>/dev/null
endif

help:
	@echo "make scanner | test | kmod | format | format-check | clean  (SANITIZE=1, HARDEN=1)"

-include $(SCANNER_OBJ:.o=.d) $(MAIN_OBJ:.o=.d) $(TEST_OBJ:.o=.d)
