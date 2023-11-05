/* $Id: scope_check.c,v 1.16 2023/11/03 16:13:46 leavens Exp $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scope_check.h"
#include "id_attrs.h"
#include "file_location.h"
#include "ast.h"
#include "utilities.h"
#include "symtab.h"
#include "scope_check.h"

// Build the symbol table for prog
// and check for duplicate declarations
// or uses of undeclared identifiers
void scope_check_program(block_t prog)
{
    // printf("test");
    symtab_enter_scope();
    scope_check_constDecls(prog.const_decls);
    scope_check_varDecls(prog.var_decls);
    scope_check_procDecls(prog.proc_decls);
    scope_check_stmt(prog.stmt);
    symtab_leave_scope();
}

void scope_check_constDecls(const_decls_t cds)
{
    // printf("constDecls called, reading constant declarations \n");
    const_decl_t *cdp = cds.const_decls;
    while (cdp != NULL) {
	scope_check_constDecl(*cdp);
	cdp = cdp->next;
    }
}

// Add declarations for the names in vd,
// reporting duplicate declarations
void scope_check_constDecl(const_decl_t cd)
{
    // TODO: Check if we call a declaration here
    // printf("constDecl called, reading constant declaration \n");
    scope_check_constDefs(cd.const_defs);
}

void scope_check_constDefs(const_defs_t cdfs){
    // printf("constDefs called, reading constant definitions \n");
    const_def_t *cdf = cdfs.const_defs;
    while (cdf != NULL) {
	scope_check_constDef(*cdf);
	cdf = cdf->next;
    }
}

void scope_check_constDef(const_def_t cdf){
    // printf("constDef called, reading %s \n", cdf.ident.name);
    scope_check_declare_ident(cdf.ident, constant_idk);
}

// build the symbol table and check the declarations in vds
void scope_check_varDecls(var_decls_t vds)
{
    var_decl_t *vdp = vds.var_decls;
    while (vdp != NULL) {
	scope_check_varDecl(*vdp);
	vdp = vdp->next;
    }
}

// Add declarations for the names in vd,
// reporting duplicate declarations
void scope_check_varDecl(var_decl_t vd)
{
    scope_check_idents(vd.idents, variable_idk);
}

void scope_check_procDecls(proc_decls_t pds)
{
    proc_decl_t *pdp = pds.proc_decls;
    while (pdp != NULL) {
	scope_check_procDecl(*pdp);
	pdp = pdp->next;
    }
}

void scope_check_procDecl(proc_decl_t pd)
{
    // TODO: Figure out if we need to declare the name
    scope_check_declare_proc(pd, procedure_idk);

    // TODO: Figure 
	scope_check_program(*pd.block);
}


// Add declarations for the names in ids
// to current scope as type vt
// reporting any duplicate declarations
void scope_check_idents(idents_t ids,
			id_kind vt)
{
    ident_t *idp = ids.idents;
    while (idp != NULL) {
    // printf("I was called");
	scope_check_declare_ident(*idp, vt);
	idp = idp->next;
    }
}

// Add declaration for id
// to current scope as type vt
// reporting if it's a duplicate declaration
void scope_check_declare_ident(ident_t id,
			    id_kind vt)
{
    if (symtab_declared_in_current_scope(id.name)) {
        // only variables in FLOAT
        
	bail_with_prog_error(*(id.file_loc),
			     "variable \"%s\" is already declared as a variable",
			     id.name);
    } else {
	int ofst_cnt = symtab_scope_loc_count();
	id_attrs *attrs = create_id_attrs(*(id.file_loc),
					      vt, ofst_cnt);
	symtab_insert(id.name, attrs);
    }
}

void scope_check_declare_proc(proc_decl_t id,
			    id_kind vt)
{
    if (symtab_declared_in_current_scope(id.name)) {
        // only variables in FLOAT
	bail_with_prog_error(*(id.file_loc),
			     "variable \"%s\" has already been declared!",
			     id.name);
    } else {
	int ofst_cnt = symtab_scope_loc_count();
	id_attrs *attrs = create_id_attrs(*(id.file_loc),
					      vt, ofst_cnt);
	symtab_insert(id.name, attrs);
    }
}


// check the statement to make sure that
// all idenfifiers used have been declared
// (if not, then produce an error)
void scope_check_stmt(stmt_t stmt)
{
    switch (stmt.stmt_kind) {
    case assign_stmt:
	scope_check_assignStmt(stmt.data.assign_stmt);
	break;
    case call_stmt:
	scope_check_callStmt(stmt.data.call_stmt);
	break;
    case begin_stmt:
	scope_check_beginStmt(stmt.data.begin_stmt);
	break;
    case if_stmt:
	scope_check_ifStmt(stmt.data.if_stmt);
	break;
    case while_stmt:
	scope_check_whileStmt(stmt.data.while_stmt);
	break;
    case read_stmt:
	scope_check_readStmt(stmt.data.read_stmt);
	break;
    case write_stmt:
	scope_check_writeStmt(stmt.data.write_stmt);
	break;
    case skip_stmt:
	// No further unpacking needed
	break;
    default:
	bail_with_error("Call to scope_check_stmt with an AST that is not a statement!");
	break;
    }
}

// check the statement for
// undeclared identifiers
void scope_check_assignStmt(
                       assign_stmt_t stmt)
{
    // printf("assignStmt");
    const char *name = stmt.name;
    id_use *idu = scope_check_ident_declared(*(stmt.file_loc),
					     name);
    assert(idu != NULL);  // since would bail if not declared
    scope_check_expr(*(stmt.expr));
}

void scope_check_callStmt(call_stmt_t stmt){
    // printf("callStmt");
    scope_check_ident_declared(*(stmt.file_loc), stmt.name);
}

// check the statement for
// duplicate declarations and for
// undeclared identifiers
void scope_check_beginStmt(begin_stmt_t stmt)
{
    // symtab_enter_scope();
    scope_check_stmts(stmt.stmts);
    // symtab_leave_scope();
}

// check the statements to make sure that
// all idenfifiers referenced in them have been declared
// (if not, then produce an error)
void scope_check_stmts(stmts_t stmts)
{
    stmt_t *sp = stmts.stmts;
    while (sp != NULL) {
	scope_check_stmt(*sp);
	sp = sp->next;
    }
}

// check the statement to make sure that
// all idenfifiers referenced in it have been declared
// (if not, then produce an error)
void scope_check_ifStmt(if_stmt_t stmt)
{
    scope_check_condition(stmt.condition);
    scope_check_stmt(*(stmt.then_stmt));
    scope_check_stmt(*(stmt.else_stmt));
}

void scope_check_condition(condition_t cond)
{
    switch (cond.cond_kind) {
    case ck_odd:
	scope_check_odd_cond(cond.data.odd_cond);
	break;
    case ck_rel:
	scope_check_rel_op_cond(cond.data.rel_op_cond);
	break;
    default:
	bail_with_error("Unexpected expr_kind_e (%d) in scope_check_expr",
			cond.cond_kind);
	break;
    }
}

void scope_check_odd_cond(odd_condition_t cond){
    scope_check_expr(cond.expr);
}

void scope_check_rel_op_cond(rel_op_condition_t cond){
    scope_check_expr(cond.expr1);
    scope_check_expr(cond.expr2);
}

void scope_check_whileStmt(while_stmt_t stmt){
    scope_check_condition(stmt.condition);
    scope_check_stmt(*(stmt.body));
}

// check the statement to make sure that
// all idenfifiers referenced in it have been declared
// (if not, then produce an error)
void scope_check_readStmt(read_stmt_t stmt)
{
    // printf("readStmt");
    scope_check_ident_declared(*(stmt.file_loc), stmt.name);
}

// check the statement to make sure that
// all idenfifiers referenced in it have been declared
// (if not, then produce an error)
void scope_check_writeStmt(write_stmt_t stmt)
{
    scope_check_expr(stmt.expr);
}

// check the expresion to make sure that
// all idenfifiers used have been declared
// (if not, then produce an error)
void scope_check_expr(expr_t exp)
{
    switch (exp.expr_kind) {
    case expr_bin:
	scope_check_binary_op_expr(exp.data.binary);
	break;
    case expr_ident:
	scope_check_ident_expr(exp.data.ident);
	break;
    case expr_number:
	// no identifiers are possible in this case, so just return
	break;
    default:
	bail_with_error("Unexpected expr_kind_e (%d) in scope_check_expr",
			exp.expr_kind);
	break;
    }
}

// check that all identifiers used in exp
// have been declared
// (if not, then produce an error)
void scope_check_binary_op_expr(binary_op_expr_t exp)
{
    scope_check_expr(*(exp.expr1));
    // (note: no identifiers can occur in the operator)
    scope_check_expr(*(exp.expr2));
}

// check the identifier (id) to make sure that
// all it has been declared (if not, then produce an error)
void scope_check_ident_expr(ident_t id)
{
    scope_check_ident_declared(*(id.file_loc), id.name);
}

// check that name has been declared,
// if so, then return an id_use for it
// otherwise, produce an error 
id_use *scope_check_ident_declared(
         file_location floc,
         const char *name)
{
    id_use *ret = symtab_lookup(name);
    if (ret == NULL) {
	bail_with_prog_error(floc,
				"identifer \"%s\" is not declared!",
				name);
    }
    return ret;
}

/*

void scope_check_program(block_t prog)
{
   scope_check_block(prog);
}

void scope_check_block(block_t blk)
{  
   symtab_enter_scope();
   scope_check_constDecls(blk.const_decls);
   scope_check_varDecls( blk.var_decls);
   scope_check_procDecls(blk.proc_decls);
   scope_check_stmt( blk.stmt);
   symtab_leave_scope();

}

void scope_check_constDecls(const_decls_t cds)
{
   const_decl_t *cd_listp = cds.const_decls;
   while (cd_listp != NULL)
   {
      scope_check_constDecl(*cd_listp);
      cd_listp = cd_listp->next;
   }
}

void scope_check_constDecl(const_decl_t cd)
{
   scope_check_constDefs(cd.const_defs);
}

void scope_check_constDefs(const_defs_t cdfs)
{
   const_def_t *cdp = cdfs.const_defs;
   while (cdp != NULL)
   {
      scope_check_constDef(*cdp);
      cdp = cdp->next;
   }

}
//TODO:  
void scope_check_constDef(const_defs_t cdf)
{

}

void scope_check_varDecls(var_decls_t vds)
{
   var_decl_t *vdp = vds.var_decls;
   while (vdp != NULL)
   {
      scope_check_varDecl(*vdp);
      vdp = vdp->next;
   }
}

void scope_check_varDecl(var_decl_t vd)
{
   scope_check_idents( vd.idents);
}

//TODO: 
void scope_check_idents(idents_t idents)
{
   ident_t *ip = idents.idents;
   while (ip != NULL)
   {

   }
}

void scope_check_procDecls(proc_decls_t pds)
{
   proc_decl_t *pdp = pds.proc_decls;
   while (pdp != NULL)
   {
      scope_check_procDecl(*pdp);
      pdp = pdp ->next;
   }

}

void scope_check_procDecl(proc_decl_t pd)
{
   scope_check_block(*(pd.block));

}

void scope_check_stmt(stmt_t stmt)
{
   switch (stmt.stmt_kind) 
   {
    case assign_stmt:
      scope_check_assignStmt(stmt.data.assign_stmt);
      break;
    case call_stmt:
      scope_check_callStmt(stmt.data.call_stmt);
      break;
    case begin_stmt:
      scope_check_beginStmt(stmt.data.begin_stmt);
      break;
    case if_stmt:
      scope_check_ifStmt(stmt.data.if_stmt);
      break;
    case while_stmt:
      scope_check_whileStmt(stmt.data.while_stmt);
      break;
    case read_stmt:
      scope_check_readStmt(stmt.data.read_stmt);
      break;
    case write_stmt:
      scope_check_writeStmt(stmt.data.write_stmt);
      break;
    case skip_stmt:
      scope_check_skipStmt();
      break;
    default:
      bail_with_error("Unknown stmt_kind (%d) in scope_check_stmt!",
			stmt.stmt_kind);
	   break;

   }
}

void scope_check_assignStmt( assign_stmt_t stmt)
{
   if (stmt.expr == NULL) 
   {
	bail_with_error("Found null expression in assignment statment!");
   }

   scope_check_expr(*(stmt.expr));
   
}

//TODO:
void scope_check_callStmt(call_stmt_t stmt)
{
}

void scope_check_beginStmt(begin_stmt_t stmt)
{
   scope_check_stmts(stmt.stmts);
}
void scope_check_stmts( stmts_t stmts)
{
   stmt_t * sp = stmts.stmts;
   while (sp != NULL)
   {
      scope_check_stmt(*sp);
      sp = sp->next;
   }
}
void scope_check_ifStmt(if_stmt_t stmt)
{
   scope_check_condition(stmt.condition);
   scope_check_stmt(*(stmt.then_stmt));
   scope_check_stmt(*(stmt.else_stmt));
}
void scope_check_whileStmt(while_stmt_t stmt)
{
   scope_check_condition(stmt.condition);
   scope_check_stmt(*(stmt.body));
}
//TODO:
void scope_check_readStmt(read_stmt_t stmt)
{

}
void scope_check_writeStmt(write_stmt_t stmt)
{
   scope_check_expr(stmt.expr);
}
//TODO:
void scope_check_skipStmt()
{
}

void scope_check_condition(condition_t cond)
{
   switch (cond.cond_kind) 
   {
      case ck_odd:
         scope_check_oddCond(cond.data.odd_cond);
         break;
      case ck_rel:
         scope_check_relOpCond(cond.data.rel_op_cond);
         break;
      default:
	bail_with_error("Unexpected condition_kind_e (%d) in unparseCondition!",
			cond.cond_kind);
	break;
      
   
   }
}

void scope_check_oddCond(odd_condition_t cond)
{
   scope_check_expr(cond.expr);
}

void scope_check_relOpCond(rel_op_condition_t cond)
{
   scope_check_expr(cond.expr1);
   scope_check_token(cond.rel_op);
   scope_check_expr(cond.expr2);

}

//TODO:
void scope_check_token()
{

}

void scope_check_expr(expr_t exp)
{
   switch (exp.expr_kind) 
   {
      case expr_bin:
         scope_check_binOpExpr(exp.data.binary);
         break;
      case expr_ident:
         scope_check_ident(exp.data.ident);
         break;
      case expr_number:
         scope_check_number(exp.data.number);
	      break;
      default:
	bail_with_error("Unexpected expr_kind_e (%d) in scope_check_expr!",
			exp.expr_kind);
	      break;

   }

}

void scope_check_binOpExpr(binary_op_expr_t exp)
{
   scope_check_expr(*(exp.expr1));
   scope_check_token(exp.arith_op);
    scope_check_expr(*(exp.expr2));
}
//TODO:
void scope_check_ident(ident_t id)
{

}

//TODO:
void scope_check_number(number_t num)
{

}

*/