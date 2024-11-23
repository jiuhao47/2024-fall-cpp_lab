// DONE: 0
// DONE: 1
// DONE: 2
// DONE: 3
// DONE: 4
// DONE: 5
// DONE: 6
// DONE: 7
// DONE: 8
// DONE: 9
// DONE: 10
// DONE: 11
// DONE: 12
// DONE: 13
// DONE: 14
// DONE: 15
// DONE: 16
// DONE: 17
// DONE: 18
// DONE: 19

parser grammar SafeCParser;
options { tokenVocab = SafeCLexer; }

compUnit: (decl | funcDef) + EOF;
// 程序单元

decl: constDecl | varDecl;
// 声明（常量或变量）

funcDef: Void Identifier LeftParen RightParen block;
// 函数声明 void func() {}

constDecl: Const bType constDef (Comma constDef)* SemiColon;
// 常量声明格式（const int a = 1, b = 2;）

constDef: (Identifier | array) Assign (exp | (LeftBrace (exp (Comma exp)*)? RightBrace)); 
// 变量声明（需赋值）（可为数组）

varDecl: bType varDef (Comma varDef)* SemiColon;
// 变量声明格式（int a = 1, b = 2;）

bType: Int;
// 基本类型

varDef: (Identifier | array) (Assign (exp | (LeftBrace (exp (Comma exp)*)? RightBrace)))?;
// 变量声明（可赋值）（可为数组）

array: obcArray | unobcArray;
// 数组声明（两种形式）

obcArray: Obc unobcArray;
// 数据声明（int obc a[]）

unobcArray: Identifier LeftBracket (exp)? RightBracket;
// 数组声明（可为a[]形式）

block: LeftBrace (blockItem)* RightBrace;
// 语句块格式

blockItem: decl | stmt;
// 语句块单元

stmt: 
    block
    | lval Assign exp SemiColon
    | SemiColon
    | exp SemiColon
    | If LeftParen cond RightParen stmt (Else stmt)?
    | While LeftParen cond RightParen stmt;
// 语句

cond: exp;
// 条件

lval: Identifier | (Identifier LeftBracket exp RightBracket) | (Identifier LeftParen RightParen );
// 左值

number: IntConst;
// 数字

exp:
    lval
    | number
    | LeftParen exp RightParen
    | exp (Plus | Minus | Multiply | Divide | Modulo) exp
    | (Minus | Plus) exp
    | exp (Equal | NonEqual | Less | Greater | LessEqual | GreaterEqual) exp;
// 表达式
