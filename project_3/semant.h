/*
 * =====================================================================================
 *
 *       Filename:  semant.h
 *
 *    Description:  header file of semant.c
 *
 *        Version:  1.0
 *        Created:  12/02/13 20:51:06
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  wangxinalex (), wangxinalex@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef  SEMANT_H_INC
#define  SEMANT_H_INC

#include "absyn.h"
#include "types.h"
typedef void *Tr_exp;
struct expty {Tr_exp exp; Ty_ty ty;};

void SEM_transProg(A_exp);
static Ty_ty actual_ty(Ty_ty);
static Ty_ty S_look_type(S_table, S_symbol);
struct expty expTy(Tr_exp , Ty_ty );
static int equal_Ty(Ty_ty, Ty_ty);
static struct expty transVar(S_table , S_table , A_var );
static struct expty transExp(S_table , S_table , A_exp );
static void transDec(S_table , S_table , A_dec );
static Ty_ty transTy(S_table , A_ty );
static Ty_fieldList makeFieldTys(S_table, A_fieldList);
static Ty_tyList makeFormalTyList (S_table, A_fieldList );

#endif   /* ----- #ifndef SEMANT_H__INC  ----- */
