#include "../src/interpreter/ast.h"
#include "../src/interpreter/evaluator.h"
#include "../src/interpreter/lexer.h"
#include "../src/interpreter/object.h"
#include "../src/interpreter/parser.h"
#include "catch2/catch_test_macros.hpp"
#include <memory>
#include <string>
#include <tuple>
#include <vector>
using namespace std;
using ast::Program;
using obj::Object;
using obj::String;

auto evaluate_tests(const string &str, obj::Environment *env = nullptr)
    -> Object *
{
  Lexer lexer(str);
  Parser parser(lexer);
  Program program(parser.parse_program());
  Object *evaluated = nullptr;
  if (env == nullptr) {
    auto temp_env = make_unique<obj::Environment>();
    evaluated = evaluate(&program, temp_env.get());
  }
  else {
    evaluated = evaluate(&program, env);
  }

  REQUIRE(evaluated != nullptr);

  return evaluated;
}

void test_object(Object *evaluated, const int expected)
{
  auto *eval = static_cast<obj::Integer *>(evaluated);
  REQUIRE(eval->value == expected);
}

void test_object(Object *evaluated, const bool expected)
{
  auto *eval = static_cast<obj::Boolean *>(evaluated);
  REQUIRE(eval->value == expected);
}

void test_object(Object *evaluated, const char *expected)
{
  auto *eval = static_cast<obj::Error *>(evaluated);
  REQUIRE(eval->message == expected);
}

void test_object(const Object *evaluated) { REQUIRE(evaluated == _NULL.get()); }

template <typename T>
void eval_and_test_objects(const vector<tuple<string, T>> &tests)
{
  for (auto &test : tests) {
    INFO(get<0>(test));
    auto evaluated = evaluate_tests(get<0>(test));
    test_object(evaluated, get<1>(test));
  }
}

TEST_CASE("Integer evaluation", "[evaluator]")
{
  vector<tuple<string, int>> tests{{"5", 5},
                                   {"10", 10},
                                   {"-5", -5},
                                   {"-10", -10},
                                   {"5 + 5", 10},
                                   {"5 - 10", -5},
                                   {"2 * 2 * 2 * 2", 16},
                                   {"2 * 5 - 3", 7},
                                   {"50 / 2", 25},
                                   {"2 * (5 - 3)", 4},
                                   {"(2 + 7) / 3", 3},
                                   {"50 / 2 * 2 + 10", 60},
                                   {"5 / 2", 2}};

  eval_and_test_objects(tests);
}

TEST_CASE("Null value", "[evaluator]")
{
  vector<string> tests{"nulo",
                       "a",
                       "b",
                       "variable a = nulo; a",
                       "variable a = nulo; variable b = a; b",
                       "a = nulo; a",
                       "a = nulo; b = a; b"};

  for (const auto &exp : tests) {
    auto *evaluated = evaluate_tests(exp);
    test_object(evaluated);
  }
}

TEST_CASE("Boolean evaluation", "[evaluator]")
{
  vector<tuple<string, bool>> tests{{"verdadero", true},
                                    {"falso", false},
                                    {"1 < 2", true},
                                    {"1 > 2", false},
                                    {"1 < 1", false},
                                    {"1 > 1", false},
                                    {"1 == 1", true},
                                    {"1 != 1", false},
                                    {"1 != 2", true},
                                    {"verdadero == verdadero", true},
                                    {"falso == falso", true},
                                    {"verdadero == falso", false},
                                    {"verdadero != falso", true},
                                    {"(1 < 2) == verdadero", true},
                                    {"(1 < 2) == falso", false},
                                    {"(1 > 2) == verdadero", false},
                                    {"(1 > 2) == falso", true},
                                    {"nulo == nulo", true},
                                    {"nulo == 1", false},
                                    {"nulo == \"hello\"", false},
                                    {"nulo == verdadero", false},
                                    {"nulo == falso", false},
                                    {"nulo != nulo", false},
                                    {"nulo != 1", true},
                                    {"nulo != \"hello\"", true},
                                    {"nulo != verdadero", true},
                                    {"nulo != falso", true}};

  eval_and_test_objects(tests);
}

TEST_CASE("Bang operator", "[evaluator]")
{
  vector<tuple<string, bool>> tests{{"!verdadero", false}, {"!falso", true},
                                    {"!!verdadero", true}, {"!!falso", false},
                                    {"!5", false},         {"!!5", true},
                                    {"!\"hi\"", false},    {"!!\"hi\"", true},
                                    {"!nulo", true},       {"!!nulo", false}};

  eval_and_test_objects(tests);
}

