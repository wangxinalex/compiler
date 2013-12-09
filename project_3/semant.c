/*
 * =====================================================================================
 *
 *       Filename:  semant.c
 *
 *    Description:  perfroms the semantic analysis
 *
 *        Version:  1.0
 *        Created:  12/02/13 20:51:27
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  wangxinalex (), wangxinalex@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "semant.h"
#include "util.h"
#include "errormsg.h"
#include "env.h"

#define MAX_LEN 512

static bool record_flag = 0;
static bool illegal_record = 0;

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  equal_Ty
 *  Description:  check whether two types consistent
 * =====================================================================================
 */
int equal_Ty(Ty_ty left_ty, Ty_ty right_ty){
	Ty_ty real_left_ty = actual_ty(left_ty);
	Ty_ty real_right_ty = actual_ty(right_ty);
	/*printf ( "left = %d, right = %d\n", real_left_ty->kind, real_right_ty->kind);*/

	int flag1 = (real_left_ty->kind == Ty_record||real_left_ty->kind == Ty_array)&&(real_left_ty==real_right_ty);
	int flag2 =	(real_left_ty->kind != Ty_record&&real_left_ty->kind!=Ty_array&&real_left_ty->kind==real_right_ty->kind);
	int flag3 = (real_left_ty->kind == Ty_record) && (real_right_ty->kind == Ty_nil);
	int flag = flag1||flag2||flag3;
	/*printf ( "flag = %d %d %d\n", flag1, flag2, flag3 );*/
	return flag;
}

Ty_ty actual_ty(Ty_ty ty){
	if(ty->kind == Ty_name){
		return actual_ty(ty->u.name.ty);		
	}else{
		return ty;
	}
}

