#include "evaluator.h"
#include "ast.h"
#include "object.h"

using namespace ast;

auto to_boolean_object(const std::string &operatr, obj::Object *left,
                       obj::Object *right) -> obj::Object *
{
  switch (left->type()) {
  case obj::ObjectType::BOOLEAN: {
    if (right->type() == obj::ObjectType::BOOLEAN && operatr == "==") {
      auto value = static_cast<obj::Boolean *>(left)->value ==
                   static_cast<obj::Boolean *>(right)->value;
      return value ? TRUE.get() : FALSE.get();
    }
    if (right->type() == obj::ObjectType::BOOLEAN && operatr == "!=") {
      auto value = static_cast<obj::Boolean *>(left)->value !=
                   static_cast<obj::Boolean *>(right)->value;
      return value ? TRUE.get() : FALSE.get();
    }
    if (operatr == "!=") {
      return TRUE.get();
    }

    return FALSE.get();
  }

  case obj::ObjectType::STRING: {
    if (operatr == "!=") {
      return TRUE.get();
    }

    return FALSE.get();
  }

  case obj::ObjectType::_NULL: {
    if (right->type() == obj::ObjectType::_NULL && operatr == "==") {
      return TRUE.get();
    }
    if (operatr == "!=" && right->type() != obj::ObjectType::_NULL) {
      return TRUE.get();
    }

    return FALSE.get();
  }

  case obj::ObjectType::INTEGER: {
    if (operatr == "!=") {
      return TRUE.get();
    }

    return FALSE.get();
  }

  default:
    return FALSE.get();
  }
}

auto to_boolean_object(bool value) -> obj::Object *
{
  return value ? TRUE.get() : FALSE.get();
}

inline auto is_truthy(const obj::Object *const obj) -> bool
{
  if (obj == _NULL.get()) {
    return false;
  }
  if (obj == TRUE.get()) {
    return true;
  }
  if (obj == FALSE.get()) {
    return false;
  }
  return true;
}

auto evaluate_expression(const std::vector<Expression *> &expressions,
                         obj::Environment *env) -> std::vector<obj::Object *>
{
  auto result = std::vector<obj::Object *>();

  for (auto *exp : expressions) {
    auto *evaluated = evaluate(exp, env);

    if (evaluated != nullptr) {
      result.push_back(evaluated);
    }
  }

  return result;
}

auto evaluate_string_infix_expression(const std::string &operatr,
                                      obj::Object *left, obj::Object *right,
                                      const int line) -> obj::Object *
{
  auto left_value = static_cast<obj::String *>(left)->value;
  auto right_value = static_cast<obj::String *>(right)->value;

  if (operatr == "+") {
    auto *str = new obj::String(left_value + right_value);
    cleaner.push_back(str);
    return str;
  }
  if (operatr == "==") {
    return to_boolean_object(left_value == right_value);
  }
  if (operatr == "!=") {
    return to_boolean_object(left_value != right_value);
  }

  auto *error =
      new obj::Error{fmt::format(UNKNOWN_INFIX_OPERATION, left->type_string(),
                                 operatr, right->type_string(), line)};
  eval_errors.push_back(error);

  return error;
}

auto evaluate_identifier(Identifier *ident, obj::Environment *env)
    -> obj::Object *
{
  if (env->item_exist(ident->value)) {
    return env->get_item(ident->value);
  }
  if (BUILTINS.find(ident->value) != BUILTINS.end()) {
    return &BUILTINS.at(ident->value);
  }
  return _NULL.get();
}

inline auto evaluate_integer_infix_expression(const std::string &operatr,
                                              obj::Object *left,
                                              obj::Object *right,
                                              const int line) -> obj::Object *
{
  auto left_value = static_cast<obj::Integer *>(left)->value;
  auto right_value = static_cast<obj::Integer *>(right)->value;

  if (operatr == "+") {
    auto *integer = new obj::Integer(left_value + right_value);
    cleaner.push_back(integer);
    return integer;
  }
  if (operatr == "-") {
    auto *integer = new obj::Integer(left_value - right_value);
    cleaner.push_back(integer);
    return integer;
  }
  if (operatr == "*") {
    auto *integer = new obj::Integer(left_value * right_value);
    cleaner.push_back(integer);
    return integer;
  }
  if (operatr == "/") {
    auto *integer = new obj::Integer(left_value / right_value);
    cleaner.push_back(integer);
    return integer;
  }
  if (operatr == "<") {
    return to_boolean_object(left_value < right_value);
  }
  if (operatr == ">") {
    return to_boolean_object(left_value > right_value);
  }
  if (operatr == "==") {
    return to_boolean_object(left_value == right_value);
  }
  if (operatr == "!=") {
    return to_boolean_object(left_value != right_value);
  }

  auto *error =
      new obj::Error{fmt::format(UNKNOWN_INFIX_OPERATION, left->type_string(),
                                 operatr, right->type_string(), line)};
  eval_errors.push_back(error);

  return error;
}

