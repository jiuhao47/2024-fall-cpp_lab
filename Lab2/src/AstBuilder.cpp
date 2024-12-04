#include "AstBuilder.h"
#include "AstNode.h"
#include "Utils.h"
// #include <exception>
// #include <iostream>
// #include <typeinfo>

namespace antlrcpp {
antlrcpp::Any AstBuilder::visitCompUnit(SafeCParser::CompUnitContext *ctx) {
  if (JJY_DEBUG_AST) {
    printf("%s visitCompUnit\n", JJY_DEBUG_SIGN);
  }
  auto result = new comp_unit_node;
  auto compile_units = ctx->children;

  result->line = ctx->getStart()->getLine();
  result->pos = ctx->getStart()->getCharPositionInLine();

  // Ignore the last EOF in the for loop.
  for (auto i = 0; i < compile_units.size() - 1; i++) {
    if (auto decl =
            dynamic_cast<SafeCParser::DeclContext *>(compile_units[i])) {
      // Global define
      auto global_def_stmt_n = visit(decl).as<stmt_node *>();

      auto var_def_stmt_n =
          dynamic_cast<var_def_stmt_node *>(global_def_stmt_n);
      result->comp_units.push_back(ptr<comp_unit_child_node>(var_def_stmt_n));
    } else if (auto func_def = dynamic_cast<SafeCParser::FuncDefContext *>(
                   compile_units[i])) {
      // Function define
      if (JJY_DEBUG_AST) {
        printf("%s visitCompUnit: func\n", JJY_DEBUG_SIGN);
      }
      auto func_def_n = visit(func_def).as<func_def_node *>();
      result->comp_units.push_back(ptr<comp_unit_child_node>(func_def_n));
    } else {
      // Error.
      log(compile_units[i]->getText());
      assert(0 && "Unknown Compile Unit.");
    }
  }
  if (JJY_DEBUG_AST)
    printf("%s %s [info] return result\n", JJY_DEBUG_SIGN, __func__);
  return result;
}

antlrcpp::Any AstBuilder::visitDecl(SafeCParser::DeclContext *ctx) {
  // decl: constDecl | varDecl;
  if (JJY_DEBUG_AST) {
    printf("%s visitDecl\n", JJY_DEBUG_SIGN);
  }
  if (auto const_decl = ctx->constDecl()) {
    return visit(const_decl);
  } else if (auto var_decl = ctx->varDecl()) {
    return visit(var_decl);
  } else {
    assert(0 && "Unknown DeclContext.");
  }
}

antlrcpp::Any AstBuilder::visitFuncDef(SafeCParser::FuncDefContext *ctx) {
  // TODO: FucDef
  //
  // funcDef: Void Identifier LeftParen RightParen block;
  // struct func_def_node : comp_unit_child_node {
  //   std::string name;
  //   ptr<block_node> body;
  //   virtual void accept(AstNode_Visitor &visitor) override;
  // };
  if (JJY_DEBUG_AST) {
    printf("%s visitFuncDef\n", JJY_DEBUG_SIGN);
  }

  auto result = new func_def_node;

  result->line = ctx->getStart()->getLine();
  result->pos = ctx->getStart()->getCharPositionInLine();

  result->name = ctx->Identifier()->getText();

  if (JJY_DEBUG_AST)
    printf("%s %s [info] %s %d %d\n", JJY_DEBUG_SIGN, __func__,
           result->name.c_str(), result->line, result->pos);

  if (JJY_DEBUG_AST)
    printf("%s %s [info] block_node before cast\n", JJY_DEBUG_SIGN, __func__);
  result->body.reset(visit(ctx->block()).as<block_node *>());
  if (JJY_DEBUG_AST)
    printf("%s %s [info] block_node after cast\n", JJY_DEBUG_SIGN, __func__);
  return result;
}

antlrcpp::Any AstBuilder::visitConstDecl(SafeCParser::ConstDeclContext *ctx) {
  // TODO: ConstDecl
  //
  // constDecl: Const bType constDef (Comma constDef)* SemiColon;
  // struct var_def_stmt_node : stmt_node, global_def_node {
  //   ptr_vector<var_def_node> var_defs;
  //   virtual void accept(AstNode_Visitor &visitor) override;
  // };
  if (JJY_DEBUG_AST) {
    printf("%s visitConstDecl\n", JJY_DEBUG_SIGN);
  }

  auto result = new var_def_stmt_node;
  result->line = ctx->getStart()->getLine();
  result->pos = ctx->getStart()->getCharPositionInLine();

  for (auto const_def : ctx->constDef()) {
    auto const_def_n = visit(const_def).as<var_def_node *>();
    if (JJY_DEBUG_AST)
      printf("%s %s [info] ConstDef Return before cast\n", JJY_DEBUG_SIGN,
             __func__);
    result->var_defs.push_back(ptr<var_def_node>(const_def_n));
    if (JJY_DEBUG_AST)
      printf("%s %s [info] ConstDef Return after cast\n", JJY_DEBUG_SIGN,
             __func__);
  }
  return dynamic_cast<stmt_node *>(result);
}

antlrcpp::Any AstBuilder::visitConstDef(SafeCParser::ConstDefContext *ctx) {
  // constDef: (Identifier | array) Assign (exp | (LeftBrace (exp (Comma exp)*)?
  // RightBrace));
  //
  // struct var_def_node : stmt_node {
  //     bool is_const;
  //     BType btype;
  //     std::string name;
  //     bool is_obc;
  //     ptr<expr_node> array_length;
  //     ptr_vector<expr_node> initializers;
  //     virtual void accept(AstNode_Visitor& visitor) override;
  // };
  if (JJY_DEBUG_AST) {
    printf("%s visitConstDef\n", JJY_DEBUG_SIGN);
  }
  auto result = new var_def_node;

  result->line = ctx->getStart()->getLine();
  result->pos = ctx->getStart()->getCharPositionInLine();
  result->is_const = true;
  result->btype = BType::INT;

  if (auto array = ctx->array()) {
    // TODO: Array
    return visit(array);
  } else if (ctx->Identifier()) {
    // TODO: Scalar
    result->name = ctx->Identifier()->getText();

    if (auto exp = ctx->exp(0)) {
      result->initializers.push_back(
          ptr<expr_node>(visit(exp).as<expr_node *>()));
    }
    return result;
  } else {
    assert(0 && "Unknown ConstDef");
  }
}

antlrcpp::Any AstBuilder::visitVarDecl(SafeCParser::VarDeclContext *ctx) {
  // TODO: varDecl
  //
  // struct var_def_stmt_node : stmt_node, global_def_node {
  //   ptr_vector<var_def_node> var_defs;
  //   virtual void accept(AstNode_Visitor &visitor) override;
  // };
  // varDecl: bType varDef (Comma varDef)* SemiColon;
  if (JJY_DEBUG_AST) {
    printf("%s visitVarDecl\n", JJY_DEBUG_SIGN);
  }
  auto result = new var_def_stmt_node;

  result->line = ctx->getStart()->getLine();
  result->pos = ctx->getStart()->getCharPositionInLine();

  for (auto var_def : ctx->varDef()) {
    auto var_def_n = visit(var_def).as<var_def_node *>();
    result->var_defs.push_back(ptr<var_def_node>(var_def_n));
  }

  return dynamic_cast<stmt_node *>(result);
}

antlrcpp::Any AstBuilder::visitBType(SafeCParser::BTypeContext *ctx) {
  if (JJY_DEBUG_AST) {
    printf("%s visitBType\n", JJY_DEBUG_SIGN);
  }
  /* Nothing to do here. */
  return NULL;
}

antlrcpp::Any AstBuilder::visitVarDef(SafeCParser::VarDefContext *ctx) {
  // struct var_def_node : stmt_node {
  //   bool is_const;
  //   BType btype;
  //   std::string name;
  //   bool is_obc;
  //   ptr<expr_node> array_length;
  //   ptr_vector<expr_node> initializers;
  //   virtual void accept(AstNode_Visitor &visitor) override;
  // };
  // varDef: (Identifier | array) (Assign (exp | (LeftBrace (exp (Comma exp)*)?
  // RightBrace)))?;
  if (JJY_DEBUG_AST) {
    printf("%s visitVarDef\n", JJY_DEBUG_SIGN);
  }
  if (auto array = ctx->array()) {
    // TODO: Array
    return visit(array);
  } else if (ctx->Identifier()) {
    // TODO: Scalar

    auto result = new var_def_node;

    result->line = ctx->getStart()->getLine();
    result->pos = ctx->getStart()->getCharPositionInLine();

    result->is_const = false;
    result->btype = BType::INT;
    result->name = ctx->Identifier()->getText();

    if (auto exp = ctx->exp(0)) {
      result->initializers.push_back(
          ptr<expr_node>(visit(exp).as<expr_node *>()));
    }
    return result;
  } else {
    assert(0 && "Unknown VarDef");
  }
}

antlrcpp::Any AstBuilder::visitArray(SafeCParser::ArrayContext *ctx) {
  // TODO: Array
  //
  // array: obcArray | unobcArray;
  if (JJY_DEBUG_AST) {
    printf("%s visitArray\n", JJY_DEBUG_SIGN);
  }
  if (auto obc_array = ctx->obcArray()) {
    return visit(obc_array);
  } else if (auto unobc_array = ctx->unobcArray()) {
    return visit(unobc_array);
  } else {
    assert(0 && "Unknown Array.");
  }
}

antlrcpp::Any AstBuilder::visitObcArray(SafeCParser::ObcArrayContext *ctx) {
  // TODO: ObcArray
  //
  // obcArray: Obc unobcArray;
  // struct var_def_node : stmt_node {
  //   bool is_const;
  //   BType btype;
  //   std::string name;
  //   bool is_obc;
  //   ptr<expr_node> array_length;
  //   ptr_vector<expr_node> initializers;
  //   virtual void accept(AstNode_Visitor &visitor) override;
  // };
  if (JJY_DEBUG_AST) {
    printf("%s visitObcArray\n", JJY_DEBUG_SIGN);
  }

  auto result = new var_def_node;

  result->line = ctx->getStart()->getLine();
  result->pos = ctx->getStart()->getCharPositionInLine();

  result->is_obc = true;
  result->btype = BType::INT;
  result->name = ctx->unobcArray()->Identifier()->getText();
  if (JJY_DEBUG_AST)
    printf("%s %s [info] %s %d %d %d\n", JJY_DEBUG_SIGN, __func__,
           result->name.c_str(), result->is_obc, result->line, result->pos);

  if (auto exp = ctx->unobcArray()->exp()) {
    if (JJY_DEBUG_AST)
      printf("%s %s [info] exp before cast\n", JJY_DEBUG_SIGN, __func__);

    result->array_length.reset(visit(exp).as<expr_node *>());

    if (JJY_DEBUG_AST)
      printf("%s %s [info] exp after cast\n", JJY_DEBUG_SIGN, __func__);
  }

  return result;
}

antlrcpp::Any AstBuilder::visitUnobcArray(SafeCParser::UnobcArrayContext *ctx) {
  // TODO: UnobcArray
  //
  // unobcArray: Identifier LeftBracket (exp)? RightBracket;
  if (JJY_DEBUG_AST) {
    printf("%s visitUnobcArray\n", JJY_DEBUG_SIGN);
  }
  auto result = new var_def_node;
  result->line = ctx->getStart()->getLine();
  result->pos = ctx->getStart()->getCharPositionInLine();

  result->is_obc = false;
  result->btype = BType::INT;
  result->name = ctx->Identifier()->getText();

  if (auto exp = ctx->exp()) {
    result->array_length.reset(visit(exp).as<expr_node *>());
  }

  return result;
}

antlrcpp::Any AstBuilder::visitBlock(SafeCParser::BlockContext *ctx) {
  if (JJY_DEBUG_AST) {
    printf("%s visitBlock\n", JJY_DEBUG_SIGN);
  }
  auto result = new block_node;

  result->line = ctx->getStart()->getLine();
  result->pos = ctx->getStart()->getCharPositionInLine();

  for (auto block_item : ctx->blockItem()) {
    stmt_node *block_item_n;
    auto block_item_stmt = block_item->stmt();
    if (block_item_stmt != nullptr && block_item_stmt->block() != nullptr) {
      block_item_n =
          dynamic_cast<stmt_node *>(visit(block_item).as<block_node *>());
    } else {
      block_item_n = visit(block_item).as<stmt_node *>();
    }
    result->body.push_back(ptr<stmt_node>(block_item_n));
  }
  return dynamic_cast<block_node *>(result);
}

antlrcpp::Any AstBuilder::visitBlockItem(SafeCParser::BlockItemContext *ctx) {
  // TODO: blockItem
  //
  // blockItem: decl | stmt;
  if (JJY_DEBUG_AST) {
    printf("%s visitBlockItem\n", JJY_DEBUG_SIGN);
  }
  if (auto decl = ctx->decl()) {
    return visit(decl);
  } else if (auto stmt = ctx->stmt()) {
    return visit(stmt);
  } else {
    assert(0 && "Unknown BlockItem.");
  }
}

antlrcpp::Any AstBuilder::visitStmt(SafeCParser::StmtContext *ctx) {
  // TODO: Stmt
  //
  // stmt:
  //     block
  //     | SemiColon
  //     | exp SemiColon
  //     | If LeftParen cond RightParen stmt (Else stmt)?
  //     | While LeftParen cond RightParen stmt;
  if (JJY_DEBUG_AST) {
    printf("%s visitStmt\n", JJY_DEBUG_SIGN);
  }
  if (ctx->Assign()) {
    if (JJY_DEBUG_AST)
      printf("%s %s [info] Assign\n", JJY_DEBUG_SIGN, __func__);

    auto result = new assign_stmt_node;
    result->line = ctx->getStart()->getLine();
    result->pos = ctx->getStart()->getCharPositionInLine();
    result->target.reset(visit(ctx->lval()).as<lval_node *>());
    result->value.reset(visit(ctx->exp()).as<expr_node *>());
    return dynamic_cast<stmt_node *>(result);
  }
  if (auto block = ctx->block()) {
    if (JJY_DEBUG_AST)
      printf("%s %s [info] Stmt block\n", JJY_DEBUG_SIGN, __func__);
    return visit(block);
  } else if (ctx->isEmpty()) {
    if (JJY_DEBUG_AST)
      printf("%s %s [info] Stmt Empty\n", JJY_DEBUG_SIGN, __func__);
    auto result = new empty_stmt_node;
    return dynamic_cast<stmt_node *>(result);
  } else if (auto exp = ctx->exp()) {
    if (JJY_DEBUG_AST)
      printf("%s %s [info] Stmt exp\n", JJY_DEBUG_SIGN, __func__);
    return visit(exp);
  } else if (auto if_stmt = ctx->If()) {
    if (JJY_DEBUG_AST)
      printf("%s %s [info] Stmt If\n", JJY_DEBUG_SIGN, __func__);

    auto result = new if_stmt_node;
    result->line = ctx->getStart()->getLine();
    result->pos = ctx->getStart()->getCharPositionInLine();
    result->cond.reset(visit(ctx->cond()).as<cond_node *>());

    block_node *if_body;
    if_body = visit(ctx->stmt(0)).as<block_node *>();
    result->if_body.reset(dynamic_cast<stmt_node *>(if_body));

    if (auto stmt = ctx->stmt(1)) {
      block_node *else_body;
      if (JJY_DEBUG_AST)
        printf("%s %s [info] Stmt Else\n", JJY_DEBUG_SIGN, __func__);
      if (stmt->If()) {
        if (JJY_DEBUG_AST)
          printf("%s %s [info] Stmt Else If\n", JJY_DEBUG_SIGN, __func__);
        result->else_body.reset(visit(stmt).as<stmt_node *>());
      } else {
        else_body = visit(stmt).as<block_node *>();
        result->else_body.reset(dynamic_cast<stmt_node *>(else_body));
      }
    }
    return dynamic_cast<stmt_node *>(result);
  } else if (auto while_stmt = ctx->While()) {
    if (JJY_DEBUG_AST)
      printf("%s %s [info] Stmt While\n", JJY_DEBUG_SIGN, __func__);
    auto result = new while_stmt_node;

    result->line = ctx->getStart()->getLine();
    result->pos = ctx->getStart()->getCharPositionInLine();
    result->cond.reset(visit(ctx->cond()).as<cond_node *>());
    result->body.reset(visit(ctx->stmt(0)).as<block_node *>());

    return dynamic_cast<stmt_node *>(result);
  } else {
    assert(0 && "Unknown Stmt.");
  }
}

antlrcpp::Any AstBuilder::visitCond(SafeCParser::CondContext *ctx) {
  // TODO: Cond
  //
  // cond: exp;
  if (JJY_DEBUG_AST) {
    printf("%s visitCond\n", JJY_DEBUG_SIGN);
  }
  return visit(ctx->exp());
}

antlrcpp::Any AstBuilder::visitLval(SafeCParser::LvalContext *ctx) {
  // TODO: Lval
  //
  // lval: Identifier | (Identifier LeftBracket exp RightBracket) | (Identifier
  // LeftParen RightParen );

  // struct lval_node : expr_node {
  //   std::string name;
  //   ptr<expr_node> array_index;
  //   virtual void accept(AstNode_Visitor &visitor) override;
  // };
  if (JJY_DEBUG_AST) {
    printf("%s visitLval\n", JJY_DEBUG_SIGN);
  }
  if (ctx->LeftParen() && ctx->RightParen()) {
    if (JJY_DEBUG_AST)
      printf("%s %s [info] func_call_stmt_node\n", JJY_DEBUG_SIGN, __func__);
    auto result = new func_call_stmt_node;
    result->line = ctx->getStart()->getLine();
    result->pos = ctx->getStart()->getCharPositionInLine();
    result->name = ctx->Identifier()->getText();
    return dynamic_cast<stmt_node *>(result);
  } else {
    auto result = new lval_node;
    result->line = ctx->getStart()->getLine();
    result->pos = ctx->getStart()->getCharPositionInLine();
    result->name = ctx->Identifier()->getText();

    if (JJY_DEBUG_AST)
      printf("%s %s [info] %s %d %d\n", JJY_DEBUG_SIGN, __func__,
             result->name.c_str(), result->line, result->pos);

    if (auto exp = ctx->exp()) {
      result->array_index.reset(visit(exp).as<expr_node *>());
    }

    // return dynamic_cast<expr_node *>(result);
    return result;
  }
}

antlrcpp::Any AstBuilder::visitNumber(SafeCParser::NumberContext *ctx) {
  if (JJY_DEBUG_AST) {
    printf("%s visitNumber\n", JJY_DEBUG_SIGN);
  }
  auto result = new number_node;
  result->btype = BType::INT;
  result->line = ctx->getStart()->getLine();
  result->pos = ctx->getStart()->getCharPositionInLine();
  auto num_str = ctx->IntConst()->getText();

  if (num_str.size() > 2 && num_str[0] == '0' &&
      (num_str[1] == 'x' || num_str[1] == 'X')) {
    // Hex
    result->number = std::stoi(num_str, 0, 16);
  } else {
    // Dec
    result->number = std::stoi(num_str, 0, 10);
  }
  if (JJY_DEBUG_AST)
    printf("%s %s [info] %d %d\n", JJY_DEBUG_SIGN, __func__, result->number,
           result->line);
  return dynamic_cast<expr_node *>(result);
}

antlrcpp::Any AstBuilder::visitExp(SafeCParser::ExpContext *ctx) {
  // exp: lval Assign exp
  //     | lval
  //     | number
  //     | LeftParen exp RightParen
  //     | exp (Plus | Minus | Multiply | Divide | Modulo) exp
  //     | (Minus | Plus) exp
  //     | exp (Equal | NonEqual | Less | Greater | LessEqual | GreaterEqual)
  //     exp;
  if (JJY_DEBUG_AST) {
    printf("%s visitExp\n", JJY_DEBUG_SIGN);
  }
  auto exps = ctx->exp();
  if (ctx->lval()) {
    if (JJY_DEBUG_AST)
      printf("%s %s [info] lval\n", JJY_DEBUG_SIGN, __func__);
    // 左值: lval
    if (ctx->lval()->RightParen() && ctx->lval()->LeftParen()) {
      return visit(ctx->lval());
    } else {
      auto result = new lval_node;
      result = visit(ctx->lval()).as<lval_node *>();

      return dynamic_cast<expr_node *>(result);
    }
  } else if (ctx->number()) {
    if (JJY_DEBUG_AST)
      printf("%s %s [info] number\n", JJY_DEBUG_SIGN, __func__);
    // 数字: number
    return visit(ctx->number());
  }

  else if (ctx->LeftParen() && ctx->RightParen()) {
    if (JJY_DEBUG_AST)
      printf("%s %s [info] (exp)\n", JJY_DEBUG_SIGN, __func__);
    // 括号表达式: (exp)
    return visit(exps[0]).as<expr_node *>();
  }

  else if (exps.size() == 2) {
    // 二元运算: exp (op) exp
    bool is_bin_op = ctx->Plus() || ctx->Minus() || ctx->Multiply() ||
                     ctx->Divide() || ctx->Modulo();
    bool is_rel_op = ctx->Equal() || ctx->NonEqual() || ctx->Less() ||
                     ctx->LessEqual() || ctx->Greater() || ctx->GreaterEqual();
    if (is_bin_op) {
      if (JJY_DEBUG_AST)
        printf("%s %s [info] BinOp\n", JJY_DEBUG_SIGN, __func__);
      auto result = new binop_expr_node;
      result->line = ctx->getStart()->getLine();
      result->pos = ctx->getStart()->getCharPositionInLine();
      if (ctx->Plus()) {
        result->op = BinOp::PLUS;
      } else if (ctx->Minus()) {
        result->op = BinOp::MINUS;
      } else if (ctx->Multiply()) {
        result->op = BinOp::MULTIPLY;
      } else if (ctx->Divide()) {
        result->op = BinOp::DIVIDE;
      } else if (ctx->Modulo()) {
        result->op = BinOp::MODULO;
      } else {
        assert(0 && "Unknown binary operator in exp");
      }

      result->lhs.reset(visit(exps[0]).as<expr_node *>());
      if (JJY_DEBUG_AST)
        printf("%s %s [info] after exp0 cast\n", JJY_DEBUG_SIGN, __func__);
      result->rhs.reset(visit(exps[1]).as<expr_node *>());
      if (JJY_DEBUG_AST)
        printf("%s %s [info] after exp1 cast\n", JJY_DEBUG_SIGN, __func__);

      return dynamic_cast<expr_node *>(result);
    } else if (is_rel_op) {
      if (JJY_DEBUG_AST)
        printf("%s %s [info] RelOp\n", JJY_DEBUG_SIGN, __func__);
      auto result = new cond_node;
      result->line = ctx->getStart()->getLine();
      result->pos = ctx->getStart()->getCharPositionInLine();
      if (ctx->Equal()) {
        result->op = RelOp::EQUAL;
      } else if (ctx->NonEqual()) {
        result->op = RelOp::NON_EQUAL;
      } else if (ctx->Less()) {
        result->op = RelOp::LESS;
      } else if (ctx->LessEqual()) {
        result->op = RelOp::LESS_EQUAL;
      } else if (ctx->Greater()) {
        result->op = RelOp::GREATER;
      } else if (ctx->GreaterEqual()) {
        result->op = RelOp::GREATER_EQUAL;
      } else {
        assert(0 && "Unknown binary operator in exp");
      }

      result->lhs.reset(visit(exps[0]).as<expr_node *>());
      result->rhs.reset(visit(exps[1]).as<expr_node *>());

      return result;
    }
  }

  else if (exps.size() == 1) {
    if (JJY_DEBUG_AST)
      printf("%s %s [info] UnaryOp\n", JJY_DEBUG_SIGN, __func__);
    auto result = new unaryop_expr_node;
    result->line = ctx->getStart()->getLine();
    result->pos = ctx->getStart()->getCharPositionInLine();

    if (ctx->Plus()) {
      result->op = UnaryOp::PLUS;
    } else if (ctx->Minus()) {
      result->op = UnaryOp::MINUS;
    } else {
      assert(0 && "Unknown unary operator in exp");
    }

    result->rhs.reset(visit(exps[0]).as<expr_node *>());
    return dynamic_cast<expr_node *>(result);
  }

  assert(0 && "Unknown Exp.");
}

ptr<ast_node> AstBuilder::operator()(antlr4::tree::ParseTree *ctx) {
  auto result = visit(ctx);
  if (result.is<ast_node *>())
    return ptr<ast_node>(result.as<ast_node *>());
  if (result.is<comp_unit_node *>())
    return ptr<ast_node>(result.as<comp_unit_node *>());
  if (result.is<comp_unit_child_node *>())
    return ptr<ast_node>(result.as<comp_unit_child_node *>());
  if (result.is<func_def_node *>())
    return ptr<ast_node>(result.as<func_def_node *>());
  if (result.is<expr_node *>())
    return ptr<ast_node>(result.as<expr_node *>());
  if (result.is<cond_node *>())
    return ptr<ast_node>(result.as<cond_node *>());
  if (result.is<binop_expr_node *>())
    return ptr<ast_node>(result.as<binop_expr_node *>());
  if (result.is<unaryop_expr_node *>())
    return ptr<ast_node>(result.as<unaryop_expr_node *>());
  if (result.is<lval_node *>())
    return ptr<ast_node>(result.as<lval_node *>());
  if (result.is<number_node *>())
    return ptr<ast_node>(result.as<number_node *>());
  if (result.is<stmt_node *>())
    return ptr<ast_node>(result.as<stmt_node *>());
  if (result.is<var_def_stmt_node *>())
    return ptr<ast_node>(result.as<var_def_stmt_node *>());
  if (result.is<assign_stmt_node *>())
    return ptr<ast_node>(result.as<assign_stmt_node *>());
  if (result.is<func_call_stmt_node *>())
    return ptr<ast_node>(result.as<func_call_stmt_node *>());
  if (result.is<block_node *>())
    return ptr<ast_node>(result.as<block_node *>());
  if (result.is<if_stmt_node *>())
    return ptr<ast_node>(result.as<if_stmt_node *>());
  if (result.is<while_stmt_node *>())
    return ptr<ast_node>(result.as<while_stmt_node *>());
  if (result.is<empty_stmt_node *>())
    return ptr<ast_node>(result.as<empty_stmt_node *>());
  return ptr<ast_node>(result.as<ast_node *>());
}
} // namespace antlrcpp
