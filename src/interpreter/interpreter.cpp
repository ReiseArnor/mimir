#include "interpreter.h"
#include "ast.h"
#include "evaluator.h"
#include "lexer.h"
#include "object.h"
#include "parser.h"
#include "token.h"
#include <fmt/core.h>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace std;
using ast::Programs_Guard;
using obj::Environment;

auto main_print_parser_errors(const vector<string> &errors) -> string
{
  std::string result;
  for (const auto &err : errors) {
    result.append(err + "\n");
  }
  return result;
}

auto interprete_code(const string &code) -> string
{
  auto env = make_unique<Environment>();
  Programs_Guard guard;

  Lexer lexer(code);
  Parser parser(lexer);
  auto *program = guard.new_program(parser.parse_program());
  if (!parser.errors().empty()) {
    return main_print_parser_errors(parser.errors());
  }

  auto *evaluated = evaluate(program, env.get());

  if (evaluated != nullptr) {
    return fmt::format("{}", evaluated->inspect());
  }

  return "";
}
