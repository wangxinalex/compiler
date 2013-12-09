/*
 * =====================================================================================
 *
 *       Filename:  env.h
 *
 *    Description:  head file of env.c
 *
 *        Version:  1.0
 *        Created:  12/02/13 20:41:56
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  wangxinalex (), wangxinalex@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */


#ifndef  ENV_H_INC
#define  ENV_H_INC

#include "types.h"
#include "symbol.h"
struct E_enventry_ {
	enum {E_varEntry, E_funEntry} kind;
	union {struct {Ty_ty ty;} var;
			struct {Ty_tyList formals; Ty_ty result;} fun;
	} u;
};				/* ----------  end of struct E_enventry_  ---------- */

typedef struct E_enventry_ *E_enventry;

E_enventry E_VarEntry(Ty_ty ty);
E_enventry E_FunEntry(Ty_tyList formals, Ty_ty result);

S_table E_base_tenv(void);
S_table E_base_venv(void);
#endif   /* ----- #ifndef ENV_H__INC  ----- */
