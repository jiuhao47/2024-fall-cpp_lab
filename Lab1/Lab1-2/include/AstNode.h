#ifndef _ASTNODE_
#define _ASTNODE_

#include <memory>
#include <string>
#include <vector>

// 智能指针
template <typename T> using ptr = std::shared_ptr<T>;

// 指针数组
template <typename T> using ptr_vector = std::vector<ptr<T>>;

// 二元运算符
enum class BinOp { PLUS = 0, MINUS, MULTIPLY, DIVIDE, MODULO };

// 一元运算符
enum class UnaryOp { PLUS = 0, MINUS };

// 关系运算符
enum class RelOp {
  EQUAL = 0,
  NON_EQUAL,
  LESS,
  LESS_EQUAL,
  GREATER,
  GREATER_EQUAL
};

// 基本类型
enum class BType {
  INT = 0,
};

struct ast_node;
struct comp_unit_node;
struct comp_unit_child_node;
struct global_def_node;
struct func_def_node;
struct expr_node;
struct cond_node;
struct binop_expr_node;
struct unaryop_expr_node;
struct lval_node;
struct number_node;
struct stmt_node;
struct var_def_stmt_node;
struct var_def_node;
struct assign_stmt_node;
struct func_call_stmt_node;
struct block_node;
struct if_stmt_node;
struct while_stmt_node;
struct empty_stmt_node;

struct AstNode_Visitor;

// 抽象语法树节点
struct ast_node {
  int line;
  int pos;
  virtual void accept(AstNode_Visitor &visitor) = 0;
};

// 编译单元节点
struct comp_unit_node : virtual ast_node {
  ptr_vector<comp_unit_child_node> comp_units;

  virtual void accept(AstNode_Visitor &visitor) override;
};

// 编译单元子节点
struct comp_unit_child_node : virtual ast_node {
  virtual void accept(AstNode_Visitor &visitor) override;
};

// 全局定义节点
struct global_def_node : comp_unit_child_node {
  virtual void accept(AstNode_Visitor &visitor) override;
};

// 函数定义节点
struct func_def_node : comp_unit_child_node {
  std::string name;
  ptr<block_node> body;
  virtual void accept(AstNode_Visitor &visitor) override;
};

// 表达式节点
struct expr_node : virtual ast_node {
  virtual void accept(AstNode_Visitor &visitor) override;
};

// 条件节点
struct cond_node : expr_node {
  RelOp op;
  ptr<expr_node> lhs, rhs;
  virtual void accept(AstNode_Visitor &visitor) override;
};

// 二元运算节点
struct binop_expr_node : expr_node {
  BinOp op;
  ptr<expr_node> lhs, rhs;
  virtual void accept(AstNode_Visitor &visitor) override;
};

// 一元运算节点
struct unaryop_expr_node : expr_node {
  UnaryOp op;
  ptr<expr_node> rhs;
  virtual void accept(AstNode_Visitor &visitor) override;
};

// 左值节点
struct lval_node : expr_node {
  std::string name;
  ptr<expr_node> array_index;
  virtual void accept(AstNode_Visitor &visitor) override;
};

// 数字节点
struct number_node : expr_node {
  BType btype;
  int number;
  virtual void accept(AstNode_Visitor &visitor) override;
};

// 语句节点
struct stmt_node : virtual ast_node {
  virtual void accept(AstNode_Visitor &visitor) override;
};

// 变量定义语句
struct var_def_stmt_node : stmt_node, global_def_node {
  ptr_vector<var_def_node> var_defs;
  virtual void accept(AstNode_Visitor &visitor) override;
};

// 变量定义节点
struct var_def_node : stmt_node {
  bool is_const;
  BType btype;
  std::string name;
  bool is_obc;
  ptr<expr_node> array_length;
  ptr_vector<expr_node> initializers;
  virtual void accept(AstNode_Visitor &visitor) override;
};

// 赋值语句
struct assign_stmt_node : stmt_node {
  ptr<lval_node> target;
  ptr<expr_node> value;
  virtual void accept(AstNode_Visitor &visitor) override;
};

// 函数调用语句
struct func_call_stmt_node : stmt_node {
  std::string name;
  virtual void accept(AstNode_Visitor &visitor) override;
};

// 代码块
struct block_node : stmt_node {
  ptr_vector<stmt_node> body;
  virtual void accept(AstNode_Visitor &visitor) override;
};

// if 语句
struct if_stmt_node : stmt_node {
  ptr<cond_node> cond;
  ptr<stmt_node> if_body;
  ptr<stmt_node> else_body;
  virtual void accept(AstNode_Visitor &visitor) override;
};

// while 语句
struct while_stmt_node : stmt_node {
  ptr<cond_node> cond;
  ptr<stmt_node> body;
  virtual void accept(AstNode_Visitor &visitor) override;
};

// 空语句
struct empty_stmt_node : stmt_node {
  virtual void accept(AstNode_Visitor &visitor) override;
};

#endif
