#include "repl.h"
#include "ast.h"
#include "evaluator.h"
#include "fmt/core.h"
#include "object.h"
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using ast::Programs_Guard;
using obj::Environment;

void print_parser_errors(const std::vector<std::string> &errors)
{
  for (const auto &error : errors) {
    std::cout << error;
  }
}
void start_repl()
{
  auto env = std::make_unique<Environment>();
  Programs_Guard guard;
  for (std::string instruction; instruction != "salir()";
       getline(std::cin, instruction)) {
    Lexer lexer(instruction);
    Parser parser(lexer);
    auto *program = guard.new_program(parser.parse_program());
    if (!parser.errors().empty()) {
      print_parser_errors(parser.errors());
      fmt::print("\n>> ");
      continue;
    }

    auto *evaluated = evaluate(program, env.get());

    if (evaluated != nullptr) {
      fmt::print("{}", evaluated->inspect());
    }
    fmt::print("\n>> ");
  }
}