auto evaluate_infix_expression(const std::string &operatr, obj::Object *left,
                               obj::Object *right, const int line)
    -> obj::Object *
{
  if (left->type() == obj::ObjectType::INTEGER &&
      right->type() == obj::ObjectType::INTEGER) {
    return evaluate_integer_infix_expression(operatr, left, right, line);
  }
  if (left->type() == obj::ObjectType::STRING &&
      right->type() == obj::ObjectType::STRING) {
    return evaluate_string_infix_expression(operatr, left, right, line);
  }
  if (operatr == "==" || operatr == "!=") {
    return to_boolean_object(operatr, left, right);
  }
  if (left->type() != right->type()) {
    auto *error =
        new obj::Error{fmt::format(TYPE_MISMATCH, left->type_string(), operatr,
                                   right->type_string(), line)};
    eval_errors.push_back(error);
    return error;
  }

  auto *error =
      new obj::Error{fmt::format(UNKNOWN_INFIX_OPERATION, left->type_string(),
                                 operatr, right->type_string(), line)};
  eval_errors.push_back(error);

  return error;
}

inline auto evaluate_minus_operator_expression(obj::Object *right,
                                               const int line) -> obj::Object *
{
  if (right->type() != obj::ObjectType::INTEGER) {
    auto *error = new obj::Error{
        fmt::format(UNKNOWN_PREFIX_OPERATION, "-", right->type_string(), line)};
    eval_errors.push_back(error);
    return error;
  }

  auto *cast_right = static_cast<obj::Integer *>(right);
  auto *integer = new obj::Integer(-cast_right->value);
  cleaner.push_back(integer);

  return integer;
}

inline auto evaluate_bang_operator_expression(const obj::Object *const right)
    -> obj::Object *
{
  if (right == TRUE.get()) {
    return FALSE.get();
  }
  if (right == FALSE.get()) {
    return TRUE.get();
  }
  if (right == _NULL.get()) {
    return TRUE.get();
  }
  return FALSE.get();
}

auto evaluate_prefix_expression(const std::string &operatr, obj::Object *right,
                                const int line) -> obj::Object *
{
  if (operatr == "!") {
    return evaluate_bang_operator_expression(right);
  }
  if (operatr == "-") {
    return evaluate_minus_operator_expression(right, line);
  }

  auto *error = new obj::Error{fmt::format(UNKNOWN_PREFIX_OPERATION, operatr,
                                           right->type_string(), line)};
  eval_errors.push_back(error);

  return error;
}

auto evaluate_block_statements(Block *block, obj::Environment *env)
    -> obj::Object *
{
  obj::Object *result = nullptr;

  for (auto *statement : block->statements) {
    result = evaluate(statement, env);

    if (((result != nullptr) && result->type() == obj::ObjectType::RETURN) ||
        result->type() == obj::ObjectType::ERROR) {
      return result;
    }
  }

  return result;
}

auto evaluate_if_expression(If *if_expression, obj::Environment *env)
    -> obj::Object *
{
  assert(if_expression->condition);
  auto *condicion = evaluate(if_expression->condition, env);
  assert(condicion);

  if (is_truthy(condicion)) {
    assert(if_expression->consequence);
    return evaluate(if_expression->consequence, env);
  }
  if (if_expression->alternative != nullptr) {
    return evaluate(if_expression->alternative, env);
  }
  return _NULL.get();
}

auto evaluate_loop_statement(LoopStatement *loop, obj::Environment *env)
    -> obj::Object *
{
  assert(loop->condition);
  auto *condicion = evaluate(loop->condition, env);
  assert(condicion);

  if (is_truthy(condicion)) {
    evaluate(loop->repeat, env);
    evaluate(loop, env);
  }

  return _NULL.get();
}

auto evaluate_program(ast::Program *program, obj::Environment *env)
    -> obj::Object *
{
  obj::Object *result = nullptr;
  for (auto *stm : program->statements) {
    result = evaluate(stm, env);
    if (result->type() == obj::ObjectType::RETURN) {
      auto *cast_result = static_cast<obj::Return *>(result);
      return cast_result->value;
    }
    if (result->type() == obj::ObjectType::ERROR) {
      return result;
    }
  }

  return result;
}

inline auto extend_function_environment(obj::Function *fun,
                                        const std::vector<obj::Object *> &args,
                                        const int line) -> obj::Environment *
{
  if (fun->parameters.size() != args.size()) {
    auto *error = new obj::Error{
        fmt::format(WRONG_ARGS, line, fun->parameters.size(), args.size())};
    eval_errors.push_back(error);
    return nullptr;
  }

  auto *env = new obj::Environment(fun->env);
  environments.push_back(env);

  for (std::size_t i = 0; i < fun->parameters.size(); i++) {
    env->set_item(fun->parameters.at(i)->value, args.at(i));
  }

  return env;
}

