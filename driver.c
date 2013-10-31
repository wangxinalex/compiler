#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "errormsg.h"
#include "tokens.h"

YYSTYPE yylval;

int yylex(void); /* prototype for the lexing function */



string toknames[] = {
"ID", "STRING", "INT", "COMMA", "COLON", "SEMICOLON", "LPAREN",
"RPAREN", "LBRACK", "RBRACK", "LBRACE", "RBRACE", "DOT", "PLUS",
"MINUS", "TIMES", "DIVIDE", "EQ", "NEQ", "LT", "LE", "GT", "GE",
"AND", "OR", "ASSIGN", "ARRAY", "IF", "THEN", "ELSE", "WHILE", "FOR",
"TO", "DO", "LET", "IN", "END", "OF", "BREAK", "NIL", "FUNCTION",
"VAR", "TYPE"
};


string tokname(tok) {
  return tok<257 || tok>299 ? "BAD_TOKEN" : toknames[tok-257];
}

int main(int argc, char **argv) {
 string fname; int tok;
 if (argc!=2) {fprintf(stderr,"usage: a.out filename\n"); exit(1);}
 fname=argv[1];
 EM_reset(fname);
 for(;;) {
   tok=yylex();
   if (tok==0) break;
   switch(tok) {
   case ID: case STRING:
     printf("%10s %4d %s\n",tokname(tok),EM_tokPos,yylval.sval);
     break;
   case INT:
     printf("%10s %4d %d\n",tokname(tok),EM_tokPos,yylval.ival);
     break;
   case COMMA: case COLON: case SEMICOLON: case LPAREN: case RPAREN:
   case LBRACK: case RBRACK: case LBRACE: case RBRACE: case DOT:
   case PLUS: case MINUS: case TIMES: case DIVIDE: case EQ:
   case NEQ: case LT: case LE: case GT: case GE: 
   case AND: case OR: case ASSIGN: case ARRAY: case IF:
   case THEN: case ELSE: case WHILE: case FOR: case TO:
   case DO: case LET: case IN: case END: case OF: 
   case BREAK: case NIL: case FUNCTION: case VAR: case TYPE: 
	 printf("%10s %4d\n", tokname(tok),EM_tokPos);
	 break;
   default:
     printf("%10s %4d\n",tokname(tok),EM_tokPos);
   }
 }
 return 0;
}


