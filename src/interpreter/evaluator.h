#ifndef EVALUATOR_H
#define EVALUATOR_H
#include "ast.h"
#include "builtin.h"
#include "cleaner.h"
#include "object.h"
#include "utils.h"
#include <cassert>
#include <cstddef>
#include <fmt/format.h>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

inline constexpr std::string_view WRONG_ARGS =
    "Cantidad errónea de argumentos para la función cerca de la línea {}, se "
    "esperaban {} pero se obtuvo {}";
inline constexpr std::string_view NOT_A_FUNCTION =
    "No es una function: {} cerca de la línea {}";
inline constexpr std::string_view TYPE_MISMATCH =
    "Discrepancia de tipos: {} {} {} cerca de la línea {}";
inline constexpr std::string_view UNKNOWN_PREFIX_OPERATION =
    "Operador desconocido: {}{} cerca de la línea {}";
inline constexpr std::string_view UNKNOWN_INFIX_OPERATION =
    "Operador desconocido: {} {} {} cerca de la línea {}";

/* NOLINT */ inline const auto TRUE = std::make_unique<obj::Boolean>(true);
/* NOLINT */ inline const auto FALSE = std::make_unique<obj::Boolean>(false);
/* NOLINT */ inline const auto _NULL = std::make_unique<obj::Null>();

auto evaluate(ast::ASTNode *node, obj::Environment *env) -> obj::Object *;

#endif // EVALUATOR_H