inline auto unwrap_return_value(obj::Object *obj) -> obj::Object *
{
  if (obj->type() == obj::ObjectType::RETURN) {
    return static_cast<obj::Return *>(obj)->value;
  }
  return obj;
}

auto apply_function(obj::Object *fun, const std::vector<obj::Object *> &args,
                    const int line) -> obj::Object *
{
  if (fun->type() == obj::ObjectType::FUNCTION) {
    auto *function = static_cast<obj::Function *>(fun);
    auto *extended_environment =
        extend_function_environment(function, args, line);
    if (extended_environment == nullptr) {
      return eval_errors.at(eval_errors.size() - 1UL);
    }

    auto *evaluated = evaluate(function->body, extended_environment);
    return unwrap_return_value(evaluated);
  }
  if (fun->type() == obj::ObjectType::BUILTIN) {
    auto *function = static_cast<obj::Builtin *>(fun);
    return function->fn(args, line);
  }

  auto *error =
      new obj::Error{fmt::format(NOT_A_FUNCTION, fun->type_string(), line)};
  eval_errors.push_back(error);

  return error;
}

auto evaluate(ast::ASTNode *node, obj::Environment *env) -> obj::Object *
{
  auto node_type = node->type();

  switch (node_type) {

  case Node::Program:
    return evaluate_program(static_cast<Program *>(node), env);

  case Node::ExpressionStatement: {
    auto *cast_exp_st = static_cast<ExpressionStatement *>(node);
    return evaluate(cast_exp_st->expression, env);
  }

  case Node::Integer: {
    auto *cast_int = static_cast<Integer *>(node);
    auto *integer = new obj::Integer(cast_int->value);
    cleaner.push_back(integer);
    return integer;
  }

  case Node::Boolean: {
    auto *cast_bool = static_cast<Boolean *>(node);
    return to_boolean_object(cast_bool->value);
  }

  case Node::Prefix: {
    auto *cast_prefix = static_cast<Prefix *>(node);
    assert(cast_prefix != nullptr);
    auto *right = evaluate(cast_prefix->right, env);
    assert(right != nullptr);
    return evaluate_prefix_expression(cast_prefix->operatr, right,
                                      cast_prefix->token.line);
  }

  case Node::Infix: {
    auto *cast_infix = static_cast<Infix *>(node);
    assert(cast_infix->left && cast_infix->right);
    auto *left = evaluate(cast_infix->left, env);
    auto *right = evaluate(cast_infix->right, env);
    assert(left && right);
    return evaluate_infix_expression(cast_infix->operatr, left, right,
                                     cast_infix->token.line);
  }

  case Node::Block: {
    auto *cast_block = static_cast<Block *>(node);
    return evaluate_block_statements(cast_block, env);
  }

  case Node::If: {
    auto *cast_if = static_cast<If *>(node);
    return evaluate_if_expression(cast_if, env);
  }

  case Node::Loop: {
    auto *cast_loop = static_cast<LoopStatement *>(node);
    return evaluate_loop_statement(cast_loop, env);
  }

  case Node::ReturnStatement: {
    auto *cast_rtn_st = static_cast<ReturnStatement *>(node);
    assert(cast_rtn_st->return_value);
    auto *value = evaluate(cast_rtn_st->return_value, env);
    assert(value);
    auto *return_val = new obj::Return(value);
    cleaner.push_back(return_val);
    return return_val;
  }

  case Node::LetStatement: {
    auto *cast_let_st = static_cast<LetStatement *>(node);
    assert(cast_let_st->value);
    auto *value = evaluate(cast_let_st->value, env);
    assert(cast_let_st->name);
    env->set_item(cast_let_st->name->value, value);
    return value;
  }

  case Node::AssignStatement: {
    auto *cast_assign = static_cast<AssignStatement *>(node);
    auto *value = evaluate(cast_assign->value, env);
    env->set_item(cast_assign->name->value, value);
    return value;
  }

  case Node::Identifier: {
    auto *cast_ident = static_cast<Identifier *>(node);
    assert(cast_ident);
    return evaluate_identifier(cast_ident, env);
  }

  case Node::Function: {
    auto *cast_func = static_cast<Function *>(node);
    assert(cast_func);
    auto *func = new obj::Function(cast_func->parameters, cast_func->body, env);
    cleaner.push_back(func);
    return func;
  }

  case Node::Call: {
    auto *cast_call = static_cast<Call *>(node);
    auto *function = evaluate(cast_call->function, env);
    auto args = evaluate_expression(cast_call->arguments, env);
    return apply_function(function, args, cast_call->token.line);
  }

  case Node::StringLiteral: {
    auto *cast_str_lit = static_cast<StringLiteral *>(node);
    auto *str = new obj::String(cast_str_lit->value);
    cleaner.push_back(str);
    return str;
  }

  case Node::Null:
    return _NULL.get();

  default:
    return nullptr;
  }
}
