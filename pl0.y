 /* $Id: bison_pl0_y_top.y,v 1.1 2023/10/19 18:47:38 leavens Exp $ */
 /* This file should be named pl0.y, it won't work with other file names! */

%code top {
#include <stdio.h>
}

%code requires {

 /* Including "ast.h" must be at the top, to define the AST type */
#include "ast.h"
#include "machine_types.h"
#include "parser_types.h"
#include "lexer.h"

    /* Report an error to the user on stderr */
extern void yyerror(const char *filename, const char *msg);

}    /* end of %code requires */

%verbose
%define parse.lac full
%define parse.error detailed

 /* the following passes file_name to yyerror,
    and declares it as an formal parameter of yyparse. */
%parse-param { char const *file_name }

%token <ident> identsym
%token <number> numbersym
%token <token> plussym    "+"
%token <token> minussym   "-"
%token <token> multsym    "*"
%token <token> divsym     "/"

%token <token> periodsym  "."
%token <token> semisym    ";"
%token <token> eqsym      "="
%token <token> commasym   ","
%token <token> becomessym ":="

%token <token> constsym   "const"
%token <token> varsym     "var"
%token <token> proceduresym "procedure"
%token <token> callsym    "call"
%token <token> beginsym   "begin"
%token <token> endsym     "end"
%token <token> ifsym      "if"
%token <token> thensym    "then"
%token <token> elsesym    "else"
%token <token> whilesym   "while"
%token <token> dosym      "do"
%token <token> readsym    "read"
%token <token> writesym   "write"
%token <token> skipsym    "skip"
%token <token> oddsym     "odd"

%token <token> neqsym     "<>"
%token <token> ltsym      "<"
%token <token> leqsym     "<="
%token <token> gtsym      ">"
%token <token> geqsym     ">="
%token <token> lparensym  "("
%token <token> rparensym  ")"

%type <block> program
%type <block> block
%type <const_decls> constDecls

%type <var_decls> varDecls
%type <var_decl> varDecl
%type <idents> idents
%type <proc_decls> procDecls
%type <empty> empty
%type <const_decl> constDecl
%type <const_def> constDef
%type <const_defs> constDefs

%type <proc_decl> procDecl
%type <stmt> stmt
%type <assign_stmt> assignStmt
%type <call_stmt> callStmt
%type <begin_stmt> beginStmt
%type <if_stmt> ifStmt
%type <while_stmt> whileStmt
%type <read_stmt> readStmt
%type <write_stmt> writeStmt
%type <skip_stmt> skipStmt
%type <stmts> stmts
%type <condition> condition
%type <odd_condition> oddCondition
%type <rel_op_condition> relOpCondition
%type <expr> expr

%type <token> relOp
%type <expr> term
%type <expr> factor
%type <token> posSign

%start program

%code {
 /* extern declarations provided by the lexer */
extern int yylex(void);

 /* The AST for the program, set by the semantic action 
    for the nonterminal program. */
block_t progast; 

 /* Set the program's ast to be t */
extern void setProgAST(block_t t);
}

%%
    // Nawfal
    program : block { setProgAST($1); };
    block: constDecls varDecls procDecls stmt { $$ = ast_block($1, $2, $3, $4); } ;     
    constDecls ::= constDecl { $$ }
    const-decl ::= const ⟨const-defs⟩ ;
    ⟨const-defs⟩ ::= ⟨const-def⟩ | ⟨const-defs⟩ , ⟨const-def⟩
    ⟨const-def⟩ ::= ⟨ident⟩ = ⟨number⟩
    ⟨var-decls⟩ ::= {⟨var-decl⟩}
    ⟨var-decl⟩ ::= var ⟨idents⟩ ;
    ⟨idents⟩ ::= ⟨ident⟩ | ⟨idents⟩ , ⟨ident⟩
    ⟨proc-decls⟩ ::= {⟨proc-decl⟩}

    // Yamil
    ⟨proc-decl⟩ ::= procedure ⟨ident⟩ ; ⟨block⟩ ; procedure ident {$$ = ast_proc_decl($1, $2)}
    ⟨stmt⟩ ::= ⟨assign-stmt⟩ | ⟨call-stmt⟩ | ⟨begin-stmt⟩ | ⟨if-stmt⟩
    | ⟨while-stmt⟩ | ⟨read-stmt⟩ | ⟨write-stmt⟩ | ⟨skip-stmt⟩
    ⟨assign-stmt⟩ ::= ⟨ident⟩ := ⟨expr⟩
    ⟨call-stmt⟩ ::= call ⟨ident⟩
    ⟨begin-stmt⟩ ::= begin ⟨stmts⟩ end
    ⟨if-stmt⟩ ::= if ⟨condition⟩ then ⟨stmt⟩ else ⟨stmt⟩
    ⟨while-stmt⟩ ::= while ⟨condition⟩ do ⟨stmt⟩
    ⟨read-stmt⟩ ::= read ⟨ident⟩
    ⟨write-stmt⟩ ::= write ⟨expr⟩

    // Robert
    ⟨skip-stmt⟩ ::= skip
    ⟨stmts⟩ ::= ⟨stmt⟩ | ⟨stmts⟩ ; ⟨stmt⟩
    ⟨condition⟩ ::= ⟨odd-condition⟩ | ⟨rel-op-condition⟩
    ⟨odd-condition⟩ ::= odd ⟨expr⟩
    ⟨rel-op-condition⟩ ::= ⟨expr⟩ ⟨rel-op⟩ ⟨expr⟩
    ⟨rel-op⟩ ::= = | <> | < | <= | > | >=
    ⟨expr⟩ ::= ⟨term⟩ | ⟨expr⟩ ⟨plus⟩ ⟨term⟩ | ⟨expr⟩ ⟨minus⟩ ⟨term⟩
    ⟨term⟩ ::= ⟨factor⟩ | ⟨term⟩ ⟨mult⟩ ⟨factor⟩ | ⟨term⟩ ⟨div⟩ ⟨factor⟩
    ⟨factor⟩ ::= ⟨ident⟩ | ⟨minus⟩ ⟨number⟩ | ⟨pos-sign⟩ ⟨number⟩ | ( ⟨expr⟩ )
    ⟨pos-sign⟩ ::= ⟨plus⟩ | ⟨empty⟩
    ⟨empty⟩ ::=
%%

// Set the program's ast to be ast
void setProgAST(block_t ast) { progast = ast; }