TEST_CASE("If else evaluation")
{
  const int dies = 10;
  const int veinte = 20;
  vector<tuple<string, const int *>> tests{
      {"si (verdadero) { 10 }", &dies},
      {"si (falso) { 10 }", nullptr},
      {"si (nulo) { 10 }", nullptr},
      {"si (1) { 10 }", &dies},
      {"si (\"1\") { 10 }", &dies},
      {"si (1 < 2) { 10 }", &dies},
      {"si (1 > 2) { 10 }", nullptr},
      {"si (1 < 2) { 10 } si_no { 20 }", &dies},
      {"si (1 > 2) { 10 } si_no { 20 }", &veinte}};

  for (auto &statement : tests) {
    auto *evaluated = evaluate_tests(get<0>(statement));

    if (get<1>(statement) == nullptr) {
      test_object(evaluated);
    }
    else {
      test_object(evaluated, *get<1>(statement));
    }
  }
}

TEST_CASE("Return evaluation")
{
  vector<tuple<string, int>> tests{{"regresa 10;", 10},
                                   {"regresa 10; 9;", 10},
                                   {"regresa 2 * 5; 9;", 10},
                                   {"9; regresa 3 * 6; 9;", 18},
                                   {"                          \
            si (10 > 1) {           \
                si (20 > 10) {      \
                    regresa 1;      \
                }                   \
                                    \
                regresa 0;          \
            }                       \
        ",
                                    1}};

  eval_and_test_objects(tests);
}

TEST_CASE("Error handling")
{
  vector<tuple<string, const char *>> tests{
      {"5 + verdadero",
       "Discrepancia de tipos: INTEGER + BOOLEAN cerca de la línea 1"},
      {"5 + verdadero; 9;",
       "Discrepancia de tipos: INTEGER + BOOLEAN cerca de la línea 1"},
      {"-verdadero", "Operador desconocido: -BOOLEAN cerca de la línea 1"},
      {"1; verdadero + falso",
       "Operador desconocido: BOOLEAN + BOOLEAN cerca de la línea 1"},
      {"5; verdadero - falso; 10;",
       "Operador desconocido: BOOLEAN - BOOLEAN cerca de la línea 1"},
      {"                                          \
            si (10 > 7) {\n                         \
                regresa verdadero + falso;\n        \
            }",
       "Operador desconocido: BOOLEAN + BOOLEAN cerca de la línea 2"},
      {"                                          \
            si (10 > 1) {\n                         \
                si (verdadero) {\n                  \
                    regresa verdadero * falso;\n    \
                }\n                                 \
                regresa 1;\n                        \
            }",
       "Operador desconocido: BOOLEAN * BOOLEAN cerca de la línea 3"},
      {"                                          \
            si (5 < 2) {\n                          \
                regresa 1;\n                        \
            } si_no {\n                             \
                regresa verdadero / falso;\n        \
            }",
       "Operador desconocido: BOOLEAN / BOOLEAN cerca de la línea 4"},
      {R"("foo" - "bar";)",
       "Operador desconocido: STRING - STRING cerca de la línea 1"},
      {"longitud(1);", "Argumento para longitud sin soporte, se recibió "
                       "INTEGER cerca de la línea 1"},
      {R"(longitud("uno", "dos");)",
       "Número incorrecto de argumentos para longitud, se recibieron 2, se "
       "esperaba 1, cerca de la línea 1"}};

  eval_and_test_objects(tests);
}

TEST_CASE("Assignment evaluation")
{
  vector<tuple<string, int>> tests{
      {"variable a = 5; a;", 5},
      {"variable a = 5 * 5; a;", 25},
      {"variable a = 10; variable b = a; b;", 10},
      {"variable a = 5; variable b = a; variable c = a + b + 5; c;", 15},
      {"variable a = si (1 > 0) { 1 } si_no { 0 }; a", 1},
      {"variable a = procedimiento(){regresa 10;}(); a", 10},
      {"a = si (1 > 0) { 1 } si_no { 0 }; a", 1},
      {"a = procedimiento(){regresa 10;}(); a", 10},
      {"a = 5; a;", 5},
      {"a = 5 * 5; a;", 25},
      {"a = 10; b = a; b;", 10},
      {"a = 5; b = a; c = a + b + 5; c;", 15},
      {"a = 10; variable b = a; b;", 10},
      {"variable a = 5; b = a; variable c = a + b + 5; c;", 15},
  };

  auto env = make_unique<obj::Environment>();
  for (auto &test : tests) {
    auto *evaluated = evaluate_tests(get<0>(test), env.get());
    test_object(evaluated, get<1>(test));
  }
}

