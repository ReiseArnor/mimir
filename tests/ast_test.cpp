#include "../src/interpreter/ast.h"
#include "../src/interpreter/token.h"
#include "catch2/catch_test_macros.hpp"
#include <string>
#include <vector>
using namespace std;
using namespace ast;

TEST_CASE("Let statement", "[ast]")
{
  string let = "variable";
  string var1 = "mi_var";
  string var2 = "otra_var";
  Program program(vector<Statement *>{
      new LetStatement(Token(TokenType::LET, let),
                       new Identifier(Token(TokenType::IDENT, var1), var1),
                       new Identifier(Token(TokenType::IDENT, var2), var2))});

  string program_str = program.to_string();

  REQUIRE(program_str == "variable mi_var = otra_var;");
}

TEST_CASE("Return statement", "[ast]")
{
  string return_value = "regresa";
  string expression = "100";
  Program program(vector<Statement *>{
      new ReturnStatement(Token(TokenType::RETURN, return_value),
                          new Expression(Token(TokenType::INT, expression)))});

  string program_str = program.to_string();

  REQUIRE(program_str == "regresa 100;");
}

TEST_CASE("Expression statement", "[ast]")
{
  Program program(vector<Statement *>{
      new ExpressionStatement{
          Token(TokenType::IDENT, "foo", 1, 3),
          new Identifier(Token(TokenType::IDENT, "foo", 1, 3), "foo")},
      new ExpressionStatement{
          Token(TokenType::INT, "5"),
          new Identifier(Token(TokenType::INT, "5"), "5")}});

  string program_str = program.to_string();

  REQUIRE(program_str == "foo5");
}

TEST_CASE("Assign statement", "[ast]")
{
  string var = "a";
  string value = "mi_var";
  Program program(vector<Statement *>{new AssignStatement(
      Token(TokenType::ASSIGN, "="),
      new Identifier(Token(TokenType::IDENT, "a"), "a"),
      new Expression(Token(TokenType::STRING, "mi_var", 1, 6)))});

  string program_str = program.to_string();

  REQUIRE(program_str == "a = mi_var");
}
