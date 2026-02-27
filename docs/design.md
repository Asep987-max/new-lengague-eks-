# Design Document

## 1. Parser Choice: Handwritten (Plan A)

### Rationale
We have chosen **Option B: Handwritten (recursive-descent) parser** as the primary implementation (Plan A).

**Reasons:**
1.  **Simplicity & Self-Containment**: It avoids the dependency on a Java Runtime Environment (JRE) for ANTLR generation, making the build process more robust and easier to manage in restricted environments (like CI runners or sandboxes).
2.  **No Code Generation Step**: Eliminate potential issues with generated code versioning or build system integration.
3.  **Performance**: Recursive descent parsers are generally faster and use less memory for small grammars like this DSL.
4.  **Control**: Allows fine-grained control over error reporting and recovery, crucial for a DSL intended for developers.

### Fallback (Plan B)
If the handwritten parser proves too complex to maintain or fails critical validation, we will revert to **Option A: ANTLR-based parser**. This would involve adding the ANTLR runtime dependency and configuring the build system to generate the parser sources.

## 2. Architecture

### Hybrid Runtime
-   **C++ Core (`ExecutorCore`)**:
    -   Responsible for parsing the DSL, validating semantics, and executing commands.
    -   Implements `IProcessRunner` interface.
    -   **Windows Implementation (`WindowsProcessRunner`)**: Uses native Windows APIs (or `std::system` wrappers initially, evolving to `CreateProcess` for timeout control) to execute `wsl.exe` or `powershell.exe`.
    -   **Linux/Mock Implementation (`MockProcessRunner`)**: Simulates execution for development and testing in non-Windows environments. It logs commands and returns predefined results.
-   **Python Tooling (`dsl-exec`)**:
    -   A CLI wrapper around the C++ executable.
    -   Handles file I/O and invokes the C++ binary.
    -   Provides a test harness for integration tests.

### Execution Flow
1.  `dsl-exec` reads `.dsl` file.
2.  `dsl-exec` invokes `executor_core` executable with the DSL content (or path).
3.  `executor_core` parses the DSL into an AST.
4.  `executor_core` validates the AST (types, targets, modes).
5.  `executor_core` selects the appropriate `IProcessRunner` (Windows or Mock).
6.  `executor_core` executes the command and returns a JSON result.
7.  `dsl-exec` formats and displays the result.

## 3. Risk Analysis

| Risk | Likelihood | Impact | Mitigation |
| :--- | :--- | :--- | :--- |
| **ANTLR Tooling Unavailable** | Medium | High | **Selected Plan A (Handwritten)** to avoid this entirely. |
| **Complex Grammar Evolution** | Low | Medium | The DSL is strictly scoped (no loops, no vars). If it grows, we can switch to ANTLR (Plan B). |
| **Windows API Complexity** | Medium | High | Use `IProcessRunner` abstraction. Implement strict timeout/cancellation logic. rigorous unit tests for the Windows adapter. |
| **CI Environment Limits** | High | High | Heavy reliance on **Mocks** for logic verification. Real integration tests only run on Windows runners. |
| **Injection Vulnerabilities** | Medium | High | **Safe Mode** by default. Strict allow-list for characters. Parameter typing. Detailed security tests. |

## 4. Mock Strategy

-   **Development**: default to `MockProcessRunner` on Linux.
-   **CI (Windows)**: Use `WindowsProcessRunner` by default.
-   **Testing**: Unit tests can force `MockProcessRunner` to verify logic without spawning processes.
-   **Verification**:
    -   Lexer/Parser/AST: Verified via C++ unit tests (cross-platform).
    -   Semantics: Verified via C++ unit tests.
    -   Execution Logic: Verified via `MockProcessRunner` (output checking).
    -   Real Execution: Verified via Integration Tests on Windows CI only.
