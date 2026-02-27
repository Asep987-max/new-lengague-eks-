# Installation

## Prerequisites
-   **Windows 11**
-   **WSL** (Windows Subsystem for Linux) installed and running.
-   **Python 3.10+**
-   **CMake 3.20+**
-   **Visual Studio Build Tools** (for compiling C++ core) or MinGW (if configured).

## Build Steps

1.  **Clone Repository**:
    ```bash
    git clone <repo_url>
    cd executor-dsl
    ```

2.  **Build C++ Core**:
    ```bash
    mkdir build
    cd build
    cmake .. -G "Visual Studio 17 2022" -A x64
    cmake --build . --config Release
    ```

3.  **Install Python Tooling**:
    ```bash
    cd ..
    python -m pip install -e .[test]
    ```

## Verify Installation

Run the CLI:
```bash
dsl-exec --help
```
You should see the help message.
