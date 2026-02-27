#include <catch2/catch_test_macros.hpp>
#include "parser.hpp"
#include "lexer.hpp"
#include <iostream>

using namespace executor;

TEST_CASE("Parser parses simple command", "[parser]") {
    std::string source = R"(
        command test_cmd:
            target: wsl
            params: [p1: string = "val"]
    )";

    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto commands = parser.parse();

    REQUIRE(commands.size() == 1);
    REQUIRE(commands[0].name == "test_cmd");
    REQUIRE(commands[0].target == Target::WSL);
    REQUIRE(commands[0].params.size() == 1);
    REQUIRE(commands[0].params["p1"].string_val == "val");
}

TEST_CASE("Parser handles multiple params", "[parser]") {
    std::string source = R"(
        command multi_param:
            target: powershell
            params: [
                p1: string = "s",
                p2: int = 100
            ]
    )";

    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto commands = parser.parse();

    REQUIRE(commands[0].params["p1"].type == ParamType::String);
    REQUIRE(commands[0].params["p2"].type == ParamType::Int);
    REQUIRE(commands[0].params["p2"].int_val == 100);
}

TEST_CASE("Parser handles optional fields", "[parser]") {
    std::string source = R"(
        command full_cmd:
            target: wsl
            params: []
            timeout: 9999
            mode: raw
            description: "A description"
    )";

    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto commands = parser.parse();

    REQUIRE(commands[0].timeout_ms == 9999);
    REQUIRE(commands[0].mode == Mode::Raw);
    REQUIRE(commands[0].description == "A description");
}

TEST_CASE("Parser throws on invalid syntax", "[parser]") {
    std::string source = "command bad: target: wsl params: ["; // Missing closing bracket

    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    Parser parser(tokens);

    REQUIRE_THROWS(parser.parse());
}
