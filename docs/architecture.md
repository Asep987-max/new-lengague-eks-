# Architecture

## Overview
This system provides a minimal, secure DSL for executing shell commands in WSL or PowerShell on Windows 11. It consists of a C++ core runtime (`executor_core`) and Python tooling (`dsl-exec`).

## Components

### 1. `executor_core` (C++)
- **Lexer/Parser**: Handwritten recursive-descent parser. Produces an Abstract Syntax Tree (AST).
- **Semantic Validator**: Checks types, targets, and enforces security constraints (e.g., safe mode by default).
- **Execution Engine**:
  - `IProcessRunner`: Interface for executing commands.
  - `WindowsProcessRunner`: Uses Win32 APIs (CreateProcess) to spawn `wsl.exe` or `powershell.exe`. Handles timeouts and output capture.
  - `MockProcessRunner`: For testing and non-Windows environments. Simulates execution.

### 2. `dsl-exec` (Python)
- **CLI Wrapper**: `argparse`-based CLI.
- **Invocation**: Calls `executor_core` with the DSL file and arguments.
- **Result Parsing**: Parses the JSON output from `executor_core` and presents it to the user.

## Data Flow
User -> `dsl-exec` -> `executor_core` -> [Parse -> Validate -> Execute] -> JSON Result -> `dsl-exec` -> User
