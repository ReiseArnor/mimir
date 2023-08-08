#ifndef TOKEN_H
#define TOKEN_H
#include "fmt/format.h"
#include "utils.h"
#include <algorithm>
#include <array>
#include <ostream>
#include <string>

enum class TokenType {
  ASSIGN,
  COMMA,
  /* NOLINT */ _EOF,
  FUNCTION,
  LOOP,
  IDENT,
  ILLEGAL,
  INT,
  LBRACE,
  LET,
  LPAREN,
  PLUS,
  RBRACE,
  RPAREN,
  SEMICOLON,
  LT,
  GT,
  MINUS,
  DIVISION,
  MULTIPLICATION,
  NEGATION,
  /* NOLINT */ _TRUE,
  /* NOLINT */ _FALSE,
  /* NOLINT */ _NULL,
  IF,
  ELSE,
  RETURN,
  EQ,
  NOT_EQ,
  STRING
};

static constexpr std::array<NameValuePair<TokenType>, 30> tokens_enums_strings{
    {{TokenType::ASSIGN, "ASSIGN"},
     {TokenType::COMMA, "COMMA\t"},
     {TokenType::_EOF, "EOF\t"},
     {TokenType::FUNCTION, "FUNCTION"},
     {TokenType::LOOP, "LOOP"},
     {TokenType::IDENT, "IDENT\t"},
     {TokenType::ILLEGAL, "ILLEGAL"},
     {TokenType::INT, "INT\t"},
     {TokenType::LBRACE, "LBRACE"},
     {TokenType::LET, "LET\t"},
     {TokenType::LPAREN, "LPAREN"},
     {TokenType::PLUS, "PLUS\t"},
     {TokenType::RBRACE, "RBRACE"},
     {TokenType::RPAREN, "RPAREN"},
     {TokenType::SEMICOLON, "SEMICOLON"},
     {TokenType::LT, "LT\t"},
     {TokenType::GT, "GT\t"},
     {TokenType::MINUS, "MINUS\t"},
     {TokenType::DIVISION, "DIVISION"},
     {TokenType::MULTIPLICATION, "MULTIPLICATION"},
     {TokenType::NEGATION, "NEGATION"},
     {TokenType::_TRUE, "TRUE\t"},
     {TokenType::_FALSE, "FALSE\t"},
     {TokenType::_NULL, "NULL\t"},
     {TokenType::IF, "IF\t"},
     {TokenType::ELSE, "ELSE\t"},
     {TokenType::RETURN, "RETURN"},
     {TokenType::EQ, "EQ\t"},
     {TokenType::NOT_EQ, "NOT_EQ"},
     {TokenType::STRING, "STRING"}}};

class Token {
public:
  std::string literal;
  TokenType token_type;
  int line;

  Token() = default;
  Token(const TokenType tkn, const char *lit, const int line = 1,
        const std::size_t size = 1)
      : literal(lit, size), token_type(tkn), line(line) {}
  Token(const TokenType tkn, const char *begin, const char *end,
        const int line = 1)
      : literal(begin, end), token_type(tkn), line(line) {}
  Token(const TokenType tkn, const std::string &str, const int line = 1)
      : literal(str), token_type(tkn), line(line) {}
  auto operator==(const Token &right) const noexcept -> bool
  {
    /* NOLINT */ return !(token_type != right.token_type ||
                          literal != right.literal);
  }

  auto operator!=(const Token &right) const noexcept -> bool
  {
    return token_type != right.token_type || literal != right.literal;
  }

  friend auto operator<<(std::ostream &out, const Token &tkn) -> std::ostream &
  {
    out << fmt::format("Type: {} \tLiteral: {}\n",
                       getNameForValue(tokens_enums_strings, tkn.token_type),
                       tkn.literal);
    return out;
  }
};

#endif // TOKEN_H
