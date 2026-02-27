#include "parser.hpp"
#include <stdexcept>
#include <iostream>
#include <vector>
#include <string>

namespace executor {

Parser::Parser(const std::vector<Token>& tokens) : tokens_(tokens) {}

const Token& Parser::peek() const {
    if (pos_ >= tokens_.size()) {
        static Token eof{TokenType::END_OF_FILE, "", 0, 0};
        return eof;
    }
    return tokens_[pos_];
}

const Token& Parser::advance() {
    if (pos_ < tokens_.size()) {
        return tokens_[pos_++];
    }
    return peek();
}

bool Parser::match(TokenType type) {
    if (peek().type == type) {
        advance();
        return true;
    }
    return false;
}

void Parser::consume(TokenType type, const std::string& message) {
    if (peek().type == type) {
        advance();
    } else {
        throw std::runtime_error(message + ". Found: " + peek().value + " (" + std::to_string((int)peek().type) + ")");
    }
}

// Forward declarations of helper methods
// In a real scenario, these would be member functions. I'll define them as such.

void Parser::parse_target(Command& cmd) {
    consume(TokenType::TARGET, "Expected 'target'");
    consume(TokenType::COLON, "Expected ':'");

    if (peek().type == TokenType::WSL) {
        cmd.target = Target::WSL;
        advance();
    } else if (peek().type == TokenType::POWERSHELL) {
        cmd.target = Target::PowerShell;
        advance();
    } else {
        throw std::runtime_error("Expected 'wsl' or 'powershell'");
    }
}

void Parser::parse_params(Command& cmd) {
    consume(TokenType::PARAMS, "Expected 'params'");
    consume(TokenType::COLON, "Expected ':'");
    consume(TokenType::LBRACKET, "Expected '['");

    if (peek().type != TokenType::RBRACKET) {
        while (true) {
            std::string param_name = peek().value;
            consume(TokenType::IDENT, "Expected parameter name");
            consume(TokenType::COLON, "Expected ':'");

            ParamType type;
            if (peek().value == "string") {
                type = ParamType::String;
                advance();
            } else if (peek().value == "int") {
                type = ParamType::Int;
                advance();
            } else {
                throw std::runtime_error("Expected 'string' or 'int'");
            }

            consume(TokenType::EQUALS, "Expected '='");

            ParamValue val;
            val.type = type;

            if (type == ParamType::String) {
                val.string_val = peek().value;
                consume(TokenType::STRING, "Expected string value");
            } else {
                val.int_val = std::stoi(peek().value);
                consume(TokenType::NUMBER, "Expected integer value");
            }

            cmd.params[param_name] = val;

            if (peek().type == TokenType::COMMA) {
                advance();
            } else {
                break;
            }
        }
    }

    consume(TokenType::RBRACKET, "Expected ']'");
}

void Parser::parse_timeout(Command& cmd) {
    consume(TokenType::TIMEOUT, "Expected 'timeout'");
    consume(TokenType::COLON, "Expected ':'");
    cmd.timeout_ms = std::stoi(peek().value);
    consume(TokenType::NUMBER, "Expected timeout value");
}

void Parser::parse_mode(Command& cmd) {
    consume(TokenType::MODE, "Expected 'mode'");
    consume(TokenType::COLON, "Expected ':'");
    if (peek().type == TokenType::SAFE) {
        cmd.mode = Mode::Safe;
        advance();
    } else if (peek().type == TokenType::RAW) {
        cmd.mode = Mode::Raw;
        advance();
    } else {
        throw std::runtime_error("Expected 'safe' or 'raw'");
    }
}

void Parser::parse_description(Command& cmd) {
    // Only if current token is "description" (IDENT)
    if (peek().type == TokenType::IDENT && peek().value == "description") {
        advance(); // consume "description"
        consume(TokenType::COLON, "Expected ':'");
        cmd.description = peek().value;
        consume(TokenType::STRING, "Expected description string");
    }
}

void Parser::parse_command_body(Command& cmd) {
    parse_target(cmd);
    parse_params(cmd);

    if (peek().type == TokenType::TIMEOUT) {
        parse_timeout(cmd);
    }

    if (peek().type == TokenType::MODE) {
        parse_mode(cmd);
    }

    if (peek().type == TokenType::IDENT && peek().value == "description") {
        parse_description(cmd);
    }
}

Command Parser::parse_command() {
    consume(TokenType::COMMAND, "Expected 'command'");

    std::string name = peek().value;
    consume(TokenType::IDENT, "Expected command name");

    consume(TokenType::COLON, "Expected ':' after command name");

    Command cmd;
    cmd.name = name;

    parse_command_body(cmd);

    return cmd;
}

std::vector<Command> Parser::parse() {
    std::vector<Command> commands;
    while (peek().type != TokenType::END_OF_FILE) {
        if (peek().type == TokenType::COMMAND) {
            commands.push_back(parse_command());
        } else {
            throw std::runtime_error("Unexpected token at top level: " + peek().value);
        }
    }
    return commands;
}

ParamValue Parser::parse_param_value() {
    return {}; // Unused helper
}

} // namespace executor
