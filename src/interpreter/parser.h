#ifndef PARSER_H
#define PARSER_H
#include "ast.h"
#include "fmt/format.h"
#include "lexer.h"
#include "token.h"
#include <cstddef>
#include <cstdlib>
#include <functional>
#include <map>
#include <memory>
#include <new>
#include <stdexcept>
#include <string>
#include <vector>

using PrefixParseFn = std::function<ast::Expression *()>;
using InfixParseFn = std::function<ast::Expression *(ast::Expression *)>;

using PrefixParseFns = std::map<TokenType, PrefixParseFn>;
using InfixParseFns = std::map<TokenType, InfixParseFn>;

enum class Precedence {
  LOWEST,
  EQUALS,
  LESSGREATER,
  SUM,
  PRODUC,
  PREFIX,
  CALL
};

static constexpr std::array<std::pair<TokenType, Precedence>, 9>
    precedence_values{{{TokenType::EQ, Precedence::EQUALS},
                       {TokenType::NOT_EQ, Precedence::EQUALS},
                       {TokenType::LT, Precedence::LESSGREATER},
                       {TokenType::GT, Precedence::LESSGREATER},
                       {TokenType::PLUS, Precedence::SUM},
                       {TokenType::MINUS, Precedence::SUM},
                       {TokenType::DIVISION, Precedence::PRODUC},
                       {TokenType::MULTIPLICATION, Precedence::PRODUC},
                       {TokenType::LPAREN, Precedence::CALL}}};

class Parser {
private:
  Lexer lexer;
  Token current_token;
  Token peek_token;
  PrefixParseFns prefix_parse_fns;
  InfixParseFns infix_parse_fns;
  std::vector<std::string> errors_list;

  auto parse_statement() -> ast::Statement *;
  auto parse_let_statement() -> ast::LetStatement *;
  auto parse_return_statement() -> ast::ReturnStatement *;
  auto parse_assign_statement() -> ast::AssignStatement *;
  auto parse_while_statement() -> ast::LoopStatement *;
  auto parse_expression_statements() -> ast::ExpressionStatement *;
  auto parse_expression(Precedence) -> ast::Expression *;
  auto parse_block() -> ast::Block *;
  auto parse_function_parameters() -> std::vector<ast::Identifier *>;
  auto parse_call_arguments() -> std::vector<ast::Expression *>;
  auto expected_token(const TokenType &) -> bool;
  void advance_tokens();
  void expected_token_error(const TokenType &);
  auto register_infix_fns() -> InfixParseFns;
  auto register_prefix_fns() -> PrefixParseFns;
  static auto get_precedence(const TokenType &) -> Precedence;

public:
  explicit Parser(const Lexer &lxr);
  auto parse_program() -> std::vector<ast::Statement *>;
  auto errors() -> std::vector<std::string> &;

private:
  PrefixParseFn parse_identifier = [&]() -> ast::Expression * {
    return new ast::Identifier(current_token, current_token.literal);
  };

  PrefixParseFn parse_integer = [&]() -> ast::Expression * {
    return new ast::Integer(current_token, static_cast<std::size_t>(std::stoi(
                                               current_token.literal)));
  };

  PrefixParseFn parse_prefix_expression = [&]() -> ast::Expression * {
    auto prefix_expression =
        std::make_unique<ast::Prefix>(current_token, current_token.literal);

    advance_tokens();
    prefix_expression->right = parse_expression(Precedence::PREFIX);

    return prefix_expression.release();
  };

  PrefixParseFn parse_boolean = [&]() -> ast::Expression * {
    return new ast::Boolean(current_token,
                            current_token.token_type == TokenType::_TRUE);
  };

  PrefixParseFn parse_null = [&]() -> ast::Expression * {
    return new ast::Null(current_token);
  };

  PrefixParseFn parse_grouped_expression = [&]() -> ast::Expression * {
    advance_tokens();
    auto expression =
        std::unique_ptr<ast::Expression>(parse_expression(Precedence::LOWEST));

    if (!expected_token(TokenType::RPAREN)) {
      return nullptr;
    }

    return expression.release();
  };

  PrefixParseFn parse_if = [&]() -> ast::Expression * {
    auto if_expression = std::make_unique<ast::If>(current_token);

    if (!expected_token(TokenType::LPAREN)) {
      return nullptr;
    }
    advance_tokens();

    if_expression->condition = parse_expression(Precedence::LOWEST);

    if (!expected_token(TokenType::RPAREN)) {
      return nullptr;
    }

    if (!expected_token(TokenType::LBRACE)) {
      return nullptr;
    }
    if_expression->consequence = parse_block();

    if (peek_token.token_type == TokenType::ELSE) {
      advance_tokens();
      if (!expected_token(TokenType::LBRACE)) {
        return nullptr;
      }
      if_expression->alternative = parse_block();
    }

    return if_expression.release();
  };

  PrefixParseFn parse_function = [&]() -> ast::Expression * {
    auto function = std::make_unique<ast::Function>(current_token);
    if (!expected_token(TokenType::LPAREN)) {
      return nullptr;
    }
    function->parameters = parse_function_parameters();

    if (!expected_token(TokenType::LBRACE)) {
      return nullptr;
    }

    function->body = parse_block();

    return function.release();
  };

  PrefixParseFn parse_string_literal = [&]() -> ast::Expression * {
    auto string_literal = std::make_unique<ast::StringLiteral>(
        current_token, current_token.literal);
    return string_literal.release();
  };

  InfixParseFn parse_infix_expression =
      [&](ast::Expression *left) -> ast::Expression * {
    auto infix = std::make_unique<ast::Infix>(current_token, left,
                                              current_token.literal);

    auto precedence = get_precedence(current_token.token_type);
    advance_tokens();
    infix->right = parse_expression(precedence);

    return infix.release();
  };

  InfixParseFn parse_call =
      [&](ast::Expression *function) -> ast::Expression * {
    auto call = std::make_unique<ast::Call>(current_token, function);
    call->arguments = parse_call_arguments();
    return call.release();
  };
};

#endif // PARSER_H
