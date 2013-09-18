/*========================================================================
#   FileName: main.c
#     Author: wangxinalex
#      Email: wangxinalex@gmail.com
#   HomePage: http://singo.10ss.me
# LastChange: 2013-09-16 09:52:45
========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "slp.h"
#include "prog1.h"

#define MAX_VAR 100

int maxargs(A_stm prog,int *args);
A_table interpStm(A_stm prog, A_table table);
struct IntAndTable interpExp( A_exp exp, A_table table);
int lookup(string s, A_table t);
int calculate(int left, int right, A_binop op);
void print_table(A_table t);

//return the maximum number of arguments in print statements
int maxargs(A_stm prog, int *max){
	int args = 0;
	//compound statements
	if(prog->kind == A_compoundStm){
		maxargs(prog->u.compound.stm1, max);
		maxargs(prog->u.compound.stm2, max);
	//assignment statements
	}else if(prog->kind == A_assignStm){
		if(prog->u.assign.exp->kind == A_eseqExp){
	//recursively handle the statements in eseqExp
			A_stm ex_prog = prog->u.assign.exp->u.eseq.stm;
			maxargs(ex_prog, max);
		}
	//print statements
	}else{
		A_expList list = prog->u.print.exps;
		while(list->kind != A_lastExpList){
			args++;
	//recursively handle the statements in eseqExp
			if(list->u.pair.head->kind == A_eseqExp){
				A_stm ex_prog = list->u.pair.head->u.eseq.stm;
				maxargs(ex_prog, max);
			}
			list = list->u.pair.tail;
		}
		if(++args > *max){
			*max = args;
		}
	}
	return 0;
}

//interpret statements recursively
A_table interpStm(A_stm prog, A_table table){
	if(prog->kind == A_compoundStm){
		table = interpStm(prog->u.compound.stm1, table);
		table = interpStm(prog->u.compound.stm2, table);

	}else if(prog->kind == A_assignStm){
		//handle the assignment expression, return the value and change the "id x int" table
		struct IntAndTable int_and_table = 
			interpExp( prog->u.assign.exp, table);
		//change the table by returned value
		table = (A_table)Table(prog->u.assign.id, int_and_table.i, int_and_table.t);
		//print_table(table);

	}else if(prog->kind == A_printStm){
		A_expList list = prog->u.print.exps;
		//handle the side-effects in eseqExp
		while(list->kind != A_lastExpList){
			if(list->u.pair.head->kind == A_eseqExp){
				A_stm ex_prog = list->u.pair.head->u.eseq.stm;
				interpStm(ex_prog, table);
			}
			list = list->u.pair.tail;
		}	
		if(list->u.last->kind == A_eseqExp){
			interpStm(list->u.last->u.eseq.stm, table);
		}
	}	
	return table;
}

//handle the expression, return the value and change the table 
struct IntAndTable interpExp( A_exp exp, A_table table){
	struct IntAndTable iat;
	if(exp->kind == A_numExp){
		iat.i = exp->u.num;
		iat.t = table;
		return iat;
	}else if(exp->kind == A_idExp){
		int val = lookup(exp->u.id, table);
		iat.i = val;
		iat.t = table;
		return iat;
	}else if(exp->kind == A_opExp){
		struct IntAndTable left_iat, right_iat, final_iat;
		//from left to right, as well as side-effects
		left_iat = interpExp(exp->u.op.left, table);
		right_iat = interpExp(exp->u.op.right, left_iat.t);
		final_iat.i = calculate(left_iat.i, right_iat.i, exp->u.op.oper);
		final_iat.t = right_iat.t;
		return final_iat;
	}else if(exp->kind == A_eseqExp){
		//recursively handle the statements in eseqStm
		iat.t = interpStm(exp->u.eseq.stm, table);
		//record the returned value
		iat = interpExp(exp->u.eseq.exp, iat.t);
		return iat;
	}
	return iat;
}

//get value by id in the table
int lookup(string s, A_table t){
	while(t != NULL){
		if(strcmp(t->id, s)==0){
			return t->value;
		}
		t = t->tail;
	}	
	
	printf("[Error] Identifier %s does not exist!\n\n",s);
	exit(-1);
}

//print format
void print_table(A_table t){
	printf("\n        Id:Value     \n");
	printf("[^_^] Now dump Variable\n");
	int index = 0, i = 0;
	string s_list[MAX_VAR];
	int is_dup = 0;
	while(t != NULL){
		is_dup = 0;
		for(i = 0; i < index; i++){
			if(strcmp(s_list[i], t->id)==0){
				is_dup = 1;
			}
		}
		if(is_dup == 0){
			s_list[index++] =  t->id;
			printf("         %s=%d        \n", t->id, t->value); 
		}
		t = t->tail;
	}	
	printf("\n");
}

//calculate opExp
int calculate(int left, int right, A_binop op){
	int result;
	switch(op){
		case A_plus:
			result = left + right; break;
		case A_minus:
			result = left - right; break;
		case A_times:
			result = left * right; break;
		case A_div:
			if(right == 0){
				printf("[Error] divisor cannot be zero!\n\n");
				exit(-2);
			}
			result = left / right; break;
		default:
			result = 0;
	}
	return result;
}

int main(){
	//right statements
	int max_args = 0;
	A_table global_table = NULL;
	A_stm right_stm = right_prog();
	maxargs(right_stm, &max_args);
	printf(">> Right Prog Section:\n");
	printf("the maximum number of arguments of any print statement is %d\n",max_args);
	global_table = interpStm(right_stm, global_table);
	print_table(global_table);

	//error statements
	//initialize the global variables
	global_table = NULL;
	max_args = 0;
	A_stm error_stm = error_prog();
	maxargs(error_stm, &max_args);
	printf(">> Error Prog Section:\n");
	printf("the maximum number of arguments of any print statement is %d\n",max_args);
	global_table = interpStm(error_stm, global_table);
	print_table(global_table);

   return 0;
}
