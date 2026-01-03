# RISC-V Assembly Simulator

This project is an educational RISC-V assembly simulator written in C. It demonstrates the internal stages of instruction processing, from assembly parsing and encoding to execution and state updates, using a simplified RV32I-style architecture.

The project uses CMake as the underlying build system and provides a Makefile as a convenience interface for building, testing, and managing the simulator.

---

## Requirements

- C compiler with C11 support (e.g., GCC or Clang)
- CMake (version 3.x or newer)
- Make

---

## Build System Overview

CMake is used as the primary build system generator, while the Makefile acts as a thin wrapper that standardizes common development workflows into simple and easy-to-remember `make` commands. The Makefile does not replace CMake; instead, it simplifies interaction with the build system while preserving portability and configurability.

All builds are performed out-of-source, using a dedicated `build/` directory. The simulator executable is generated as `build/riscv_simulator`.

The default build type is `Release`, which produces an optimized binary. A `Debug` build can be selected at invocation time to enable debug symbols.

---

## Building the Project

After cloning the repository, the simulator can be configured and built using:

```bash
make
```

This command performs the necessary CMake configuration step if required and builds the simulator executable. The same behavior can be invoked explicitly using:

```bash
make all
```

To explicitly build only the simulator target, the following command can be used:

```bash
make sim
```

To build the project with debug symbols enabled, the build type can be specified as follows:

```bash
make BUILD_TYPE=Debug
```

Internally, the Makefile invokes CMake with the selected build type, performing the equivalent of running:

```bash
cmake -DCMAKE_BUILD_TYPE=<type> ..
```

...inside the `build/` directory.

---

## Running Tests

Test programs are provided as `.asm` files located in the `tests/` directory. All test files are discovered automatically by the Makefile.

To list all detected test programs, use:

```bash
make list-tests
```

To build the simulator and run the full test suite, use:

```bash
make test
```

This command executes all discovered test programs, stores their output in `tests/results/<test>_out.log` files, prints a summary of passed and failed tests, and returns a non-zero exit code if any test fails. This behavior makes the command suitable for both local testing and automated environments.

To execute a single test program for focused debugging, use:

```bash
make run TEST=<file.asm>
```

The output of the test is written to the corresponding log file in the `tests/results/` directory.

To generate log files for all tests without producing a final summary, use:

```bash
make logs
```

---

## Cleaning and Rebuilding

To remove compiled objects and intermediate files while preserving the build configuration and CMake cache, use:

```bash
make clean
```

To completely remove the `build/` directory and force a fresh configuration on the next build, use:

```bash
make distclean
```

To perform a full clean rebuild from scratch, use:

```bash
make rebuild
```

---

## Help

For a concise overview of all available Makefile targets and supported variables, use:

```bash
make help
```

---

## Notes

- All build artifacts are kept outside the source tree to maintain a clean repository.
- Test output logs are stored persistently under `tests/results/` for inspection and debugging.
- The Makefile is intended as a convenience layer; advanced users may invoke CMake directly if desired.

---

## License

This project was developed for educational purposes.