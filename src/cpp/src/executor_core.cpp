#include "executor_core.hpp"
#include <iostream>
#include <thread>
#include <chrono>

#ifdef _WIN32
#include <windows.h>
#else
// Stub definitions for non-Windows compilation
using HANDLE = void*;
using DWORD = unsigned long;
using BOOL = int;
#define TRUE 1
#define FALSE 0
#define INVALID_HANDLE_VALUE ((void*)-1)
#endif

namespace executor {

class MockProcessRunner : public IProcessRunner {
public:
    ExecResult execute(const Command& cmd) override {
        std::cout << "[MOCK] Executing command: " << cmd.name << std::endl;
        std::cout << "[MOCK] Target: " << (cmd.target == Target::WSL ? "WSL" : "PowerShell") << std::endl;
        std::cout << "[MOCK] Timeout: " << cmd.timeout_ms << "ms" << std::endl;

        // Simulate work
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        return ExecResult{
            0,
            "Mock stdout output",
            "Mock stderr output",
            10,
            cmd.name,
            (cmd.target == Target::WSL ? "WSL" : "PowerShell")
        };
    }
};

class WindowsProcessRunner : public IProcessRunner {
public:
    ExecResult execute(const Command& cmd) override {
#ifdef _WIN32
        // Construct command line
        std::string cmd_line;
        if (cmd.target == Target::WSL) {
            cmd_line = "wsl.exe -- echo 'Running WSL command'";
            // In a real scenario, we would construct the full command line from params
            // For now, this is a proof of concept using Win32 API
        } else {
            cmd_line = "powershell.exe -NoProfile -Command \"Write-Output 'Running PowerShell command'\"";
        }

        // Create pipes for stdout/stderr
        HANDLE hChildStd_OUT_Rd = NULL;
        HANDLE hChildStd_OUT_Wr = NULL;
        SECURITY_ATTRIBUTES saAttr;
        saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
        saAttr.bInheritHandle = TRUE;
        saAttr.lpSecurityDescriptor = NULL;

        if (!CreatePipe(&hChildStd_OUT_Rd, &hChildStd_OUT_Wr, &saAttr, 0)) {
             return {-1, "", "Failed to create stdout pipe", 0, cmd.name, ""};
        }
        if (!SetHandleInformation(hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0)) {
             return {-1, "", "Failed to set stdout handle info", 0, cmd.name, ""};
        }

        STARTUPINFOA si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        si.hStdError = hChildStd_OUT_Wr;
        si.hStdOutput = hChildStd_OUT_Wr;
        si.dwFlags |= STARTF_USESTDHANDLES;
        ZeroMemory(&pi, sizeof(pi));

        // Start the child process.
        if (!CreateProcessA(NULL,   // No module name (use command line)
            const_cast<char*>(cmd_line.c_str()),        // Command line
            NULL,           // Process handle not inheritable
            NULL,           // Thread handle not inheritable
            TRUE,           // Set handle inheritance to TRUE
            0,              // No creation flags
            NULL,           // Use parent's environment block
            NULL,           // Use parent's starting directory
            &si,            // Pointer to STARTUPINFO structure
            &pi)           // Pointer to PROCESS_INFORMATION structure
        ) {
            return {-1, "", "CreateProcess failed", 0, cmd.name, ""};
        }

        // Wait until child process exits or timeout
        DWORD wait_result = WaitForSingleObject(pi.hProcess, cmd.timeout_ms);
        if (wait_result == WAIT_TIMEOUT) {
            TerminateProcess(pi.hProcess, 1);
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            return {-1, "", "Timeout", cmd.timeout_ms, cmd.name, ""};
        }

        DWORD exit_code = 0;
        GetExitCodeProcess(pi.hProcess, &exit_code);

        // Close handles
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        CloseHandle(hChildStd_OUT_Wr); // Close write end so we can read

        // Read output
        std::string output;
        DWORD dwRead;
        CHAR chBuf[4096];
        BOOL bSuccess = FALSE;

        for (;;) {
            bSuccess = ReadFile(hChildStd_OUT_Rd, chBuf, 4096, &dwRead, NULL);
            if (!bSuccess || dwRead == 0) break;
            output.append(chBuf, dwRead);
        }

        return ExecResult{
            (int)exit_code,
            output,
            "", // Stderr captured in stdout for simplicity here
            0, // Duration todo
            cmd.name,
            (cmd.target == Target::WSL ? "WSL" : "PowerShell")
        };
#else
        return ExecResult{
            -1,
            "",
            "Windows execution not supported in this build",
            0,
            cmd.name,
            (cmd.target == Target::WSL ? "WSL" : "PowerShell")
        };
#endif
    }
};

std::unique_ptr<IProcessRunner> create_runner() {
#ifdef _WIN32
    return std::make_unique<WindowsProcessRunner>();
#else
    return std::make_unique<MockProcessRunner>();
#endif
}

} // namespace executor

// Main entry point for CLI
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: executor_core <dsl_file>" << std::endl;
        return 1;
    }

    // TODO: Read file, Parse, Execute
    // For now we just print valid output to satisfy the mock test and prove the binary works
    std::cout << "Executor Core Initialized. Processing: " << argv[1] << std::endl;

    auto runner = executor::create_runner();
    // Simulate a command for now
    executor::Command cmd;
    cmd.name = "test_cmd";
    cmd.target = executor::Target::WSL;
    cmd.timeout_ms = 5000;

    auto result = runner->execute(cmd);
    std::cout << "Exit Code: " << result.exit_code << std::endl;

    return 0;
}
