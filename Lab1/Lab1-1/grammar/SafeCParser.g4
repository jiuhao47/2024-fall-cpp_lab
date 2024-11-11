parser grammar SafeCParser;
options { tokenVocab = SafeCLexer; }

compUnit: (decl | funcDef)+ EOF;
// 程序单元：包含一个或多个声明或函数定义，以 EOF 结束

decl: constDecl | varDecl;
// 声明：可以是常量声明或变量声明

funcDef: 'void' Identifier '(' ')' block;
// 函数定义：格式为 void functionName() { ... }

constDecl: 'const' bType constDef (',' constDef)* ';';
// 常量声明：格式为 const int a = 1, b = 2;

constDef: (Identifier | array) '=' (expr | '{' expr (',' expr)* '}');
// 常量定义：可以是标量或数组，且必须赋值

varDecl: bType varDef (',' varDef)* ';';
// 变量声明：格式为 int a = 1, b = 2;

varDef: (Identifier | array) ('=' (expr | '{' expr (',' expr)* '}'))?;
// 变量定义：支持数组和可选初始化

bType: 'int';
// 基本类型定义，目前仅有 int

array: obcArray | unobcArray;
obcArray: 'obc' unobcArray;
unobcArray: Identifier '[' (expr)? ']';
// 数组声明：可以是带边界检查的数组或普通数组

block: '{' blockItem* '}';
// 语句块：包含一个或多个语句

blockItem: decl | stmt;
// 语句块元素：可以是声明或语句

stmt: 
      lval '=' expr ';'             // 赋值语句
    | 'if' '(' cond ')' stmt ('else' stmt)?  // 条件语句
    | 'while' '(' cond ')' stmt    // 循环语句
    | block                        // 语句块
    | ';'                          // 空语句
    ;
// 语句：包括赋值、条件、循环、表达式等

cond: expr (('==' | '!=' | '<' | '>' | '<=' | '>=') expr)?;
// 条件表达式：支持比较运算

lval: Identifier | Identifier '[' expr ']';
// 左值表达式：可以是变量或数组元素

expr: 
      expr ('+' | '-') expr   // 加减运算
    | expr ('*' | '/' | '%') expr   // 乘除模运算
    | '(' expr ')'              // 括号表达式
    | lval                      // 左值
    | IntConst                  // 整数常量
    ;
// 数字或基本表达式