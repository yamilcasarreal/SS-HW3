/* $Id: scope_check.h,v 1.5 2023/11/01 13:20:24 leavens Exp $ */
#ifndef _SCOPE_CHECK_H
#define _SCOPE_CHECK_H
#include "ast.h"
#include "id_use.h"

// Build the symbol table for the given program AST
// and check the given program AST for duplicate declarations
// or uses of identifiers that were not declared
extern void scope_check_program(block_t prog);

extern void scope_check_constDecls(const_decls_t cds);

extern void scope_check_constDecl(const_decl_t cd);

// build the symbol table and check the declarations in vds
extern void scope_check_varDecls(var_decls_t vds);

// Add declarations to the current scope's symbol table for vd,
// producing errors for duplicate declarations
extern void scope_check_varDecl(var_decl_t vd);

extern void scope_check_procDecls(proc_decls_t pds);

extern void scope_check_procDecl(proc_decl_t *pd);

// Add declarations for the names in ids to the current scope's symbol table,
// for variables of the type vt,
// producing errors for any duplicate declarations
extern void scope_check_idents(idents_t ids, id_kind vt);

// Add a declaration of the name id.name with the type vt
// to the current scope's symbol table,
// producing an error if this would be a duplicate declaration
extern void scope_check_declare_ident(ident_t id, id_kind vt);

// check the statement to make sure that
// all idenfifiers referenced in it have been declared
// (if not, then produce an error)
extern void scope_check_stmt(stmt_t stmt);

// check the statement to make sure that
// all idenfifiers referenced in it have been declared
// (if not, then produce an error)
extern void scope_check_assignStmt(assign_stmt_t stmt);

extern void scope_check_callStmt(call_stmt_t stmt);

// check the statement to make sure that
// there are no duplicate declarations and that
// all idenfifiers referenced in it have been declared
// (if not, then produce an error)
extern void scope_check_beginStmt(begin_stmt_t stmt);

// check the statements to make sure that
// all idenfifiers referenced in them have been declared
// (if not, then produce an error)
extern void scope_check_stmts(stmts_t stmts);

// check the statement to make sure that
// all idenfifiers referenced in it have been declared
// (if not, then produce an error)
extern void scope_check_ifStmt(if_stmt_t stmt);

extern void scope_check_condition(condition_t cond);

extern void scope_check_odd_cond(odd_condition_t cond);

extern void scope_check_rel_op_cond(rel_op_condition_t cond);

extern void scope_check_whileStmt(while_stmt_t stmt);

// check the statement to make sure that
// all idenfifiers referenced in it have been declared
// (if not, then produce an error)
extern void scope_check_readStmt(read_stmt_t stmt);

// check the statement to make sure that
// all idenfifiers referenced in it have been declared
// (if not, then produce an error)
extern void scope_check_writeStmt(write_stmt_t stmt);

// check the expresion to make sure that
// all idenfifiers referenced in it have been declared
// (if not, then produce an error)
extern void scope_check_expr(expr_t exp);

// check the expression (exp) to make sure that
// all idenfifiers referenced in it have been declared
// (if not, then produce an error)
extern void scope_check_binary_op_expr(binary_op_expr_t exp);

// check the identifier (id) to make sure that
// all it has been declared (if not, then produce an error)
extern void scope_check_ident_expr(ident_t id);

// check that the given name has been declared,
// if so, then return an id_use (containing the attributes) for that name,
// otherwise, produce an error using the file_location (floc) given.
extern id_use *scope_check_ident_declared(file_location floc, const char *name);

#endif
