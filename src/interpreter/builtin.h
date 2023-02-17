#ifndef BUILTIN_H
#define BUILTIN_H
#include "cleaner.h"
#include "object.h"
#include "utils.h"
#include <cstdlib>
#include <fmt/format.h>
#include <map>
#include <string>
#include <string_view>
#include <vector>

static constexpr std::string_view UNSUPPORTED_ARGUMENT_TYPE =
    "Argumento para longitud sin soporte, se recibió {} cerca de la línea {}";
static constexpr std::string_view WRONG_ARGS_BUILTIN_FN =
    "Número incorrecto de argumentos para {}, se recibieron {}, se esperaba 1, "
    "cerca de la línea {}";

static const obj::BuiltinFunction longitud = [](const std::vector<obj::Object *> &args,
                          const int line) -> obj::Object * {
  if (args.size() != 1) {
    auto *error = new obj::Error{
        fmt::format(WRONG_ARGS_BUILTIN_FN, "longitud", args.size(), line)};
    eval_errors.push_back(error);
    return error;
  }

  auto *argument = dynamic_cast<obj::String *>(args.at(0));

  if (argument != nullptr) {
    if (argument->value.empty()) {
      auto *integer = new obj::Integer(0);
      cleaner.push_back(integer);
      return integer;
    }
    auto *integer = new obj::Integer(argument->value.size());
    cleaner.push_back(integer);
    return integer;
  }

  auto *error = new obj::Error{
      fmt::format(UNSUPPORTED_ARGUMENT_TYPE, args.at(0)->type_string(), line)};
  eval_errors.push_back(error);
  return error;
};

inline auto salir(const std::vector<obj::Object *> & /*unused*/,
                  const int /*unused*/) -> obj::Object *
{
  exit(EXIT_SUCCESS);
};

static std::map<std::string_view, obj::Builtin> BUILTINS{
    {"longitud", obj::Builtin(longitud)},
    {"salir", obj::Builtin(salir)},
};

#endif // BUILTIN_H
