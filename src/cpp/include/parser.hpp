#pragma once
#include "lexer.hpp"
#include "executor_core.hpp"
#include <vector>
#include <map>
#include <memory>

namespace executor {

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);
    std::vector<Command> parse();

private:
    const std::vector<Token>& tokens_;
    size_t pos_ = 0;

    const Token& peek() const;
    const Token& advance();
    bool match(TokenType type);
    void consume(TokenType type, const std::string& message);

    Command parse_command();
    void parse_command_body(Command& cmd);
    void parse_target(Command& cmd);
    void parse_params(Command& cmd);
    void parse_timeout(Command& cmd);
    void parse_mode(Command& cmd);
    void parse_description(Command& cmd);
    ParamValue parse_param_value();
};

} // namespace executor
