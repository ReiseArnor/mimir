#include "lexer.h"
#include "token.h"
#include "utils.h"
#include <array>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>
using namespace std;

auto is_indentation(char) -> bool;
auto is_number(char /*chr*/) -> bool;
auto is_identifier(char /*chr*/) -> bool;
auto skip_whitespace(char /*chr*/, int & /*line*/) -> bool;

Lexer::Lexer(const string &src)
    : source(src), current_char(' '), read_position(0), position(0), line(1)
{
}

void Lexer::read_character()
// TODO switch to char pointers instead of ints
{
  if (read_position >= source.size()) {
    current_char = '\0';
  }
  else {
    current_char = source.at(read_position);
  }

  position = read_position;
  read_position++;
}

auto Lexer::next_token() -> Token
{
  read_character();
  while (skip_whitespace(current_char, line)) {
    read_character();
  }

  switch (current_char) {
  case 'a':
  case 'b':
  case 'c':
  case 'd':
  case 'e':
  case 'f':
  case 'g':
  case 'h':
  case 'i':
  case 'j':
  case 'k':
  case 'l':
  case 'm':
  case 'n':
  case 'o':
  case 'p':
  case 'q':
  case 'r':
  case 's':
  case 't':
  case 'u':
  case 'v':
  case 'w':
  case 'x':
  case 'y':
  case 'z':
  case 'A':
  case 'B':
  case 'C':
  case 'D':
  case 'E':
  case 'F':
  case 'G':
  case 'H':
  case 'I':
  case 'J':
  case 'K':
  case 'L':
  case 'M':
  case 'N':
  case 'O':
  case 'P':
  case 'Q':
  case 'R':
  case 'S':
  case 'T':
  case 'U':
  case 'V':
  case 'W':
  case 'X':
  case 'Y':
  case 'Z':
    return read_identifier();
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
    return read_number();
  case '=':
    if (peek_character() == '=') {
      read_position++;
      return {TokenType::EQ, "==", line, 2};
    }
    return {TokenType::ASSIGN, &current_char, line};
  case '+':
    return {TokenType::PLUS, &current_char, line};
  case '-':
    return {TokenType::MINUS, &current_char, line};
  case '/':
    return {TokenType::DIVISION, &current_char, line};
  case '*':
    return {TokenType::MULTIPLICATION, &current_char, line};
  case '!':
    if (peek_character() == '=') {
      read_position++;
      return {TokenType::NOT_EQ, "!=", line, 2};
    }
    return {TokenType::NEGATION, &current_char, line};
  case '<':
    return {TokenType::LT, &current_char, line};
  case '>':
    return {TokenType::GT, &current_char, line};
  case '(':
    return {TokenType::LPAREN, &current_char, line};
  case ')':
    return {TokenType::RPAREN, &current_char, line};
  case '{':
    return {TokenType::LBRACE, &current_char, line};
  case '}':
    return {TokenType::RBRACE, &current_char, line};
  case ',':
    return {TokenType::COMMA, &current_char, line};
  case ';':
    return {TokenType::SEMICOLON, &current_char, line};
  case '\"':
    return read_string('\"');
  case '\'':
    return read_string('\'');
  case '\0':
    return {TokenType::_EOF, &current_char, line};
  default:
    return {TokenType::ILLEGAL, &current_char, line};
  }
}

auto Lexer::read_identifier() -> Token
{
  const char *begin = &source.at(position);
  const char *end;
  while (is_identifier(current_char)) {
    read_character();
  }
  if (current_char == '\0') {
    end = &source.at(position - 1);
    end++;
  }
  else {
    end = &source.at(position);
  }
  read_position = position;

  return keyword(string(begin, end));
}

auto Lexer::read_number() -> Token
{
  const char *begin = &source.at(position);
  const char *end;
  while (is_number(current_char)) {
    read_character();
  }
  if (current_char == '\0') {
    end = &source.at(position - 1);
    end++;
  }
  else {
    end = &source.at(position);
  }
  read_position = position;

  return Token{TokenType::INT, begin, end, line};
}

auto Lexer::read_string(char quote) -> Token
{
  read_character();
  if (current_char == quote) {
    return Token{TokenType::STRING, string(""), line};
  }

  const char *begin = &source.at(position);
  const char *end = begin;
  while (current_char != '\0') {
    read_character();
    if (current_char == quote) {
      end = &source.at(position);
      break;
    }
  }

  return Token{TokenType::STRING, begin, end, line};
}

static constexpr array<pair<string_view, TokenType>, 9> keyword_values{{
    {"variable", TokenType::LET},
    {"procedimiento", TokenType::FUNCTION},
    {"mientras", TokenType::LOOP},
    {"regresa", TokenType::RETURN},
    {"si", TokenType::IF},
    {"si_no", TokenType::ELSE},
    {"verdadero", TokenType::_TRUE},
    {"falso", TokenType::_FALSE},
}};

auto Lexer::keyword(const string &str) const -> Token
{
  static constexpr auto keywords =
      Map<string_view, TokenType, keyword_values.size()>{{keyword_values}};

  if (keywords.find(str)) {
    return {keywords.at(str), str, line};
  }

  return {TokenType::IDENT, str, line};
}

auto Lexer::peek_character() const -> char
{
  return read_position >= source.size() ? '\0' : source.at(read_position);
}

auto is_identifier(char chr) -> bool
{
  switch (chr) {
  case 'a':
  case 'b':
  case 'c':
  case 'd':
  case 'e':
  case 'f':
  case 'g':
  case 'h':
  case 'i':
  case 'j':
  case 'k':
  case 'l':
  case 'm':
  case 'n':
  case 'o':
  case 'p':
  case 'q':
  case 'r':
  case 's':
  case 't':
  case 'u':
  case 'v':
  case 'w':
  case 'x':
  case 'y':
  case 'z':
  case 'A':
  case 'B':
  case 'C':
  case 'D':
  case 'E':
  case 'F':
  case 'G':
  case 'H':
  case 'I':
  case 'J':
  case 'K':
  case 'L':
  case 'M':
  case 'N':
  case 'O':
  case 'P':
  case 'Q':
  case 'R':
  case 'S':
  case 'T':
  case 'U':
  case 'V':
  case 'W':
  case 'X':
  case 'Y':
  case 'Z':
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
  case '_':
    return true;
  default:
    return false;
  }
}

auto is_number(char chr) -> bool
{
  switch (chr) {
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
    return true;
  default:
    return false;
  }
}

auto skip_whitespace(char chr, int &line) -> bool
{
  switch (chr) {
  case '\n':
    line++;
    [[fallthrough]];
  case '\r':
  case ' ':
  case '\t':
    return true;
  default:
    return false;
  }
}
