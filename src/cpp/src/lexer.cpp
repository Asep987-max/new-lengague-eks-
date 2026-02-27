#include "lexer.hpp"
#include <cctype>
#include <stdexcept>
#include <unordered_map>

namespace executor {

Lexer::Lexer(std::string source) : source_(std::move(source)) {}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (pos_ < source_.length()) {
        char c = peek();
        if (std::isspace(c)) {
            skip_whitespace();
        } else if (c == '#') {
            read_comment(); // Skip comment but don't produce token unless required
        } else if (std::isalpha(c) || c == '_') {
            tokens.push_back(read_ident_or_keyword());
        } else if (c == '"') {
            tokens.push_back(read_string());
        } else if (std::isdigit(c)) {
            tokens.push_back(read_number());
        } else {
            switch (c) {
                case ':': tokens.push_back(create_token(TokenType::COLON, ":")); advance(); break;
                case '[': tokens.push_back(create_token(TokenType::LBRACKET, "[")); advance(); break;
                case ']': tokens.push_back(create_token(TokenType::RBRACKET, "]")); advance(); break;
                case '{': tokens.push_back(create_token(TokenType::LBRACE, "{")); advance(); break;
                case '}': tokens.push_back(create_token(TokenType::RBRACE, "}")); advance(); break;
                case '=': tokens.push_back(create_token(TokenType::EQUALS, "=")); advance(); break;
                case ',': tokens.push_back(create_token(TokenType::COMMA, ",")); advance(); break;
                default:
                    throw std::runtime_error("Unexpected character: " + std::string(1, c));
            }
        }
    }
    tokens.push_back(create_token(TokenType::END_OF_FILE, ""));
    return tokens;
}

char Lexer::peek() const {
    if (pos_ >= source_.length()) return '\0';
    return source_[pos_];
}

char Lexer::advance() {
    if (pos_ >= source_.length()) return '\0';
    char c = source_[pos_++];
    if (c == '\n') {
        line_++;
        column_ = 1;
    } else {
        column_++;
    }
    return c;
}

void Lexer::skip_whitespace() {
    while (std::isspace(peek())) {
        advance();
    }
}

Token Lexer::create_token(TokenType type, std::string value) {
    return Token{type, std::move(value), line_, column_};
}

Token Lexer::read_ident_or_keyword() {
    int start_col = column_;
    std::string value;
    while (std::isalnum(peek()) || peek() == '_') {
        value += advance();
    }

    TokenType type = TokenType::IDENT;
    if (value == "command") type = TokenType::COMMAND;
    else if (value == "target") type = TokenType::TARGET;
    else if (value == "wsl") type = TokenType::WSL;
    else if (value == "powershell") type = TokenType::POWERSHELL;
    else if (value == "params") type = TokenType::PARAMS;
    else if (value == "timeout") type = TokenType::TIMEOUT;
    else if (value == "mode") type = TokenType::MODE;
    else if (value == "safe") type = TokenType::SAFE;
    else if (value == "raw") type = TokenType::RAW;

    return Token{type, value, line_, start_col};
}

Token Lexer::read_string() {
    int start_col = column_;
    advance(); // Skip opening quote
    std::string value;
    while (peek() != '"' && peek() != '\0') {
        if (peek() == '\\') {
            advance(); // Skip backslash
            // Very simple escape handling
            char escaped = advance();
            if (escaped == '"') value += '"';
            else if (escaped == 'n') value += '\n';
            else if (escaped == '\\') value += '\\';
            else value += escaped;
        } else {
            value += advance();
        }
    }
    if (peek() == '"') {
        advance(); // Skip closing quote
    } else {
        throw std::runtime_error("Unterminated string literal");
    }
    return Token{TokenType::STRING, value, line_, start_col};
}

Token Lexer::read_number() {
    int start_col = column_;
    std::string value;
    while (std::isdigit(peek())) {
        value += advance();
    }
    return Token{TokenType::NUMBER, value, line_, start_col};
}

Token Lexer::read_comment() {
    advance(); // Skip #
    while (peek() != '\n' && peek() != '\0') {
        advance();
    }
    return Token{TokenType::COMMENT, "", line_, column_}; // Empty value for now
}

} // namespace executor
