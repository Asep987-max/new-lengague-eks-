#pragma once
#include <string>
#include <vector>
#include <optional>
#include <iostream>

namespace executor {

enum class TokenType {
    COMMAND, IDENT, COLON, TARGET, PARAMS, TIMEOUT, MODE, COMMENT,
    STRING, NUMBER, LBRACKET, RBRACKET, LBRACE, RBRACE, EQUALS, COMMA,
    WSL, POWERSHELL, SAFE, RAW, END_OF_FILE, UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
};

class Lexer {
public:
    explicit Lexer(std::string source);
    std::vector<Token> tokenize();

private:
    std::string source_;
    size_t pos_ = 0;
    int line_ = 1;
    int column_ = 1;

    char peek() const;
    char advance();
    bool match(char expected);
    void skip_whitespace();
    Token create_token(TokenType type, std::string value);
    Token read_ident_or_keyword();
    Token read_string();
    Token read_number();
    Token read_comment();
};

} // namespace executor
