#ifndef OBJECT_H
#define OBJECT_H
#include "ast.h"
#include "parser.h"
#include "token.h"
#include "utils.h"
#include <cstddef>
#include <functional>
#include <map>
#include <string>
#include <string_view>
#include <vector>

namespace obj {
enum class ObjectType {
  BOOLEAN,
  INTEGER,
  /*NOLINT*/ _NULL,
  RETURN,
  ERROR,
  FUNCTION,
  STRING,
  BUILTIN
};

static constexpr std::array<const NameValuePair<ObjectType>, 8>
    objects_enums_string{{{ObjectType::BOOLEAN, "BOOLEAN"},
                          {ObjectType::INTEGER, "INTEGER"},
                          {ObjectType::_NULL, "NULL"},
                          {ObjectType::RETURN, "RETURN"},
                          {ObjectType::ERROR, "ERROR"},
                          {ObjectType::FUNCTION, "FUNCTION"},
                          {ObjectType::STRING, "STRING"},
                          {ObjectType::BUILTIN, "BUILTIN"}}};

class Object {
public:
  [[nodiscard]] virtual auto type() const -> ObjectType = 0;
  [[nodiscard]] virtual auto inspect() const -> std::string = 0;
  [[nodiscard]] virtual auto type_string() const -> std::string_view = 0;
  virtual ~Object() = default;
  Object() = default;
  Object(const Object &) = delete;
  auto operator=(const Object &) -> Object & = delete;
  Object(Object &&) = delete;
  auto operator=(Object &&) -> Object & = delete;
};

class Integer : public Object {
public:
  const std::size_t value;
  explicit Integer(const std::size_t val) : value(val) {}
  [[nodiscard]] auto type() const -> ObjectType final;
  [[nodiscard]] auto inspect() const -> std::string final;
  [[nodiscard]] auto type_string() const -> std::string_view final;
};

class Boolean : public Object {
public:
  const bool value;
  explicit Boolean(bool val) : value(val) {}
  [[nodiscard]] auto type() const -> ObjectType final;
  [[nodiscard]] auto inspect() const -> std::string final;
  [[nodiscard]] auto type_string() const -> std::string_view final;
};

class Null : public Object {
public:
  [[nodiscard]] auto type() const -> ObjectType final;
  [[nodiscard]] auto inspect() const -> std::string final;
  [[nodiscard]] auto type_string() const -> std::string_view final;
};

class Return : public Object {
public:
  Object *value;
  explicit Return(Object *val) : value(val) {}
  [[nodiscard]] auto type() const -> ObjectType final;
  [[nodiscard]] auto inspect() const -> std::string final;
  [[nodiscard]] auto type_string() const -> std::string_view final;
};

class Error : public Object {
public:
  const std::string message;
  explicit Error(const std::string &msg) : message(msg) {}
  [[nodiscard]] auto type() const -> ObjectType final;
  [[nodiscard]] auto inspect() const -> std::string final;
  [[nodiscard]] auto type_string() const -> std::string_view final;
};

class Environment {
  std::map<std::string, Object *> store;
  Environment *outer = nullptr;

public:
  Environment() = default;
  explicit Environment(Environment *outer) : outer(outer) {}
  void set_item(const std::string &key, Object *value);
  void del_item(const std::string &key);
  auto get_item(const std::string &key) -> Object *;
  auto item_exist(const std::string &key) -> bool;
};

class Function : public Object {
public:
  std::vector<ast::Identifier *> parameters;
  ast::Block *body;
  Environment *env;
  Function(const std::vector<ast::Identifier *> &params, ast::Block *blk,
           Environment *env)
      : parameters(params), body(blk), env(env) {}
  [[nodiscard]] auto type() const -> ObjectType final;
  [[nodiscard]] auto type_string() const -> std::string_view final;
  [[nodiscard]] auto inspect() const -> std::string final;
};

class String : public Object {
public:
  const std::string value;
  explicit String(const std::string &val) : value(val) {}
  [[nodiscard]] auto type() const -> ObjectType final;
  [[nodiscard]] auto inspect() const -> std::string final;
  [[nodiscard]] auto type_string() const -> std::string_view final;
};

using BuiltinFunction =
    std::function<Object *(const std::vector<Object *> &, const int)>;

class Builtin : public Object {
public:
  const BuiltinFunction &fn;
  explicit Builtin(const BuiltinFunction &builtin_fn) : fn(builtin_fn) {}
  [[nodiscard]] auto type() const -> ObjectType final;
  [[nodiscard]] auto inspect() const -> std::string final;
  [[nodiscard]] auto type_string() const -> std::string_view final;
  Builtin(const Builtin &cpy) : fn(cpy.fn) {}
};

} // namespace obj
#endif // OBJECT_H
