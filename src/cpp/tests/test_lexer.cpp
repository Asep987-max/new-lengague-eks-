#include <catch2/catch_test_macros.hpp>
#include "lexer.hpp"
#include <iostream>

using namespace executor;

TEST_CASE("Lexer tokenizes basic command", "[lexer]") {
    std::string source = R"(
        command test:
            target: wsl
            params: [p1: string = "val"]
    )";
    Lexer lexer(source);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens.size() > 0);
    REQUIRE(tokens[0].type == TokenType::COMMAND);
    REQUIRE(tokens[1].value == "test");
    REQUIRE(tokens[2].type == TokenType::COLON);
    REQUIRE(tokens[3].type == TokenType::TARGET);
}

TEST_CASE("Lexer handles strings with escapes", "[lexer]") {
    std::string source = R"( "hello \"world\"" )";
    Lexer lexer(source);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens[0].type == TokenType::STRING);
    REQUIRE(tokens[0].value == "hello \"world\"");
}

TEST_CASE("Lexer handles numbers", "[lexer]") {
    std::string source = "12345";
    Lexer lexer(source);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens[0].type == TokenType::NUMBER);
    REQUIRE(tokens[0].value == "12345");
}

TEST_CASE("Lexer skips comments", "[lexer]") {
    std::string source = R"(
        # This is a comment
        target: wsl
    )";
    Lexer lexer(source);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens[0].type == TokenType::TARGET);
}
