#ifndef AST_H
#define AST_H
#include "token.h"
#include <cstddef>
#include <string>
#include <vector>

namespace ast {
enum class Node {
  AssignStatement,
  Block,
  Boolean,
  Call,
  Loop,
  Expression,
  ExpressionStatement,
  Function,
  Identifier,
  If,
  Infix,
  Integer,
  LetStatement,
  Null,
  Prefix,
  Program,
  ReturnStatement,
  Statement,
  StringLiteral
};

class ASTNode {
public:
  [[nodiscard]] virtual auto token_literal() const -> std::string = 0;
  [[nodiscard]] virtual auto to_string() const -> std::string = 0;
  [[nodiscard]] virtual auto type() const -> Node = 0;
  virtual ~ASTNode() = default;
  ASTNode() = default;
  ASTNode(const ASTNode &) = delete;
  auto operator=(const ASTNode &) -> ASTNode & = delete;
  ASTNode(ASTNode &&) = delete;
  auto operator=(ASTNode &&) -> ASTNode & = delete;
};

class Statement : public ASTNode {
public:
  Token token;
  Statement() = default;
  explicit Statement(const Token &tkn) : token(tkn) {}
  [[nodiscard]] auto token_literal() const -> std::string override;
  [[nodiscard]] auto type() const -> Node override;
};

class Expression : public ASTNode {
public:
  Token token;
  Expression() = default;
  explicit Expression(const Token &tkn) : token(tkn) {}
  [[nodiscard]] auto token_literal() const -> std::string override;
  [[nodiscard]] auto to_string() const -> std::string override;
  [[nodiscard]] auto type() const -> Node override;
};

class Program final : public ASTNode {
public:
  Program() = delete;
  Program(const Program &) = delete;
  auto operator=(const Program &) -> Program & = delete;

  std::vector<Statement *> statements;

  explicit Program(const std::vector<Statement *> &statements)
      : statements(statements) {}
  [[nodiscard]] auto type() const -> Node override { return Node::Program; }
  [[nodiscard]] auto token_literal() const -> std::string override;
  [[nodiscard]] auto to_string() const -> std::string override;

  auto operator==(const Program &other_program) const -> bool;

  ~Program() final
  {
    for (auto *statement : statements) {
      delete statement;
    }
  }
};

class Identifier : public Expression {
public:
  const std::string value;
  Identifier() = default;
  Identifier(const Token &tkn, const std::string &val)
      : Expression(tkn), value(val) {}
  [[nodiscard]] auto type() const -> Node override;
  [[nodiscard]] auto to_string() const -> std::string override;
};

class LetStatement final : public Statement {
public:
  Identifier *name;
  Expression *value;
  LetStatement() = default;
  explicit LetStatement(const Token &token)
      : Statement(token), name(nullptr), value(nullptr) {}
  explicit LetStatement(const Token &token, Identifier *name, Expression *value)
      : Statement(token), name(name), value(value) {}
  [[nodiscard]] auto type() const -> Node override;
  [[nodiscard]] auto to_string() const -> std::string override;

  ~LetStatement() final
  {
    delete name;
    delete value;
  }
};

class ReturnStatement final : public Statement {
public:
  Expression *return_value;
  ReturnStatement() = default;
  explicit ReturnStatement(const Token &tkn)
      : Statement(tkn), return_value(nullptr) {}
  explicit ReturnStatement(const Token &tkn, Expression *rtn_val)
      : Statement(tkn), return_value(rtn_val) {}
  [[nodiscard]] auto type() const -> Node override;
  [[nodiscard]] auto to_string() const -> std::string override;

  ~ReturnStatement() final { delete return_value; }
};

class ExpressionStatement final : public Statement {
public:
  Expression *expression;
  ExpressionStatement() = default;
  explicit ExpressionStatement(const Token &tkn)
      : Statement(tkn), expression(nullptr) {}
  explicit ExpressionStatement(const Token &tkn, Expression *exp)
      : Statement(tkn), expression(exp) {}
  [[nodiscard]] auto type() const -> Node override;
  [[nodiscard]] auto to_string() const -> std::string override;

  ~ExpressionStatement() final { delete expression; }
};

class Integer : public Expression {
public:
  const std::size_t value;
  explicit Integer(const Token &tkn) : Expression(tkn), value(0) {}
  Integer(const Token &tkn, const std::size_t val) : Expression(tkn), value(val) {}
  [[nodiscard]] auto type() const -> Node override;
  [[nodiscard]] auto to_string() const -> std::string override;
};

class Prefix final : public Expression {
public:
  const std::string operatr;
  Expression *right;
  Prefix(const Token &tkn, const std::string &optr)
      : Expression(tkn), operatr(optr), right(nullptr) {}
  Prefix(const Token &tkn, const std::string &optr, Expression *exp)
      : Expression(tkn), operatr(optr), right(exp) {}
  [[nodiscard]] auto type() const -> Node override;
  [[nodiscard]] auto to_string() const -> std::string override;