Ty_ty S_look_type(S_table tenv, S_symbol sym){
	Ty_ty type = S_look(tenv, sym);
	if(type)
		return actual_ty(type);
	else 
		return NULL;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  makeFieldTys
 *  Description:  fill in a list of each element in a record type
 * =====================================================================================
 */
Ty_fieldList makeFieldTys(S_table tenv, A_fieldList fields ){
	Ty_fieldList   fields_head = NULL;
	Ty_fieldList   fields_tail = NULL;

	Ty_field field;
	A_fieldList index;
	Ty_ty t; 
	for(index = fields; index; index=index->tail){
		/*printf("name = %s  type = %s\n", S_name(index->head->name), S_name(index->head->typ));*/
		if((t = S_look(tenv, index->head->typ))==NULL){
			/*printf("makeFields\n");*/
			if(!record_flag){
				EM_error(index->head->pos, "undefined type %s", S_name(index->head->typ));
			}else{
				illegal_record = 1;			
			}
		}else{
			field = Ty_Field(index->head->name, t);		
			if(fields_head){
				fields_tail->tail = Ty_FieldList(field,NULL);	
				fields_tail = fields_tail->tail;
			}else{
				fields_head = Ty_FieldList(field, NULL);
				fields_tail = fields_head;
			}
		}
	}
	return fields_head;
}
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  makeFormalTyList
 *  Description:  fill in a list of formal parameters of a function
 * =====================================================================================
 */
static Ty_tyList makeFormalTyList (S_table tenv, A_fieldList params ){
	Ty_tyList param_head = NULL;
	Ty_tyList param_tail = NULL;
	A_fieldList f_list = NULL;
	Ty_ty type;
	for ( f_list = params; f_list; f_list = f_list->tail) {
		/*printf ( "flist->head->name = %s\n", S_name(f_list->head->name) );*/
		type = S_look_type(tenv, f_list->head->typ);
		/*printf ( "type = %p\n", type );*/
	   	if(type==NULL){
	   		EM_error(f_list->head->pos, "undefined type %s", S_name(f_list->head->typ));

	   	}else{ 
			if(param_head){
				param_tail->tail = Ty_TyList(type, NULL);
				param_tail = param_tail->tail;
			}else{
				param_head = Ty_TyList(type, NULL);
				param_tail = param_head;
			}
		}
	}
	return param_head;
}		

struct expty expTy(Tr_exp exp, Ty_ty ty){
	struct expty e;
	e.exp = exp;
	e.ty = ty;
	return e;
}

void  SEM_transProg(A_exp exp){
	S_table tenv = E_base_tenv();
	S_table venv = E_base_venv();
	transExp(venv, tenv, exp);
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  transTy
 *  Description:  translate the A_ty to Ty_ty
 * =====================================================================================
 */
Ty_ty transTy(S_table tenv, A_ty t){
	Ty_ty ty;
	switch(t->kind){
		case A_nameTy:{
			if((ty = S_look(tenv, t->u.name))==NULL){
				EM_error(t->pos, "undefined type %s", S_name(t->u.name));
			}
			return ty;
		}	
		case A_recordTy:{
			Ty_fieldList f_list = makeFieldTys(tenv, t->u.record);
			return Ty_Record(f_list);
		}
		case A_arrayTy:{
			if((ty = S_look(tenv, t->u.name)) == NULL){
				EM_error(t->pos, "undefined type %s", S_name(t->u.name));
			}
			return Ty_Array(ty);
		}
	}
}
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  transDec
 *  Description:  translate a declaration and add the declaration records to environment
 * =====================================================================================
 */
static void transDec(S_table venv , S_table tenv, A_dec d){
	switch(d->kind){
		case A_functionDec:{
			A_fundecList f_list;
			Ty_tyList formal_tys;
			/*U_boolList formal_bools;*/
			Ty_ty result_ty;
			E_enventry fun_entry;
			Ty_tyList param_tys;
			string previous_name = calloc(MAX_LEN, sizeof(char));
			/*add the variables in declaration into environment*/
			for(f_list = d->u.function; f_list; f_list = f_list->tail){
				if(!strcmp(previous_name, S_name(f_list->head->name))){
					EM_error(d->u.function->head->pos, "two functions has same name");
				}
				result_ty = NULL;
				if(strlen(S_name(f_list->head->result))){
					if((result_ty = S_look(tenv, f_list->head->result))==NULL){
						EM_error(d->pos, "undefined return type");
					}	
				}					
				if(result_ty == NULL){
					result_ty = Ty_Void();
				}
				formal_tys = makeFormalTyList(tenv, f_list->head->params);
				Ty_tyList index = formal_tys;
				S_enter(venv, f_list->head->name, E_FunEntry(formal_tys, result_ty));
				strcpy(previous_name, S_name(f_list->head->name));
			}			
			/*check the corresponding types of declaration*/
			for(f_list = d->u.function; f_list; f_list = f_list->tail){
				fun_entry = S_look(venv, f_list->head->name);
				S_beginScope(venv);
				param_tys = fun_entry->u.fun.formals;
				A_fieldList param_list;
				for ( param_list = f_list->head->params; param_list; param_list = param_list->tail, param_tys = param_tys->tail) {
					S_enter(venv, param_list->head->name, E_VarEntry(param_tys->head));
				}
				struct expty e = transExp(venv, tenv , f_list->head->body);
				if(fun_entry->u.fun.result->kind==Ty_void && e.ty->kind!=Ty_void){
					EM_error(f_list->head->body->pos, "procedure returns value");
				}
				else if(!equal_Ty(fun_entry->u.fun.result, e.ty )){
					EM_error(f_list->head->body->pos, "return type mismatch");
				}
				S_endScope(venv);
			}
			break;
		}
		case A_varDec:{
			/*printf("varDec d->u.var.init= %d\n", (d->u.var.init==NULL));*/
			if(d->u.var.init == NULL){
				S_enter(venv, d->u.var.var, E_VarEntry(Ty_Void()));
				break;
			}
			struct expty e = transExp(venv, tenv, d->u.var.init);			  
			/*no type declared, take the type of init expression as default type*/
			if(strlen(S_name(d->u.var.typ))){
				Ty_ty type;
			    if((type = S_look_type(tenv, d->u.var.typ)) == NULL){
					EM_error(d->pos, "undefined type %s", S_name(d->u.var.typ));
					type = Ty_Void();
				}
				/*printf ( "d.type = %s\n", S_name(d->u.var.typ) );*/
				if(!equal_Ty(type, e.ty)){
					EM_error(d->u.var.init->pos, "type mismatch");
				}
				S_enter(venv, d->u.var.var, E_VarEntry(type));
			}else{
				if(e.ty->kind == Ty_nil){
					EM_error(d->pos,"type required");
				}
				S_enter(venv, d->u.var.var, E_VarEntry(e.ty) );
			}
			break;
		}
		case A_typeDec:{
			/*S_enter(tenv, d->u.type->head->name, transTy(tenv, d->u.type->head->ty));			   */
			A_nametyList index;
			Ty_ty t, nameTy;
			
			bool hasCycle = 1;
			string previous_name = calloc(MAX_LEN,sizeof(char));
			for(index = d->u.type; index; index = index->tail ){
				S_enter(tenv, index->head->name, Ty_Name(index->head->name, NULL));
			}
			for(index = d->u.type; index; index = index->tail ){
				if(!strcmp(previous_name, S_name(index->head->name))){
					EM_error(d->u.type->head->ty->pos, "two types has same name");
				}
				record_flag = 1;
				t = transTy(tenv, index->head->ty);
				record_flag = 0;
				/*eliminating cyclic definition of type declaration*/
				if(hasCycle){
					if(t->kind != Ty_name){
						hasCycle = 0;
					}
				}
				if(illegal_record){
					/*A_var var = S_look(venv, d->u.type->head->ty->u.record->head->name);*/
					EM_error(d->u.type->head->ty->pos-10, "type %s is illegal", S_name(d->u.type->head->name));
					illegal_record =0;
				}
				nameTy = S_look(tenv, index->head->name);
				nameTy->u.name.ty = t;
				strcpy(previous_name, S_name(index->head->name));
			}
			if(hasCycle){
				EM_error(d->u.type->head->ty->pos, "illegal type cycle");
			}
			free(previous_name);
			break;
		} 
	}
}
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  transExp
 *  Description:  translate the expression into environment
 * =====================================================================================
 */
struct expty transExp(S_table venv, S_table tenv, A_exp a){
	/*printf("transExp a->kind = %d\n", a->kind);*/
	switch(a->kind){
		case A_varExp:
			return transVar(venv, tenv, a->u.var);
		case A_nilExp:
			return expTy(NULL, Ty_Nil());
		case A_stringExp:
			return expTy(NULL, Ty_String());
		case A_intExp:
			return expTy(NULL, Ty_Int());
		case A_callExp:{
			A_expList args;
			Ty_tyList formal_tys;
			E_enventry call_entry = S_look(venv, a->u.call.func);
			/*calculate the number of arguments and the formal parameters*/
			int size_args , size_formals; 
			if(call_entry && call_entry->kind == E_funEntry){
				formal_tys = call_entry->u.fun.formals;
				args = a->u.call.args;
				for ( size_args = 0; args; size_args++, args=args->tail) ;
				for ( size_formals = 0; formal_tys; size_formals++, formal_tys=formal_tys->tail) ;
				if(size_args < size_formals && size_args){
					EM_error(a->u.call.args->head->pos, "para type mismatched");
				}else if(size_args>size_formals){
					args = a->u.call.args;
					int index = 0;
					for(; index < size_formals - 1; args=args->tail, index++);
					EM_error(args->head->pos, "para type mismatched" );
				}
				formal_tys = call_entry->u.fun.formals;
				args = a->u.call.args;
				for(;args&&formal_tys;args = args->tail,formal_tys = formal_tys->tail){
					struct expty this_arg = transExp(venv, tenv, args->head);		
					if(!equal_Ty(this_arg.ty, formal_tys->head)){
						EM_error(args->head->pos, "para type mismatched");	
					}
				}
				
				Ty_ty result_ty = actual_ty(call_entry->u.fun.result );
				return expTy(NULL, result_ty );
			}else{
				EM_error(a->pos, "undefined function %s", S_name(a->u.call.func));
				return expTy(NULL, Ty_Int());
			}
		}
		case A_opExp:{
			struct expty left = transExp(venv, tenv, a->u.op.left);			 
			struct expty right = transExp(venv, tenv, a->u.op.right);			 
			switch(a->u.op.oper){
				case A_plusOp:
				case A_minusOp:
				case A_timesOp:
				case A_divideOp:{
					if(left.ty->kind != Ty_int){
						EM_error(a->u.op.left->pos, "integer required");
					}
					if(right.ty->kind != Ty_int){
						EM_error(a->u.op.right->pos, "integer required");
					}
					return expTy(NULL, Ty_Int());
				}
				case A_eqOp:
				case A_neqOp:
				case A_ltOp:
				case A_leOp:
				case A_gtOp:
				case A_geOp:{
				/*printf("left = %d, right = %d\n", left.ty->kind, right.ty->kind);*/
					if(left.ty->kind != right.ty->kind&&right.ty->kind!=Ty_nil){
						EM_error(a->pos,"same  type required");
					}	
					return expTy(NULL, Ty_Int());
				}

			}
			return expTy(NULL, Ty_Int());
		}
		case A_recordExp:{
			Ty_ty type = S_look_type(tenv , a->u.record.typ );					 
			if(type == NULL){
				EM_error(a->pos,"undefined record %s",S_name(a->u.record.typ));
				return expTy(NULL, Ty_Record(NULL));
			}
			if(type->kind != Ty_record){
				EM_error(a->pos, "not a record type");	
			}
			/*definition of record type*/
			Ty_fieldList f_list = type->u.record;
			/*initialization of record type*/
			A_efieldList ef_list;
			for ( ef_list = a->u.record.fields; ef_list; ef_list=ef_list->tail,f_list = f_list->tail ) {
				struct expty e = transExp(venv, tenv, ef_list->head->exp);
				if(f_list->head->name != ef_list->head->name){
					EM_error(a->pos, "field %s doesn't exist", S_name(ef_list->head->name));
				}
				/*check the types of each members in definition and initialization */
				if(!equal_Ty(f_list->head->ty, e.ty)){
					EM_error(ef_list->head->exp->pos,"type mismatch");
				}
			}
			return expTy(NULL, type);
		} 
		case A_seqExp:{
			struct expty expr = expTy(NULL, Ty_Void());
			A_expList index = a->u.seq;
			/*printf ( "index = %d\n", index->head==NULL );*/
			for(; index; index = index->tail){
				if(index->head){
					expr = transExp(venv, tenv, index->head);	
				}else{
					return expTy(NULL, Ty_Void());
				}
			}
			return expTy(NULL, expr.ty);
		}
		case A_assignExp:{
			struct expty var_ty = transVar(venv, tenv, a->u.assign.var);				 
			struct expty exp_ty = transExp(venv, tenv, a->u.assign.exp);				 
			if(!equal_Ty(var_ty.ty, exp_ty.ty) && var_ty.ty->kind != Ty_void){
				EM_error(a->pos, "type mismatch");
			}
			return expTy(NULL, Ty_Void());
		}
		case A_ifExp:{
			struct expty test, then, elsee;
			test = transExp(venv, tenv, a->u.iff.test);
			then = transExp(venv, tenv, a->u.iff.then);
			if(a->u.iff.elsee){
				elsee = transExp(venv, tenv, a->u.iff.elsee);
				if(!equal_Ty(then.ty, elsee.ty)){
					EM_error(a->u.iff.elsee->pos,"then exp and else exp type mismatch");
				}
				return expTy(NULL, then.ty);
			}
			else if(then.ty->kind != Ty_void){
				EM_error(a->u.iff.then->pos,"this exp must produce no value");
			}
			return expTy(NULL, Ty_Void());
		}

		case A_whileExp:{
			struct expty test, body;
			test = transExp(venv, tenv, a->u.whilee.test);	
			body = transExp(venv, tenv, a->u.whilee.body);	
			if(body.ty->kind != Ty_void){
				EM_error(a->u.whilee.body->pos, "this exp must produce no value");
			}
			return expTy(NULL, Ty_Void());
		}

		case A_forExp:{
			A_dec var_dec = A_VarDec(a->pos, a->u.forr.var, NULL, NULL  );
			transDec(venv, tenv, var_dec);
			struct expty lo, hi ;
			lo = transExp(venv, tenv, a->u.forr.lo);
			hi = transExp(venv, tenv, a->u.forr.hi);
			if(lo.ty->kind != Ty_int){
				EM_error(a->u.forr.lo->pos, "integer type required");
			}
			if(hi.ty->kind != Ty_int){
				EM_error(a->u.forr.hi->pos, "integer type required");
			}
			S_symbol index_var = a->u.forr.var;
			A_exp body  = a->u.forr.body;
			/*cannot assign to index variable*/
			if(body->kind == A_assignExp){
				if(!strcmp(S_name(body->u.assign.var->u.simple), S_name(index_var))){
					EM_error(body->pos, "invalid assign to index");
				}
			}	
			return expTy(NULL, Ty_Void());
		}
		case A_breakExp:{
			return expTy(NULL, Ty_Void());
		}
		case A_letExp:{
			struct expty exp;
			A_decList index;
			S_beginScope(venv);
			S_beginScope(tenv);
			for(index = a->u.let.decs;index;index=index->tail){
				transDec(venv, tenv, index->head);
			}
			exp = transExp(venv, tenv, a->u.let.body);
			S_endScope(tenv);	
			S_endScope(venv);	

			return exp;
						
		}
		case A_arrayExp:{
			Ty_ty type;
		    if ((type = S_look_type(tenv, a->u.array.typ))==NULL){
				EM_error(a->pos, "undefined type");
				return expTy(NULL, Ty_Int());	
		    }else{
				struct expty size = transExp(venv, tenv, a->u.array.size);
				struct expty init = transExp(venv, tenv, a->u.array.init);
				if(size.ty->kind != Ty_int){
					EM_error(a->u.array.size->pos, "integer type required");
				}
				if(!equal_Ty(type->u.array, init.ty)){
					EM_error(a->u.array.init->pos, "type mismatched");
				}
				return expTy(NULL, type);
			}			

		}

	}
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  transVar
 *  Description:  translate the variable definition into environment
 * =====================================================================================
 */
struct expty transVar(S_table venv , S_table tenv, A_var v){
	switch(v->kind){
		case A_simpleVar:{
			E_enventry x = S_look(venv, v->u.simple);	
			if(x && x->kind == E_varEntry){
				return expTy(NULL, actual_ty(x->u.var.ty));
			}else{
				EM_error(v->pos, "undefined variable %s", S_name(v->u.simple));
				return expTy(NULL, Ty_Int());
			}
		}	
		case A_fieldVar:{
			struct expty e = transVar(venv, tenv, v->u.field.var);	
			/*must be a record type*/
			if(e.ty->kind != Ty_record){
				EM_error(v->u.field.var->pos,"record var required");
				EM_error(v->pos, "field %s doesn't exist", S_name(v->u.field.sym));
			}else{
				Ty_fieldList f_list;
				for(f_list = e.ty->u.record; f_list; f_list = f_list->tail){
					if(f_list->head->name == v->u.field.sym){
						return expTy(NULL, actual_ty(f_list->head->ty));
					}
				}
				EM_error(v->pos, "field %s doesn't exist", S_name(v->u.field.sym));
				return expTy(NULL, Ty_Void());
			}	
			return expTy(NULL, Ty_Int());	
		} 
		case A_subscriptVar:{
			struct expty array_exp = transVar(venv, tenv, v->u.subscript.var); 
			/*the var of subscipt variable must be an array*/
			if(array_exp.ty->kind != Ty_array){
				EM_error(v->u.subscript.var->pos, "array required");
				return expTy(NULL, Ty_Int());
			}else{
				struct expty index = transExp(venv, tenv, v->u.subscript.exp);
				if(index.ty->kind != Ty_int){
					EM_error(v->u.subscript.exp->pos, "integer required");
				}
				return expTy(NULL,actual_ty(array_exp.ty->u.array));
			}
		}
	}
}

