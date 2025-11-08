BUILD_DIR      := build
TARGET_NAME    := riscv_simulator
SIM            := $(BUILD_DIR)/$(TARGET_NAME)

TEST_DIR       := tests
RESULTS_DIR    := $(TEST_DIR)/results
TEST_EXT       := asm

# Auto-discover test files (only top-level .asm in tests/)
TESTS          := $(wildcard $(TEST_DIR)/*.$(TEST_EXT))
TEST_BASENAMES := $(notdir $(TESTS))
LOG_FILES      := $(patsubst %.$(TEST_EXT),$(RESULTS_DIR)/%_out.log,$(TEST_BASENAMES))

# Build type (use: make BUILD_TYPE=Release)
BUILD_TYPE ?= Release

# CMake generator options (leave empty for default)
CMAKE_ARGS ?= -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)

# --------------- Phony targets ----------------
.PHONY: all sim configure build test run clean distclean rebuild list-tests logs help

# Default target: build simulator
all: sim

# Explicit build target (alias)
sim: $(SIM)

# Configure step (only runs if build dir missing or no cache)
configure:
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake $(CMAKE_ARGS) ..

# Build the simulator (depends on configure)
$(SIM): configure
	@echo "[BUILD] Building $(TARGET_NAME) in $(BUILD_DIR) (type=$(BUILD_TYPE))"
	@cmake --build $(BUILD_DIR) --config $(BUILD_TYPE) --target $(TARGET_NAME)
	@echo "[OK] Built: $(SIM)"

# --------------- Test execution ----------------

# Ensure results dir exists
$(RESULTS_DIR):
	@mkdir -p $(RESULTS_DIR)
	@touch $(RESULTS_DIR)/.gitkeep

# Pattern rule to generate a log for each test:
# Input: tests/<name>.asm
# Output: tests/results/<name>_out.log
$(RESULTS_DIR)/%_out.log: $(TEST_DIR)/%.$(TEST_EXT) $(SIM) | $(RESULTS_DIR)
	@echo "[TEST] Running $<"
	@$(SIM) $< > $@ 2>&1 && \
	  echo "[PASS] $< -> $(notdir $@)" || \
	  echo "[FAIL] $< (see $(notdir $@))"

# Build all logs (but does not summarize)
logs: $(LOG_FILES)

# List discovered tests
list-tests:
	@echo "Discovered $(words $(TESTS)) test(s):"
	@$(foreach t,$(TESTS),echo " - $(t)";)

# Run all tests (sequential) and summary at end
test: sim
	@echo "[INFO] Running all tests..."
	@echo "[INFO] Simulator: $(SIM)"
	@echo "[INFO] Build type: $(BUILD_TYPE)"
	@echo "[INFO] Results dir: $(RESULTS_DIR)"
	@mkdir -p $(RESULTS_DIR)
	@touch $(RESULTS_DIR)/.gitkeep
	@pass=0; fail=0; \
	for t in $(TESTS); do \
	  base=$$(basename $$t .$(TEST_EXT)); \
	  log="$(RESULTS_DIR)/$${base}_out.log"; \
	  echo "[TEST] $$t"; \
	  if $(SIM) $$t > $$log 2>&1; then \
	    echo "[PASS] $$t -> $$(basename $$log)"; \
	    pass=$$((pass+1)); \
	  else \
	    echo "[FAIL] $$t -> $$(basename $$log)"; \
	    fail=$$((fail+1)); \
	  fi; \
	  echo ""; \
	done; \
	total=$$((pass+fail)); \
	echo "-----------------------------"; \
	echo "Summary: total=$$total pass=$$pass fail=$$fail"; \
	echo "Logs in: $(RESULTS_DIR)"; \
	if [ $$fail -ne 0 ]; then \
	  echo "[RESULT] Some tests failed."; \
	  exit 1; \
	else \
	  echo "[RESULT] All tests passed."; \
	fi

# Run a single test: make run TEST=<file.asm>
run: sim
	@if [ -z "$(TEST)" ]; then \
	  echo "Usage: make run TEST=<file.asm>"; \
	  echo "Files available:"; \
	  for f in $(TESTS); do echo "  - $$f"; done; \
	  exit 1; \
	fi
	@if [ ! -f "$(TEST_DIR)/$(TEST)" ]; then \
	  echo "[ERROR] Test file not found: $(TEST_DIR)/$(TEST)"; \
	  exit 1; \
	fi
	@mkdir -p $(RESULTS_DIR)
	@base=$$(basename $(TEST) .$(TEST_EXT)); \
	log="$(RESULTS_DIR)/$${base}_out.log"; \
	echo "[RUN] $(TEST)"; \
	if $(SIM) $(TEST_DIR)/$(TEST) > $$log 2>&1; then \
	  echo "[PASS] $(TEST) -> $$(basename $$log)"; \
	else \
	  echo "[FAIL] $(TEST) -> $$(basename $$log)"; \
	fi
	@echo "[VIEW] tail $(RESULTS_DIR)/$${base}_out.log"

# --------------- Housekeeping ----------------
clean:
	@echo "[CLEAN] Removing simulator target files (but keeping CMake cache)"
	@if [ -d "$(BUILD_DIR)" ]; then \
	  cmake --build $(BUILD_DIR) --target clean --config $(BUILD_TYPE) || true; \
	else \
	  echo "(nothing to clean)"; \
	fi

distclean:
	@echo "[DISTCLEAN] Removing build directory '$(BUILD_DIR)'"
	@rm -rf "$(BUILD_DIR)"

rebuild: distclean all

# --------------- Help ----------------
help:
	@echo "Available targets:"
	@echo "  make / make all      - Configure & build simulator"
	@echo "  make sim             - Build simulator"
	@echo "  make test            - Run all tests (*.asm) and summarize"
	@echo "  make run TEST=foo.asm- Run a single test"
	@echo "  make logs            - Generate logs for all tests (no summary)"
	@echo "  make list-tests      - List discovered tests"
	@echo "  make clean           - Clean build artifacts (keep cache)"
	@echo "  make distclean       - Remove build directory completely"
	@echo "  make rebuild         - Full clean then build"
	@echo "Variables:"
	@echo "  BUILD_TYPE=Release|Debug (default: $(BUILD_TYPE))"
	@echo "  TEST=<file.asm> for 'make run'"