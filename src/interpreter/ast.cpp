#include "ast.h"

auto ast::Statement::token_literal() const -> std::string
{
  return token.literal;
}

auto ast::Statement::type() const -> Node { return Node::Statement; }
auto ast::Expression::token_literal() const -> std::string
{
  return token.literal;
}

auto ast::Expression::to_string() const -> std::string { return token.literal; }

auto ast::Expression::type() const -> Node { return Node::Expression; }

auto ast::Program::token_literal() const -> std::string
{
  if (!statements.empty()) {
    return statements.at(0)->token_literal();
  }
  return "";
}

auto ast::Program::to_string() const -> std::string
{
  std::string str;
  for (auto *statement : statements) {
    str.append(statement->to_string());
  }
  return str;
}

auto ast::Program::operator==(const Program &other_program) const -> bool
{
  return to_string() == other_program.to_string();
}

auto ast::Identifier::type() const -> Node { return Node::Identifier; }

auto ast::Identifier::to_string() const -> std::string { return value; }

auto ast::LetStatement::type() const -> Node { return Node::LetStatement; }

auto ast::LetStatement::to_string() const -> std::string
{
  return token_literal() + " " + name->to_string() + " = " +
         value->to_string() + ";";
}

auto ast::ReturnStatement::type() const -> Node
{
  return Node::ReturnStatement;
}

auto ast::ReturnStatement::to_string() const -> std::string
{
  return token_literal() + " " + return_value->to_string() + ";";
}

auto ast::ExpressionStatement::type() const -> Node
{
  return Node::ExpressionStatement;
}

auto ast::ExpressionStatement::to_string() const -> std::string
{
  return expression->to_string();
}

auto ast::Integer::type() const -> Node { return Node::Integer; }

auto ast::Integer::to_string() const -> std::string
{
  return std::to_string(value);
}

auto ast::Prefix::type() const -> Node { return Node::Prefix; }

auto ast::Prefix::to_string() const -> std::string
{
  return "(" + operatr + right->to_string() + ")";
}

auto ast::Infix::type() const -> Node { return Node::Infix; }

auto ast::Infix::to_string() const -> std::string
{
  return "(" + left->to_string() + " " + operatr + " " + right->to_string() +
         ")";
}

auto ast::Boolean::type() const -> Node { return Node::Boolean; }

auto ast::Boolean::to_string() const -> std::string { return token_literal(); }

auto ast::Block::type() const -> Node { return Node::Block; }

auto ast::Block::to_string() const -> std::string
{
  std::string out;
  for (auto *statement : statements) {
    out.append(statement->to_string());
  }
  return out;
}

auto ast::If::to_string() const -> std::string
{
  std::string out =
      "si " + condition->to_string() + " " + consequence->to_string();
  if (alternative != nullptr) {
    out.append(" si_no" + alternative->to_string());
  }
  return out;
}

auto ast::LoopStatement::to_string() const -> std::string
{
  return "mientras " + condition->to_string() + " " + repeat->to_string();
}

auto ast::Function::type() const -> Node { return Node::Function; }

auto ast::Function::to_string() const -> std::string
{
  std::string params;
  for (auto *exp : parameters) {
    params.append(exp->to_string() + ", ");
  }
  params.erase(params.size() - 2, 2);
  return token_literal() + "(" + params + ")" + "{" + body->to_string() + "}";
}

auto ast::Call::type() const -> Node { return Node::Call; }

auto ast::Call::to_string() const -> std::string
{
  std::string args;
  for (auto *arg : arguments) {
    args.append(arg->to_string() + ", ");
  }
  args.erase(args.size() - 2, 2);
  return function->to_string() + "(" + args + ")";
}

auto ast::StringLiteral::type() const -> Node { return Node::StringLiteral; }

auto ast::StringLiteral::to_string() const -> std::string
{
  return Expression::to_string();
}

auto ast::Null::type() const -> Node { return Node::Null; }

auto ast::Null::to_string() const -> std::string { return token_literal(); }

auto ast::AssignStatement::type() const -> Node
{
  return Node::AssignStatement;
}

auto ast::AssignStatement::to_string() const -> std::string
{
  return name->to_string() + " = " + value->to_string();
}

void ast::Programs_Guard::push_back(Program *prog) { programs.push_back(prog); }

auto ast::Programs_Guard::new_program(
    const std::vector<Statement *> &statements) -> Program *
{
  auto *prog = new Program(statements);
  programs.push_back(prog);
  return prog;
}
