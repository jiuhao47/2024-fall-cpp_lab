// Generated from SafeCParser.g4 by ANTLR 4.13.2
import org.antlr.v4.runtime.tree.ParseTreeListener;

/**
 * This interface defines a complete listener for a parse tree produced by
 * {@link SafeCParser}.
 */
public interface SafeCParserListener extends ParseTreeListener {
	/**
	 * Enter a parse tree produced by {@link SafeCParser#compUnit}.
	 * @param ctx the parse tree
	 */
	void enterCompUnit(SafeCParser.CompUnitContext ctx);
	/**
	 * Exit a parse tree produced by {@link SafeCParser#compUnit}.
	 * @param ctx the parse tree
	 */
	void exitCompUnit(SafeCParser.CompUnitContext ctx);
	/**
	 * Enter a parse tree produced by {@link SafeCParser#decl}.
	 * @param ctx the parse tree
	 */
	void enterDecl(SafeCParser.DeclContext ctx);
	/**
	 * Exit a parse tree produced by {@link SafeCParser#decl}.
	 * @param ctx the parse tree
	 */
	void exitDecl(SafeCParser.DeclContext ctx);
	/**
	 * Enter a parse tree produced by {@link SafeCParser#funcDef}.
	 * @param ctx the parse tree
	 */
	void enterFuncDef(SafeCParser.FuncDefContext ctx);
	/**
	 * Exit a parse tree produced by {@link SafeCParser#funcDef}.
	 * @param ctx the parse tree
	 */
	void exitFuncDef(SafeCParser.FuncDefContext ctx);
	/**
	 * Enter a parse tree produced by {@link SafeCParser#constDecl}.
	 * @param ctx the parse tree
	 */
	void enterConstDecl(SafeCParser.ConstDeclContext ctx);
	/**
	 * Exit a parse tree produced by {@link SafeCParser#constDecl}.
	 * @param ctx the parse tree
	 */
	void exitConstDecl(SafeCParser.ConstDeclContext ctx);
	/**
	 * Enter a parse tree produced by {@link SafeCParser#constDef}.
	 * @param ctx the parse tree
	 */
	void enterConstDef(SafeCParser.ConstDefContext ctx);
	/**
	 * Exit a parse tree produced by {@link SafeCParser#constDef}.
	 * @param ctx the parse tree
	 */
	void exitConstDef(SafeCParser.ConstDefContext ctx);
	/**
	 * Enter a parse tree produced by {@link SafeCParser#varDecl}.
	 * @param ctx the parse tree
	 */
	void enterVarDecl(SafeCParser.VarDeclContext ctx);
	/**
	 * Exit a parse tree produced by {@link SafeCParser#varDecl}.
	 * @param ctx the parse tree
	 */
	void exitVarDecl(SafeCParser.VarDeclContext ctx);
	/**
	 * Enter a parse tree produced by {@link SafeCParser#bType}.
	 * @param ctx the parse tree
	 */
	void enterBType(SafeCParser.BTypeContext ctx);
	/**
	 * Exit a parse tree produced by {@link SafeCParser#bType}.
	 * @param ctx the parse tree
	 */
	void exitBType(SafeCParser.BTypeContext ctx);
	/**
	 * Enter a parse tree produced by {@link SafeCParser#varDef}.
	 * @param ctx the parse tree
	 */
	void enterVarDef(SafeCParser.VarDefContext ctx);
	/**
	 * Exit a parse tree produced by {@link SafeCParser#varDef}.
	 * @param ctx the parse tree
	 */
	void exitVarDef(SafeCParser.VarDefContext ctx);
	/**
	 * Enter a parse tree produced by {@link SafeCParser#array}.
	 * @param ctx the parse tree
	 */
	void enterArray(SafeCParser.ArrayContext ctx);
	/**
	 * Exit a parse tree produced by {@link SafeCParser#array}.
	 * @param ctx the parse tree
	 */
	void exitArray(SafeCParser.ArrayContext ctx);
	/**
	 * Enter a parse tree produced by {@link SafeCParser#obcArray}.
	 * @param ctx the parse tree
	 */
	void enterObcArray(SafeCParser.ObcArrayContext ctx);
	/**
	 * Exit a parse tree produced by {@link SafeCParser#obcArray}.
	 * @param ctx the parse tree
	 */
	void exitObcArray(SafeCParser.ObcArrayContext ctx);
	/**
	 * Enter a parse tree produced by {@link SafeCParser#unobcArray}.
	 * @param ctx the parse tree
	 */
	void enterUnobcArray(SafeCParser.UnobcArrayContext ctx);
	/**
	 * Exit a parse tree produced by {@link SafeCParser#unobcArray}.
	 * @param ctx the parse tree
	 */
	void exitUnobcArray(SafeCParser.UnobcArrayContext ctx);
	/**
	 * Enter a parse tree produced by {@link SafeCParser#block}.
	 * @param ctx the parse tree
	 */
	void enterBlock(SafeCParser.BlockContext ctx);
	/**
	 * Exit a parse tree produced by {@link SafeCParser#block}.
	 * @param ctx the parse tree
	 */
	void exitBlock(SafeCParser.BlockContext ctx);
	/**
	 * Enter a parse tree produced by {@link SafeCParser#blockItem}.
	 * @param ctx the parse tree
	 */
	void enterBlockItem(SafeCParser.BlockItemContext ctx);
	/**
	 * Exit a parse tree produced by {@link SafeCParser#blockItem}.
	 * @param ctx the parse tree
	 */
	void exitBlockItem(SafeCParser.BlockItemContext ctx);
	/**
	 * Enter a parse tree produced by {@link SafeCParser#stmt}.
	 * @param ctx the parse tree
	 */
	void enterStmt(SafeCParser.StmtContext ctx);
	/**
	 * Exit a parse tree produced by {@link SafeCParser#stmt}.
	 * @param ctx the parse tree
	 */
	void exitStmt(SafeCParser.StmtContext ctx);
	/**
	 * Enter a parse tree produced by {@link SafeCParser#cond}.
	 * @param ctx the parse tree
	 */
	void enterCond(SafeCParser.CondContext ctx);
	/**
	 * Exit a parse tree produced by {@link SafeCParser#cond}.
	 * @param ctx the parse tree
	 */
	void exitCond(SafeCParser.CondContext ctx);
	/**
	 * Enter a parse tree produced by {@link SafeCParser#lval}.
	 * @param ctx the parse tree
	 */
	void enterLval(SafeCParser.LvalContext ctx);
	/**
	 * Exit a parse tree produced by {@link SafeCParser#lval}.
	 * @param ctx the parse tree
	 */
	void exitLval(SafeCParser.LvalContext ctx);
	/**
	 * Enter a parse tree produced by {@link SafeCParser#number}.
	 * @param ctx the parse tree
	 */
	void enterNumber(SafeCParser.NumberContext ctx);
	/**
	 * Exit a parse tree produced by {@link SafeCParser#number}.
	 * @param ctx the parse tree
	 */
	void exitNumber(SafeCParser.NumberContext ctx);
	/**
	 * Enter a parse tree produced by {@link SafeCParser#exp}.
	 * @param ctx the parse tree
	 */
	void enterExp(SafeCParser.ExpContext ctx);
	/**
	 * Exit a parse tree produced by {@link SafeCParser#exp}.
	 * @param ctx the parse tree
	 */
	void exitExp(SafeCParser.ExpContext ctx);
}