TEST_CASE("Function evaluation")
{
  string str = "procedimiento(x) {x + 2;};";
  Lexer lexer(str);
  Parser parser(lexer);
  Program program(parser.parse_program());

  auto env = make_unique<obj::Environment>();
  auto *evaluated = static_cast<obj::Function *>(evaluate(&program, env.get()));

  REQUIRE(evaluated != nullptr);

  REQUIRE(evaluated->parameters.size() == 1);
  REQUIRE(evaluated->parameters.at(0)->to_string() == "x");
  REQUIRE(evaluated->body->to_string() == "(x + 2)");
}

TEST_CASE("Function calls")
{
  vector<tuple<string, int>> tests{
      {"variable identidad = procedimiento(x) { x }; identidad(5);", 5},
      {"                                              \
            variable identidad = procedimiento(x) {     \
                regresa x;                              \
            };                                          \
            identidad(5);                               \
        ",
       5},
      {"                                              \
            variable doble = procedimiento(x) {         \
                regresa 2 * x;                          \
            };                                          \
            doble(5)                                    \
        ",
       10},
      {"                                              \
            variable suma = procedimiento(x, y) {       \
                regresa x + y;                          \
            };                                          \
            suma(3, 8);                                 \
        ",
       11},
      {"                                              \
            variable suma = procedimiento(x, y) {       \
                regresa x + y;                          \
            };                                          \
            suma(5 + 5, suma(10, 10));                  \
        ",
       30},
      {"procedimiento(x) { x }(5)", 5}};

  auto env = make_unique<obj::Environment>();
  for (auto &test : tests) {
    auto *evaluated = evaluate_tests(get<0>(test), env.get());
    test_object(evaluated, get<1>(test));
  }
}

TEST_CASE("String evaluation")
{
  vector<tuple<string, string>> tests{
      {"\"Hello world!\"", "Hello world!"},
      {"procedimiento(){ regresa \"Programar es divertido!\"; }()",
       "Programar es divertido!"},
      {"variable hello = \"Hi!\"; hello", "Hi!"},
      {"variable condicion = si (10 > 5) { \"10 es mayor que 5\" }    \
            si_no { \"5 es mayor que 10!?\" }; condicion",
       "10 es mayor que 5"},
      {"hello = \"Hi!\"; hello", "Hi!"},
      {"condicion = si (10 > 5) { \"10 es mayor que 5\" }    \
            si_no { \"5 es mayor que 10!?\" }; condicion",
       "10 es mayor que 5"}};

  for (auto &test : tests) {
    auto *evaluated = static_cast<String *>(evaluate_tests(get<0>(test)));
    REQUIRE(evaluated->value == get<1>(test));
  }
}

TEST_CASE("String concatenation")
{
  vector<tuple<string, string>> tests{
      {R"("foo" + "bar";)", "foobar"},
      {R"("Hello," + " " + "world!")", "Hello, world!"},
      {"                                              \
            variable saludo = procedimiento(nombre) {   \
                regresa \"Hola \" + nombre + \"!\";     \
            }                                           \
            saludo(\"David\")                           \
        ",
       "Hola David!"},
      {R"(variable x = "foo" + "bar"; x)", "foobar"},
      {"variable x = \"foo\"; variable y = \"bar\"    \
            variable a = x + y; a",
       "foobar"},
      {"variable adios_str = procedimiento(){ regresa \"adios!\" };    \
            variable bye = adios_str(); bye",
       "adios!"},
      {"                                              \
            saludo = procedimiento(nombre) {   \
                regresa \"Hola \" + nombre + \"!\";     \
            }                                           \
            saludo(\"David\")                           \
        ",
       "Hola David!"},
      {R"(x = "foo" + "bar"; x)", "foobar"},
      {"x = \"foo\"; y = \"bar\"    \
            a = x + y; a",
       "foobar"},
      {"adios_str = procedimiento(){ regresa \"adios!\" };    \
            bye = adios_str(); bye",
       "adios!"}};

  for (auto &test : tests) {
    auto *evaluated = static_cast<String *>(evaluate_tests(get<0>(test)));
    REQUIRE(evaluated->value == get<1>(test));
  }
}

TEST_CASE("String comparison")
{
  vector<tuple<string, bool>> tests{{R"("a" == "a")", true},
                                    {R"("a" != "a")", false},
                                    {R"("a" == "b")", false},
                                    {R"("a" != "b")", true}};

  eval_and_test_objects(tests);
}

TEST_CASE("Builtin functions")
{
  vector<tuple<string, int>> tests{{"longitud(\"\");", 0},
                                   {"longitud(\"cuatro\");", 6},
                                   {"longitud(\"Hola mundo\");", 10}};

  eval_and_test_objects(tests);
}
