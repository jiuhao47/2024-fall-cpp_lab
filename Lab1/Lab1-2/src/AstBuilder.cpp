#include "AstBuilder.h"
#define JJY_DEBUG 1
#define JJY_DEBUG_SIGN "[j] "

namespace antlrcpp {
antlrcpp::Any AstBuilder::visitCompUnit(SafeCParser::CompUnitContext *ctx) {
  if (JJY_DEBUG) {
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
      if (JJY_DEBUG) {
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
  if (JJY_DEBUG)
    printf("%s %s [info] return result\n", JJY_DEBUG_SIGN, __func__);
  return result;
}

antlrcpp::Any AstBuilder::visitDecl(SafeCParser::DeclContext *ctx) {
  if (JJY_DEBUG) {
    printf("%s visitDecl\n", JJY_DEBUG_SIGN);
  }

  // decl: constDecl | varDecl;

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
  if (JJY_DEBUG) {
    printf("%s visitFuncDef\n", JJY_DEBUG_SIGN);
  }
  // struct func_def_node : comp_unit_child_node {
  //   std::string name;
  //   ptr<block_node> body;
  //   virtual void accept(AstNode_Visitor &visitor) override;
  // };

  auto result = new func_def_node;

  result->line = ctx->getStart()->getLine();
  result->pos = ctx->getStart()->getCharPositionInLine();

  result->name = ctx->Identifier()->getText();

  if (JJY_DEBUG)
    printf("%s %s [info] %s %d %d\n", JJY_DEBUG_SIGN, __func__,
           result->name.c_str(), result->line, result->pos);

  if (JJY_DEBUG)
    printf("%s %s [info] block_node before cast\n", JJY_DEBUG_SIGN, __func__);
  result->body.reset(visit(ctx->block()).as<block_node *>());
  if (JJY_DEBUG)
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
  if (JJY_DEBUG) {
    printf("%s visitConstDecl\n", JJY_DEBUG_SIGN);
  }

  auto result = new var_def_stmt_node;
  result->line = ctx->getStart()->getLine();
  result->pos = ctx->getStart()->getCharPositionInLine();

  for (auto const_def : ctx->constDef()) {
    auto const_def_n = visit(const_def).as<var_def_node *>();
    if (JJY_DEBUG)
      printf("%s %s [info] ConstDef Return before cast\n", JJY_DEBUG_SIGN,
             __func__);
    result->var_defs.push_back(ptr<var_def_node>(const_def_n));
    if (JJY_DEBUG)
      printf("%s %s [info] ConstDef Return after cast\n", JJY_DEBUG_SIGN,
             __func__);
  }

  return dynamic_cast<stmt_node *>(result);
}

antlrcpp::Any AstBuilder::visitConstDef(SafeCParser::ConstDefContext *ctx) {
  if (JJY_DEBUG) {
    printf("%s visitConstDef\n", JJY_DEBUG_SIGN);
  }

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

  auto result = new var_def_node;

  result->line = ctx->getStart()->getLine();
  result->pos = ctx->getStart()->getCharPositionInLine();
  result->is_const = true;

  if (auto array = ctx->array()) {
    // TODO: Array
    return visit(array);
  } else if (ctx->Identifier()) {
    // TODO: Scalar
    //
    result->name = ctx->Identifier()->getText();

    if (auto exp = ctx->exp(0)) {
      // if (exp->number()) {
      //   result->initializers.push_back(
      //       ptr<number_node>(visit(exp).as<number_node *>()));
      // }
      result->initializers.push_back(
          ptr<expr_node>(visit(exp).as<expr_node *>()));
      // XXXX: more logic need to be done
    }
    return result;
  } else {
    assert(0 && "Unknown ConstDef");
  }
}

antlrcpp::Any AstBuilder::visitVarDecl(SafeCParser::VarDeclContext *ctx) {
  if (JJY_DEBUG) {
    printf("%s visitVarDecl\n", JJY_DEBUG_SIGN);
  }
  // TODO: varDecl

  // struct var_def_stmt_node : stmt_node, global_def_node {
  //   ptr_vector<var_def_node> var_defs;
  //   virtual void accept(AstNode_Visitor &visitor) override;
  // };

  // varDecl: bType varDef (Comma varDef)* SemiColon;
  //

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
  if (JJY_DEBUG) {
    printf("%s visitBType\n", JJY_DEBUG_SIGN);
  }
  /* Nothing to do here. */
  return NULL;
}

antlrcpp::Any AstBuilder::visitVarDef(SafeCParser::VarDefContext *ctx) {
  if (JJY_DEBUG) {
    printf("%s visitVarDef\n", JJY_DEBUG_SIGN);
  }

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
  if (auto array = ctx->array()) {
    // TODO: Array

    return visit(array);
  } else if (ctx->Identifier()) {
    // TODO: Scalar
    //

    auto result = new var_def_node;

    result->line = ctx->getStart()->getLine();
    result->pos = ctx->getStart()->getCharPositionInLine();

    result->is_const = false;
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
  if (JJY_DEBUG) {
    printf("%s visitArray\n", JJY_DEBUG_SIGN);
  }
  // TODO: Array
  //
  // array: obcArray | unobcArray;

  if (auto obc_array = ctx->obcArray()) {
    return visit(obc_array);
  } else if (auto unobc_array = ctx->unobcArray()) {
    return visit(unobc_array);
  } else {
    assert(0 && "Unknown Array.");
  }
}

antlrcpp::Any AstBuilder::visitObcArray(SafeCParser::ObcArrayContext *ctx) {
  if (JJY_DEBUG) {
    printf("%s visitObcArray\n", JJY_DEBUG_SIGN);
  }
  // TODO: ObcArray
  //
  // obcArray: Obc unobcArray;

  auto result = new var_def_node;

  result->line = ctx->getStart()->getLine();
  result->pos = ctx->getStart()->getCharPositionInLine();

  result->is_obc = true;
  result->name = ctx->unobcArray()->Identifier()->getText();
  if (JJY_DEBUG)
    printf("%s %s [info] %s %d %d %d\n", JJY_DEBUG_SIGN, __func__,
           result->name.c_str(), result->is_obc, result->line, result->pos);

  if (auto exp = ctx->unobcArray()->exp()) {
    if (JJY_DEBUG)
      printf("%s %s [info] exp before cast\n", JJY_DEBUG_SIGN, __func__);

    result->array_length.reset(visit(exp).as<expr_node *>());

    if (JJY_DEBUG)
      printf("%s %s [info] exp after cast\n", JJY_DEBUG_SIGN, __func__);
  }

  return result;
}

antlrcpp::Any AstBuilder::visitUnobcArray(SafeCParser::UnobcArrayContext *ctx) {
  if (JJY_DEBUG) {
    printf("%s visitUnobcArray\n", JJY_DEBUG_SIGN);
  }
  // TODO: UnobcArray
  //
  // unobcArray: Identifier LeftBracket (exp)? RightBracket;

  auto result = new var_def_node;

  result->line = ctx->getStart()->getLine();
  result->pos = ctx->getStart()->getCharPositionInLine();

  result->is_obc = false;
  result->name = ctx->Identifier()->getText();

  if (auto exp = ctx->exp()) {
    result->array_length.reset(visit(exp).as<expr_node *>());
  }

  return result;
}

antlrcpp::Any AstBuilder::visitBlock(SafeCParser::BlockContext *ctx) {
  if (JJY_DEBUG) {
    printf("%s visitBlock\n", JJY_DEBUG_SIGN);
  }
  auto result = new block_node;

  result->line = ctx->getStart()->getLine();
  result->pos = ctx->getStart()->getCharPositionInLine();

  for (auto block_item : ctx->blockItem()) {
    auto block_item_n = visit(block_item).as<stmt_node *>();
    result->body.push_back(ptr<stmt_node>(block_item_n));
  }
  return dynamic_cast<stmt_node *>(result);
}

antlrcpp::Any AstBuilder::visitBlockItem(SafeCParser::BlockItemContext *ctx) {
  if (JJY_DEBUG) {
    printf("%s visitBlockItem\n", JJY_DEBUG_SIGN);
  }
  // TODO: blockItem
  //
  // blockItem: decl | stmt;

  if (auto decl = ctx->decl()) {
    return visit(decl);

  } else if (auto stmt = ctx->stmt()) {
    return visit(stmt);

  } else {
    assert(0 && "Unknown BlockItem.");
  }
}

antlrcpp::Any AstBuilder::visitStmt(SafeCParser::StmtContext *ctx) {
  if (JJY_DEBUG) {
    printf("%s visitStmt\n", JJY_DEBUG_SIGN);
  }
  // TODO: Stmt

  // stmt:
  //     block
  //     | SemiColon
  //     | exp SemiColon
  //     | If LeftParen cond RightParen stmt (Else stmt)?
  //     | While LeftParen cond RightParen stmt;

  if (auto block = ctx->block()) {
    if (JJY_DEBUG)
      printf("%s %s [info] Stmt block\n", JJY_DEBUG_SIGN, __func__);
    return visit(block);

  } else if (ctx->isEmpty()) {
    if (JJY_DEBUG)
      printf("%s %s [info] Stmt SemiColon\n", JJY_DEBUG_SIGN, __func__);
    return visit(ctx->SemiColon());

  } else if (auto exp = ctx->exp()) {
    if (JJY_DEBUG)
      printf("%s %s [info] Stmt exp\n", JJY_DEBUG_SIGN, __func__);
    return visit(exp);

  } else if (auto if_stmt = ctx->If()) {
    if (JJY_DEBUG)
      printf("%s %s [info] Stmt If\n", JJY_DEBUG_SIGN, __func__);
    return visit(if_stmt);

  } else if (auto while_stmt = ctx->While()) {
    if (JJY_DEBUG)
      printf("%s %s [info] Stmt While\n", JJY_DEBUG_SIGN, __func__);
    return visit(while_stmt);

  } else {
    assert(0 && "Unknown Stmt.");
  }
}

antlrcpp::Any AstBuilder::visitCond(SafeCParser::CondContext *ctx) {
  if (JJY_DEBUG) {
    printf("%s visitCond\n", JJY_DEBUG_SIGN);
  }
  // TODO: Cond
  //
  // cond: exp;

  return visit(ctx->exp());
}

antlrcpp::Any AstBuilder::visitLval(SafeCParser::LvalContext *ctx) {
  if (JJY_DEBUG) {
    printf("%s visitLval\n", JJY_DEBUG_SIGN);
  }
  // TODO: Lval

  // lval: Identifier | (Identifier LeftBracket exp RightBracket) | (Identifier
  // LeftParen RightParen );

  // struct lval_node : expr_node {
  //   std::string name;
  //   ptr<expr_node> array_index;
  //   virtual void accept(AstNode_Visitor &visitor) override;
  // };

  auto result = new lval_node;

  result->line = ctx->getStart()->getLine();
  result->pos = ctx->getStart()->getCharPositionInLine();

  result->name = ctx->Identifier()->getText();

  if (JJY_DEBUG)
    printf("%s %s [info] %s %d %d\n", JJY_DEBUG_SIGN, __func__,
           result->name.c_str(), result->line, result->pos);

  if (auto exp = ctx->exp()) {
    result->array_index.reset(visit(exp).as<expr_node *>());
  }

  return dynamic_cast<expr_node *>(result);
}

antlrcpp::Any AstBuilder::visitNumber(SafeCParser::NumberContext *ctx) {
  if (JJY_DEBUG) {
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
  return dynamic_cast<expr_node *>(result);
}

antlrcpp::Any AstBuilder::visitExp(SafeCParser::ExpContext *ctx) {
  if (JJY_DEBUG) {
    printf("%s visitExp\n", JJY_DEBUG_SIGN);
  }

  // XXXX:有大问题啊这里的解析逻辑

  // exp: lval Assign exp
  //     | lval
  //     | number
  //     | LeftParen exp RightParen
  //     | exp (Plus | Minus | Multiply | Divide | Modulo) exp
  //     | (Minus | Plus) exp
  //     | exp (Equal | NonEqual | Less | Greater | LessEqual | GreaterEqual)
  //     exp;
  auto exps = ctx->exp();

  try {
    if (exps.size() == 2) {
      // binary op
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
      }

      result->lhs.reset(visit(ctx->exp(0)).as<expr_node *>());
      result->rhs.reset(visit(ctx->exp(1)).as<expr_node *>());

      return dynamic_cast<expr_node *>(result);

    } else if (exps.size() == 1) {
      // TODO: Handle other exprs.
      // else if (...) {
      //
      // struct unaryop_expr_node : expr_node {
      //   UnaryOp op;
      //   ptr<expr_node> rhs;
      //   virtual void accept(AstNode_Visitor &visitor) override;
      // };

      auto result = new unaryop_expr_node;
      result->line = ctx->getStart()->getLine();
      result->pos = ctx->getStart()->getCharPositionInLine();

      if (ctx->Plus()) {
        result->op = UnaryOp::PLUS;
      } else if (ctx->Minus()) {
        result->op = UnaryOp::MINUS;
      }

      result->rhs.reset(visit(ctx->exp(0)).as<expr_node *>());

      return dynamic_cast<expr_node *>(result);

    } else if (auto lval = ctx->lval()) {
      // lval
      return visit(lval);

    } else if (auto number = ctx->number()) {
      // number
      return visit(number);
    } else if (auto exp = ctx->exp(1)) {
      // exp (Equal | NonEqual | Less | Greater | LessEqual | GreaterEqual) exp
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
      }

      result->lhs.reset(visit(ctx->exp(0)).as<expr_node *>());
      result->rhs.reset(visit(ctx->exp(1)).as<expr_node *>());

      return dynamic_cast<expr_node *>(result);
    } else if (ctx->Assign()) {
      // lval Assign exp
      // struct assign_stmt_node : stmt_node {
      //   ptr<lval_node> target;
      //   ptr<expr_node> value;
      //   virtual void accept(AstNode_Visitor &visitor) override;
      // };
      auto result = new assign_stmt_node;
      result->line = ctx->getStart()->getLine();
      result->pos = ctx->getStart()->getCharPositionInLine();

      result->target.reset(visit(ctx->lval()).as<lval_node *>());
      result->value.reset(visit(ctx->exp(0)).as<expr_node *>());

      return dynamic_cast<expr_node *>(result);
    } else if (ctx->LeftParen()) {
      // (exp)
      return visit(ctx->exp(0));
    } else {
      assert(0 && "Unknown Exp.");
    }
  } catch (const std::bad_cast &e) {
    std::cerr << "Bad cast: " << e.what() << std::endl;
    throw; // Re-throw the exception after logging
  }
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
