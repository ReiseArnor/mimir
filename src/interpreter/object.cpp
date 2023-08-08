#include "object.h"

auto obj::Integer::type_string() const -> std::string_view
{
  return getNameForValue(objects_enums_string, ObjectType::INTEGER);
}

auto obj::Integer::inspect() const -> std::string
{
  return std::to_string(value);
}

auto obj::Integer::type() const -> ObjectType { return ObjectType::INTEGER; }

auto obj::Boolean::type() const -> ObjectType { return ObjectType::BOOLEAN; }

auto obj::Boolean::inspect() const -> std::string
{
  return value ? "verdadero" : "falso";
}

auto obj::Boolean::type_string() const -> std::string_view
{
  return getNameForValue(objects_enums_string, ObjectType::BOOLEAN);
}

auto obj::Null::type() const -> ObjectType { return ObjectType::_NULL; }

auto obj::Null::inspect() const -> std::string { return "nulo"; }

auto obj::Null::type_string() const -> std::string_view
{
  return getNameForValue(objects_enums_string, ObjectType::_NULL);
}

auto obj::Return::type() const -> ObjectType { return ObjectType::RETURN; }

auto obj::Return::inspect() const -> std::string { return value->inspect(); }

auto obj::Return::type_string() const -> std::string_view
{
  return getNameForValue(objects_enums_string, ObjectType::RETURN);
}

auto obj::Error::type() const -> ObjectType { return ObjectType::ERROR; }

auto obj::Error::inspect() const -> std::string { return message; }

auto obj::Error::type_string() const -> std::string_view
{
  return getNameForValue(objects_enums_string, ObjectType::ERROR);
}

void obj::Environment::set_item(const std::string &key, Object *value)
{
  store[key] = value;
}

void obj::Environment::del_item(const std::string &key) { store.erase(key); }

auto obj::Environment::get_item(const std::string &key) -> Object *
{
  if (store[key] != nullptr) {
    return store[key];
  }
  return outer->get_item(key);
}

auto obj::Environment::item_exist(const std::string &key) -> bool
{
  if (store.find(key) != store.end()) {
    return true;
  }
  if ((outer != nullptr) && outer->item_exist(key)) {
    return true;
  }
  return false;
}

auto obj::Function::type() const -> ObjectType { return ObjectType::FUNCTION; }

auto obj::Function::type_string() const -> std::string_view
{
  return getNameForValue(objects_enums_string, ObjectType::FUNCTION);
}

auto obj::Function::inspect() const -> std::string { return "Función"; }

auto obj::String::type() const -> ObjectType { return ObjectType::STRING; }

auto obj::String::inspect() const -> std::string { return value; }

auto obj::String::type_string() const -> std::string_view
{
  return getNameForValue(objects_enums_string, ObjectType::STRING);
}

auto obj::Builtin::type() const -> ObjectType { return ObjectType::BUILTIN; }

auto obj::Builtin::inspect() const -> std::string { return "builtin function"; }

auto obj::Builtin::type_string() const -> std::string_view
{
  return getNameForValue(objects_enums_string, ObjectType::BUILTIN);
}
