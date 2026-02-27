#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <optional>
#include <chrono>

namespace executor {

enum class Target {
    WSL,
    PowerShell
};

enum class Mode {
    Safe,
    Raw
};

enum class ParamType {
    String,
    Int
};

struct ParamValue {
    ParamType type;
    std::string string_val;
    int int_val;
};

struct Command {
    std::string name;
    Target target;
    std::map<std::string, ParamValue> params;
    int timeout_ms = 5000;
    Mode mode = Mode::Safe;
    std::string description;
};

struct ExecResult {
    int exit_code;
    std::string stdout_str;
    std::string stderr_str;
    int64_t duration_ms;
    std::string command_name;
    std::string target;
};

class IProcessRunner {
public:
    virtual ~IProcessRunner() = default;
    virtual ExecResult execute(const Command& cmd) = 0;
};

// Factory function
std::unique_ptr<IProcessRunner> create_runner();

} // namespace executor
