#include "parser.h"
#include "ast.h"
#include "lexer.h"
#include "token.h"
#include "utils.h"
#include <fmt/format.h>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;
using namespace ast;

Parser::Parser(const Lexer &lxr) : lexer(lxr)
{
  prefix_parse_fns = register_prefix_fns();
  infix_parse_fns = register_infix_fns();
  advance_tokens();
  advance_tokens();
}

auto Parser::parse_program() -> vector<Statement *>
{
  vector<Statement *> statements;
  while (current_token.token_type != TokenType::_EOF) {
    Statement *statement = parse_statement();
    if (statement != nullptr) {
      statements.push_back(statement);
    }
    advance_tokens();
  }
  return statements;
}

auto Parser::parse_statement() -> Statement *
{
  // the program receiving the pointer owns it
  if (current_token.token_type == TokenType::LET) {
    return parse_let_statement();
  }
  if (current_token.token_type == TokenType::IDENT &&
      peek_token.token_type == TokenType::ASSIGN) {
    return parse_assign_statement();
  }
  if (current_token.token_type == TokenType::RETURN) {
    return parse_return_statement();
  }
  if (current_token.token_type == TokenType::LOOP) {
    return parse_while_statement();
  }

  return parse_expression_statements();
}

auto Parser::parse_let_statement() -> LetStatement *
{
  auto let_statement = make_unique<LetStatement>(current_token);

  if (!expected_token(TokenType::IDENT)) {
    return nullptr;
  }

  let_statement->name = static_cast<Identifier *>(parse_identifier());

  if (!expected_token(TokenType::ASSIGN)) {
    return nullptr;
  }

  advance_tokens();
  let_statement->value = parse_expression(Precedence::LOWEST);

  if (peek_token.token_type == TokenType::SEMICOLON) {
    advance_tokens();
  }

  return let_statement.release();
}

auto Parser::parse_while_statement() -> LoopStatement *
{
  auto loop_statement = std::make_unique<ast::LoopStatement>(current_token);

  if (!expected_token(TokenType::LPAREN)) {
    return nullptr;
  }
  advance_tokens();

  loop_statement->condition = parse_expression(Precedence::LOWEST);

  if (!expected_token(TokenType::RPAREN)) {
    return nullptr;
  }

  if (!expected_token(TokenType::LBRACE)) {
    return nullptr;
  }
  loop_statement->repeat = parse_block();

  return loop_statement.release();
}

auto Parser::parse_return_statement() -> ReturnStatement *
{
  auto return_statement = make_unique<ReturnStatement>(current_token);
  advance_tokens();

  return_statement->return_value = parse_expression(Precedence::LOWEST);

  if (peek_token.token_type == TokenType::SEMICOLON) {
    advance_tokens();
  }

  return return_statement.release();
}

auto Parser::parse_expression_statements() -> ExpressionStatement *
{
  auto expression_statement = make_unique<ExpressionStatement>(current_token);

  expression_statement->expression = parse_expression(Precedence::LOWEST);
  if (peek_token.token_type == TokenType::SEMICOLON) {
    advance_tokens();
  }

  return expression_statement.release();
}

auto Parser::parse_expression(Precedence precedence) -> Expression *
{
  try {
    auto prefix_parse_fn = prefix_parse_fns[current_token.token_type];
    auto *left_expression = prefix_parse_fn();

    while (peek_token.token_type != TokenType::SEMICOLON &&
           precedence < get_precedence(peek_token.token_type)) {
      auto infix_parse_fn = infix_parse_fns[peek_token.token_type];
      advance_tokens();
      if (left_expression != nullptr) {
        left_expression = infix_parse_fn(left_expression);
      }
    }
    return left_expression;
  }
  catch (...) {
    auto error = fmt::format(
        "No se encontró ninguna función para parsear {} cerca de la línea {}\n",
        current_token.literal, current_token.line);
    errors_list.push_back(error);
    return nullptr;
  }
}

auto Parser::parse_block() -> Block *
{
  auto block_statement =
      make_unique<Block>(current_token, vector<Statement *>());
  advance_tokens();

  while (current_token.token_type != TokenType::RBRACE &&
         current_token.token_type != TokenType::_EOF) {
    auto *statement = parse_statement();
    if (statement != nullptr) {
      block_statement->statements.push_back(statement);
    }
    advance_tokens();
  }
  return block_statement.release();
}

