parser grammar SafeCParser;
options { tokenVocab = SafeCLexer; }

compUnit: (decl | funcDef) + EOF;

decl: constDecl | varDecl;

funcDef: (bType | Void) Identifier LeftParen RightParen block;

constDecl: Const bType constDef (Comma constDef)* SemiColon;

constDef: Identifier Assign number;

varDecl: bType varDef (Comma varDef)* SemiColon;

bType: Int;

varDef: Identifier (Assign exp)?;

array: obcArray | unobcArray;

obcArray: Obc unobcArray;

unobcArray: LeftBracket IntConst RightBracket;

block: LeftBrace (blockItem)* RightBrace;

blockItem: decl | stmt;

stmt: 
    block
    | SemiColon
    | exp SemiColon
    | If LeftParen cond RightParen stmt (Else stmt)?
    | While LeftParen cond RightParen stmt;

cond: exp;

lval: Identifier;

number: IntConst;

exp: lval Assign exp
    | lval
    | number
    | LeftParen exp RightParen
    | exp (Plus | Minus | Multiply | Divide | Modulo) exp
    | Minus exp
    | exp (Equal | NonEqual | Less | Greater | LessEqual | GreaterEqual) exp;
