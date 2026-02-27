# Security

## 1. Input Validation
-   **Strict DSL**: No complex control flow (loops, variables) to prevent malicious constructs.
-   **Parameter Typing**: All parameters must be explicitly typed (e.g., `string`, `int`).
-   **Semantic Checks**: Invalid types, unknown targets, or disallowed combinations are rejected immediately.

## 2. Safe Mode vs. Raw Mode
-   **Safe Mode (Default)**:
    -   All parameters are sanitized.
    -   Suspicious characters (`;`, `&`, `|`, `>`) are disallowed unless explicitly typed and quoted.
    -   Command injection is mitigated by strict argument handling.
-   **Raw Mode (Opt-In)**:
    -   Only allowed if explicitly requested via the `--allow-raw` flag.
    -   Allows commands to be passed as-is (useful for complex pipelines).
    -   **Warning**: Users must be aware of the risks.

## 3. Runtime Isolation
-   **Process Creation**: Child processes are spawned using dedicated APIs (`CreateProcess`).
-   **Timeouts**: Execution is strictly limited by the `timeout` parameter to prevent hanging processes.
-   **Cleanup**: Zombie processes are cleaned up automatically.

## 4. Logging
-   All executions (successful or failed) are logged with:
    -   Timestamp
    -   Command executed (sanitized)
    -   Exit code
    -   Duration
    -   Mode used
-   Logs can be used for auditing purposes.
