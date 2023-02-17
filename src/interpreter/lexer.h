#ifndef LEXER_H
#define LEXER_H
#include "token.h"
#include <cstddef>
#include <string>

class Lexer {
private:
  const std::string source;
  char current_char;
  std::size_t read_position;
  std::size_t position;
  int line;

  void read_character();
  [[nodiscard]] auto keyword(const std::string &) const -> Token;
  auto read_string(char) -> Token;
  auto read_identifier() -> Token;
  auto read_number() -> Token;
  [[nodiscard]] auto peek_character() const -> char;

public:
  explicit Lexer(const std::string &);
  auto next_token() -> Token;
};

#endif // LEXER_H