auto Parser::parse_function_parameters() -> vector<Identifier *>
{
  vector<Identifier *> params;
  if (peek_token.token_type == TokenType::RPAREN) {
    advance_tokens();
    return params;
  }
  advance_tokens();
  auto *identifier = new Identifier(current_token, current_token.literal);
  params.push_back(identifier);

  while (peek_token.token_type == TokenType::COMMA) {
    advance_tokens();
    advance_tokens();
    auto *identifiers = new Identifier(current_token, current_token.literal);
    params.push_back(identifiers);
  }

  if (!expected_token(TokenType::RPAREN)) {
    return {};
  }

  return params;
}

auto Parser::parse_call_arguments() -> vector<Expression *>
{
  auto arguments = vector<Expression *>();
  if (peek_token.token_type == TokenType::RPAREN) {
    advance_tokens();
    return arguments;
  }

  advance_tokens();

  Expression *expression = parse_expression(Precedence::LOWEST);
  if (expression != nullptr) {
    arguments.push_back(expression);
  }

  while (peek_token.token_type == TokenType::COMMA) {
    advance_tokens();
    advance_tokens();
    expression = parse_expression(Precedence::LOWEST);
    if (expression != nullptr) {
      arguments.push_back(expression);
    }
  }

  if (!expected_token(TokenType::RPAREN)) {
    return {};
  }

  return arguments;
}

auto Parser::expected_token(const TokenType &tkn_tp) -> bool
{
  if (peek_token.token_type == tkn_tp) {
    advance_tokens();
    return true;
  }
  expected_token_error(tkn_tp);
  return false;
}

void Parser::advance_tokens()
{
  current_token = peek_token;
  peek_token = lexer.next_token();
}

auto Parser::errors() -> vector<string> & { return errors_list; }

void Parser::expected_token_error(const TokenType &tkn_tp)
{
  auto error =
      fmt::format("Se esperaba que el siguente token fuera {} pero se obtuvo "
                  "{} cerca de la línea {}",
                  getNameForValue(tokens_enums_strings, tkn_tp),
                  getNameForValue(tokens_enums_strings, peek_token.token_type),
                  current_token.line);
  errors_list.push_back(error);
}

auto Parser::register_prefix_fns() -> PrefixParseFns
{
  return {{TokenType::FUNCTION, parse_function},
          {TokenType::_FALSE, parse_boolean},
          {TokenType::_TRUE, parse_boolean},
          {TokenType::_NULL, parse_null},
          {TokenType::IDENT, parse_identifier},
          {TokenType::IF, parse_if},
          {TokenType::INT, parse_integer},
          {TokenType::MINUS, parse_prefix_expression},
          {TokenType::NEGATION, parse_prefix_expression},
          {TokenType::LPAREN, parse_grouped_expression},
          {TokenType::STRING, parse_string_literal}};
}

auto Parser::register_infix_fns() -> InfixParseFns
{
  return {{TokenType::PLUS, parse_infix_expression},
          {TokenType::MINUS, parse_infix_expression},
          {TokenType::DIVISION, parse_infix_expression},
          {TokenType::MULTIPLICATION, parse_infix_expression},
          {TokenType::EQ, parse_infix_expression},
          {TokenType::NOT_EQ, parse_infix_expression},
          {TokenType::LT, parse_infix_expression},
          {TokenType::GT, parse_infix_expression},
          {TokenType::LPAREN, parse_call}};
}

auto Parser::parse_assign_statement() -> AssignStatement *
{
  auto *name = static_cast<Identifier *>(parse_identifier());
  if (!expected_token(TokenType::ASSIGN)) {
    return nullptr;
  }

  auto token = current_token;
  advance_tokens();

  auto *value = parse_expression(Precedence::LOWEST);
  if (peek_token.token_type == TokenType::SEMICOLON) {
    advance_tokens();
  }

  return new AssignStatement(token, name, value);
}

auto Parser::get_precedence(const TokenType &tkn_tp) -> Precedence
{
  static constexpr auto PRECEDENCES =
      Map<TokenType, Precedence, precedence_values.size()>{{precedence_values}};

  if (PRECEDENCES.find(tkn_tp)) {
    return PRECEDENCES.at(tkn_tp);
  }

  return Precedence::LOWEST;
}
