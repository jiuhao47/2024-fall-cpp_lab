#include "AstBuilder.h"

namespace antlrcpp {
antlrcpp::Any AstBuilder::visitCompUnit(SafeCParser::CompUnitContext *ctx) {
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
      auto func_def_n = visit(func_def).as<func_def_node *>();
      result->comp_units.push_back(ptr<comp_unit_child_node>(func_def_n));

    } else {
      // Error.
      log(compile_units[i]->getText());
      assert(0 && "Unknown Compile Unit.");
    }
  }
  return result;
}

antlrcpp::Any AstBuilder::visitDecl(SafeCParser::DeclContext *ctx) {

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

  auto result = new func_def_node;
  result->line = ctx->getStart()->getLine();
  result->pos = ctx->getStart()->getCharPositionInLine();

  result->name = ctx->Identifier()->getText();
  result->body.reset(visit(ctx->block()).as<block_node *>());

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

  auto result = new var_def_stmt_node;
  result->line = ctx->getStart()->getLine();
  result->pos = ctx->getStart()->getCharPositionInLine();

  for (auto const_def : ctx->constDef()) {
    auto const_def_n = visit(const_def).as<var_def_node *>();
    result->var_defs.push_back(ptr<var_def_node>(const_def_n));
  }

  return result;
}

antlrcpp::Any AstBuilder::visitConstDef(SafeCParser::ConstDefContext *ctx) {

  // struct var_def_node : stmt_node {
  //     bool is_const;
  //     BType btype;
  //     std::string name;
  //     bool is_obc;
  //     ptr<expr_node> array_length;
  //     ptr_vector<expr_node> initializers;
  //     virtual void accept(AstNode_Visitor& visitor) override;
  // };

  if (auto array = ctx->array()) {
    // TODO: Array

  } else if (ctx->Identifier()) {
    // TODO: Scalar

  } else {
    assert(0 && "Unknown ConstDef");
  }
}

antlrcpp::Any AstBuilder::visitVarDecl(SafeCParser::VarDeclContext *ctx) {
  // TODO: varDecl

  // varDecl: bType varDef (Comma varDef)* SemiColon;
}

antlrcpp::Any AstBuilder::visitBType(SafeCParser::BTypeContext *ctx) {
  /* Nothing to do here. */
  return NULL;
}

antlrcpp::Any AstBuilder::visitVarDef(SafeCParser::VarDefContext *ctx) {
  if (auto array = ctx->array()) {
    // TODO: Array

  } else if (ctx->Identifier()) {
    // TODO: Scalar

  } else {
    assert(0 && "Unknown VarDef");
  }
}

antlrcpp::Any AstBuilder::visitArray(SafeCParser::ArrayContext *ctx) {
  // TODO: Array
}

antlrcpp::Any AstBuilder::visitObcArray(SafeCParser::ObcArrayContext *ctx) {
  // TODO: ObcArray
}

antlrcpp::Any AstBuilder::visitUnobcArray(SafeCParser::UnobcArrayContext *ctx) {
  // TODO: UnobcArray
}

antlrcpp::Any AstBuilder::visitBlock(SafeCParser::BlockContext *ctx) {
  auto result = new block_node;

  result->line = ctx->getStart()->getLine();
  result->pos = ctx->getStart()->getCharPositionInLine();

  for (auto block_item : ctx->blockItem()) {
    auto block_item_n = visit(block_item).as<stmt_node *>();
    result->body.push_back(ptr<stmt_node>(block_item_n));
  }
  return result;
}

antlrcpp::Any AstBuilder::visitBlockItem(SafeCParser::BlockItemContext *ctx) {
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
  // TODO: Stmt

  // stmt:
  //     block
  //     | SemiColon
  //     | exp SemiColon
  //     | If LeftParen cond RightParen stmt (Else stmt)?
  //     | While LeftParen cond RightParen stmt;

  if (auto block = ctx->block()) {
    return visit(block);

  } else if (ctx->SemiColon()) {
    return visit(ctx->SemiColon());

  } else if (auto exp = ctx->exp()) {
    return visit(exp);

  } else if (auto if_stmt = ctx->If()) {
    return visit(if_stmt);

  } else if (auto while_stmt = ctx->While()) {
    return visit(while_stmt);

  } else {
    assert(0 && "Unknown Stmt.");
  }
}

antlrcpp::Any AstBuilder::visitCond(SafeCParser::CondContext *ctx) {
  // TODO: Cond
  //
  // cond: exp;

  return visit(ctx->exp());
}

antlrcpp::Any AstBuilder::visitLval(SafeCParser::LvalContext *ctx) {
  // TODO: Lval

  // lval: Identifier | (Identifier LeftBracket exp RightBracket) | (Identifier
  // LeftParen RightParen );
}

antlrcpp::Any AstBuilder::visitNumber(SafeCParser::NumberContext *ctx) {
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
  return result;
}

antlrcpp::Any AstBuilder::visitExp(SafeCParser::ExpContext *ctx) {
  auto exps = ctx->exp();

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

    // TODO: Handle other exprs.
    // else if (...) {

  } else {
    assert(0 && "Unkown exp.");
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
