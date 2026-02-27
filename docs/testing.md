# Testing

## Overview
This project uses a hybrid testing strategy:
1.  **Unit Tests**: C++ unit tests run on any platform (using `Catch2`).
2.  **Integration Tests**: Python tests using `pytest`.
    -   **Windows CI**: Runs real `wsl.exe` and `powershell.exe` execution.
    -   **Linux Sandbox**: Uses mocks (`MockProcessRunner`).

## Running Tests

### 1. C++ Unit Tests
From the build directory:
```bash
ctest
```
Or run the executable directly:
```bash
./src/cpp/tests/executor_tests
```

### 2. Python Integration Tests
From the root directory:
```bash
pytest -m "not integration"  # Run non-integration tests (Linux)
pytest -m "integration"      # Run integration tests (Windows)
```

## Adding Tests

### C++ Tests
-   Add new tests in `src/cpp/tests/`.
-   Use `Catch2` assertions.
-   Re-run CMake and build.

### Python Tests
-   Add new tests in `src/py/tests/`.
-   Use `pytest` fixtures.
-   Mark integration tests with `@pytest.mark.integration`.