  ~Prefix() final { delete right; }
};

class Infix final : public Expression {
public:
  Expression *right;
  Expression *left;
  const std::string operatr;
  Infix(const Token &tkn, Expression *lft, const std::string &optr)
      : Expression(tkn), right(nullptr), left(lft), operatr(optr) {}
  Infix(const Token &tkn, Expression *lft, const std::string &optr,
        Expression *rht)
      : Expression(tkn), right(rht), left(lft), operatr(optr) {}
  [[nodiscard]] auto type() const -> Node override;
  [[nodiscard]] auto to_string() const -> std::string override;

  ~Infix() final
  {
    delete right;
    delete left;
  }
};

class Boolean : public Expression {
public:
  const bool value;
  explicit Boolean(const Token &tkn) : Expression(tkn), value(false) {}
  Boolean(const Token &tkn, const bool val) : Expression(tkn), value(val) {}
  [[nodiscard]] auto type() const -> Node override;
  [[nodiscard]] auto to_string() const -> std::string override;
};

class Block final : public Statement {
public:
  std::vector<Statement *> statements;
  Block(const Token &tkn, const std::vector<Statement *> &statements)
      : Statement(tkn), statements(statements) {}
  [[nodiscard]] auto type() const -> Node override;
  [[nodiscard]] auto to_string() const -> std::string override;

  ~Block() final
  {
    for (auto *statement : statements) {
      delete statement;
    }
  }
};

class LoopStatement final : public Statement {
public:
  Expression *condition;
  Block *repeat;
  explicit LoopStatement(const Token &tkn)
      : Statement(tkn), condition(nullptr), repeat(nullptr) {}
  LoopStatement(const Token &tkn, Expression *cond, Block *rept)
      : Statement(tkn), condition(cond), repeat(rept){}
  [[nodiscard]] auto type() const -> Node final { return Node::Loop; }
  [[nodiscard]] auto to_string() const -> std::string final;

  ~LoopStatement() final
  {
    delete condition;
    delete repeat;
  }
};

class If final : public Expression {
public:
  Expression *condition;
  Block *consequence;
  Block *alternative;
  explicit If(const Token &tkn)
      : Expression(tkn), condition(nullptr), consequence(nullptr),
        alternative(nullptr) {}
  If(const Token &tkn, Expression *cond, Block *cons)
      : Expression(tkn), condition(cond), consequence(cons),
        alternative(nullptr){}
  If(const Token &tkn, Expression *cond, Block *cons, Block *alt)
      : Expression(tkn), condition(cond), consequence(cons), alternative(alt) {}
  [[nodiscard]] auto type() const -> Node override { return Node::If; }
  [[nodiscard]] auto to_string() const -> std::string override;

  ~If() final
  {
    delete condition;
    delete consequence;
    delete alternative;
  }
};

class Function final : public Expression {
public:
  std::vector<Identifier *> parameters;
  Block *body;
  explicit Function(const Token &tkn,
                    const std::vector<Identifier *> &params = {})
      : Expression(tkn), parameters(params), body(nullptr) {}
  Function(const Token &tkn, const std::vector<Identifier *> &params,
           Block *body)
      : Expression(tkn), parameters(params), body(body){}
  [[nodiscard]] auto type() const -> Node override;
  [[nodiscard]] auto to_string() const -> std::string override;

  ~Function() final
  {
    delete body;
    for (auto *param : parameters) {
      delete param;
    }
  }
};

class Call final : public Expression {
public:
  Expression *function;
  std::vector<Expression *> arguments;
  Call(const Token &tkn, Expression *func) : Expression(tkn), function(func) {}
  Call(const Token &tkn, Expression *func,
       const std::vector<Expression *> &args)
      : Expression(tkn), function(func), arguments(args) {}
  [[nodiscard]] auto type() const -> Node override;
  [[nodiscard]] auto to_string() const -> std::string override;

  ~Call() final
  {
    delete function;
    for (auto *arg : arguments) {
      delete arg;
    }
  }
};

class StringLiteral : public Expression {
public:
  const std::string value;
  StringLiteral(const Token &tkn, const std::string &val)
      : Expression(tkn), value(val) {}
  [[nodiscard]] auto type() const -> Node override;
  [[nodiscard]] auto to_string() const -> std::string override;
};

class Null : public Expression {
public:
  explicit Null(const Token &tkn) : Expression(tkn) {}
  [[nodiscard]] auto type() const -> Node override;
  [[nodiscard]] auto to_string() const -> std::string override;
};

class AssignStatement : public Statement {
public:
  Identifier *name;
  Expression *value;
  AssignStatement(const Token &tkn, Identifier *ident, Expression *exp)
      : Statement(tkn), name(ident), value(exp) {}
  [[nodiscard]] auto type() const -> Node override;
  [[nodiscard]] auto to_string() const -> std::string override;
  ~AssignStatement()
  {
    delete name;
    delete value;
  }
};

class Programs_Guard {
  std::vector<Program *> programs;

public:
  Programs_Guard() = default;
  void push_back(Program *prog);
  auto new_program(const std::vector<Statement *> &statements) -> Program *;

  ~Programs_Guard()
  {
    for (auto *prog : programs) {
      delete prog;
    }
  }
};

} // namespace ast
#endif // AST_H
