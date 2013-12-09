/*
 * =====================================================================================
 *
 *       Filename:  env.c
 *
 *    Description:  define the environment of type checking
 *
 *        Version:  1.0
 *        Created:  12/02/13 21:01:32
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  wangxinalex (), wangxinalex@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>

#include "env.h"
#include "util.h"
#include "types.h"

E_enventry E_VarEntry(Ty_ty ty){
	E_enventry varEntry = checked_malloc(sizeof(*varEntry));
	varEntry->kind = E_varEntry;
	varEntry->u.var.ty = ty;
	return varEntry;
}


E_enventry E_FunEntry(Ty_tyList formals, Ty_ty result){
	
	E_enventry funEntry = checked_malloc(sizeof(*funEntry));
	funEntry -> kind = E_funEntry;
	funEntry -> u.fun.formals = formals;
	funEntry -> u.fun.result = result;
	return funEntry;

}

S_table E_base_tenv(void){
	S_table tenv = S_empty();
	S_enter(tenv, S_Symbol("int"), Ty_Int());
	S_enter(tenv, S_Symbol("string"), Ty_String());
	return tenv;

}

S_table E_base_venv(void){
	S_table venv = S_empty();
	/*some predefiend functions*/
	E_enventry env_print = E_FunEntry(Ty_TyList(Ty_String(), NULL), Ty_Void()); 
	E_enventry env_flush = E_FunEntry(NULL, Ty_Void()); 
	E_enventry env_getchar = E_FunEntry(Ty_TyList(Ty_String(), NULL), Ty_String()); 
	E_enventry env_ord = E_FunEntry(Ty_TyList(Ty_String(), NULL), Ty_Int()); 
	E_enventry env_chr = E_FunEntry(Ty_TyList(Ty_Int(), NULL), Ty_String()); 
	E_enventry env_size = E_FunEntry(Ty_TyList(Ty_String(), NULL), Ty_Int()); 
	E_enventry env_substring = E_FunEntry(Ty_TyList(Ty_String(), 
				Ty_TyList(Ty_Int(), 
				Ty_TyList(Ty_Int(), NULL))), Ty_String()); 
	E_enventry env_concat = E_FunEntry(Ty_TyList(Ty_String(), Ty_TyList(Ty_String(),NULL)), Ty_String()); 
	E_enventry env_not = E_FunEntry(Ty_TyList(Ty_Int(), NULL), Ty_Int()); 
	E_enventry env_exit = E_FunEntry(Ty_TyList(Ty_Int(), NULL), Ty_Void()); 

	S_enter(venv, S_Symbol("print"), env_print);
	S_enter(venv, S_Symbol("flush"), env_flush);
	S_enter(venv, S_Symbol("getchar"), env_getchar);
	S_enter(venv, S_Symbol("ord"), env_ord);
	S_enter(venv, S_Symbol("chr"), env_chr);
	S_enter(venv, S_Symbol("size"), env_size);
	S_enter(venv, S_Symbol("substring"), env_substring);
	S_enter(venv, S_Symbol("concat"), env_concat);
	S_enter(venv, S_Symbol("not"), env_not);
	S_enter(venv, S_Symbol("exit"), env_exit);

}
