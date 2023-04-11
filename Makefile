# ===================
# Variable definition
# ===================
MAKEFILE := $(lastword $(MAKEFILE_LIST))
DOCUMENTATION := README.md CONTRIBUTING.md

# ----------------
# Folder variables
# ----------------
SRC_DIR := src
HEADER_DIR := include
TEST_DIR := tests

BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj
TEST_BUILD_DIR := $(BUILD_DIR)/tests

# ---------------
# Target variable
# ---------------
TARGET := $(BUILD_DIR)/main

# ---------------------
# Source file variables
# ---------------------
TARGET_SRC := $(TARGET:$(BUILD_DIR)/%=$(SRC_DIR)/%.c)
SRCS := $(filter-out $(TARGET_SRC), $(shell find $(SRC_DIR) -name '*.c'))
HEADERS := $(shell find $(HEADER_DIR) -name '*.h')
TEST_SRCS := $(shell find $(TEST_DIR) -name '*.c')

# -------------------
# Byproduct variables
# -------------------
TARGET_OBJ := $(TARGET:$(BUILD_DIR)/%=$(OBJ_DIR)/%.o)
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
DEPS := $(OBJS:.o=.d) $(TEST_SRCS:$(TEST_DIR)/%.c=$(TEST_BUILD_DIR)/%.d)

.SECONDARY: $(OBJS) $(TARGET_OBJ) $(DEPS)

# --------------
# Test variables
# --------------
TEST_TARGETS := $(TEST_SRCS:$(TEST_DIR)/%.c=$(TEST_BUILD_DIR)/%)

# ----------------------
# Distribution variables
# ----------------------
DIST := Abreu-Guevara-Ortiz-Yip.tgz

# ---------------------
# Compilation variables
# ---------------------
CC := clang
CFLAGS += -Wall -Wextra -Wpedantic \
					-Wformat=2 -Wno-unused-parameter -Wshadow \
					-Wwrite-strings -Wstrict-prototypes -Wold-style-definition \
					-Wredundant-decls -Wnested-externs -Wmissing-include-dirs
CFLAGS += -std=gnu11
CPPFLAGS += -I$(HEADER_DIR) -MMD -MP
LDLIBS += -lm
LDFLAGS +=

# =================
# Compilation rules
# =================
.PHONY: all all_tests
all: $(TARGET)
all_tests: $(TEST_TARGETS)

.PHONY: dist
dist: $(DIST)

$(TARGET): $(TARGET_OBJ) $(OBJS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) $^ -o $@

$(TEST_BUILD_DIR)/%: LDLIBS += -lcriterion
$(TEST_BUILD_DIR)/%: $(TEST_DIR)/%.c $(OBJS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $(LDLIBS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

# =================
# Distribution rule
# =================
$(DIST): $(SRC_DIR) $(HEADER_DIR) $(RESOURCE_INDEX) $(RESOURCES_DIR) $(TEST_DIR) $(MAKEFILE) $(DOCUMENTATION)
	mkdir $(basename $@)
	cp -r $^ $(basename $@)
	tar -zcvf $@ $(basename $@)
	rm -r $(basename $@)

# ========================
# Directory creation rules
# ========================
$(SRC_DIR) $(HEADER_DIR) $(RESOURCES_DIR) $(TEST_DIR):
	mkdir -p $@

# ========================
# Pseudo-target definition
# ========================
.PHONY: test
test: $(TEST_TARGETS)
	for test_file in $^; do ./$$test_file; done

.PHONY: clean
clean:
	$(RM) -rf $(BUILD_DIR)
	$(RM) -f $(DIST)

.PHONY: install-hooks
install-hooks:
	pre-commit install
	pre-commit install --hook-type commit-msg

.PHONY: run-hooks
run-hooks:
	pre-commit run --all-files

.PHONY: lint
lint:
	clang-tidy $(TARGET_SRC) $(SRCS) $(HEADERS) $(TEST_SRCS)

.PHONY: format
format: $(TARGET_SRC) $(SRCS) $(HEADERS) $(TEST_SRCS)
	for file in $^; do clang-format -i $$file; done

# -------------
-include $(DEPS)
