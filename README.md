# Executor DSL

A minimal, secure DSL for executing shell commands in WSL or PowerShell on Windows 11.

## Features
- **Deterministic**: Strict grammar, no unexpected behavior.
- **Secure**: Safe mode by default, typed parameters, injection protection.
- **Hybrid**: C++ Core for execution, Python for tooling.
- **Targets**: `wsl` and `powershell`.

## Usage

1. Create a `.dsl` file:
   ```yaml
   command list_files:
       target: wsl
       params: [path: string = "."]
   ```

2. Run with CLI:
   ```bash
   dsl-exec run my_script.dsl
   ```

## Development

### Prerequisites
- CMake 3.20+
- C++17 Compiler
- Python 3.10+

### Building (Linux/Mac - Mock Mode)
```bash
mkdir build && cd build
cmake ../src/cpp
cmake --build .
ctest
```

### Building (Windows)
```bash
cmake -S src/cpp -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```
