%{
#include <string.h>
#include "util.h"
#include "tokens.h"
#include "errormsg.h"

int charPos=1;

int yywrap(void)
{
 charPos=1;
 return 1;
}


void adjust(void)
{
 EM_tokPos=charPos;
 charPos+=yyleng;
}

%}

%Start COMMENT

%%

<INITIAL>" "	 {adjust(); continue;}
<INITIAL>\n	 {adjust(); EM_newline(); continue;}
<INITIAL>","	 {adjust(); return COMMA;}
<INITIAL>\t   {adjust(); continue;}
<INITIAL>":"  {adjust(); return COLON;}
<INITIAL>";"  {adjust(); return SEMICOLON;} 
<INITIAL>"("  {adjust(); return LPAREN;}
<INITIAL>")"  {adjust(); return RPAREN;}
<INITIAL>"["  {adjust(); return LBRACK;}
<INITIAL>"]"  {adjust(); return RBRACK;}
<INITIAL>"{"  {adjust(); return LBRACE;}
<INITIAL>"}"  {adjust(); return RBRACE;}
<INITIAL>"."  {adjust(); return DOT;}
<INITIAL>"+"  {adjust(); return PLUS;}
<INITIAL>"-"  {adjust(); return MINUS;}
<INITIAL>"*"  {adjust(); return TIMES;}
<INITIAL>"/"  {adjust(); return DIVIDE;}
<INITIAL>"="  {adjust(); return EQ;}
<INITIAL>"<>" {adjust(); return NEQ;}
<INITIAL>"<"  {adjust(); return LT;}
<INITIAL>"<=" {adjust(); return LE;}
<INITIAL>">"  {adjust(); return GT;}
<INITIAL>">=" {adjust(); return GE;}
<INITIAL>"&"  {adjust(); return AND;}
<INITIAL>"|"  {adjust(); return OR;}
<INITIAL>":=" {adjust(); return ASSIGN;}
<INITIAL>for  	 {adjust(); return FOR;}
<INITIAL>array {adjust(); return ARRAY;}
<INITIAL>if  {adjust(); return IF;}
<INITIAL>then {adjust(); return THEN;}
<INITIAL>else {adjust(); return ELSE;}
<INITIAL>while {adjust(); return WHILE;}
<INITIAL>to  {adjust(); return TO;}
<INITIAL>do  {adjust(); return DO;}
<INITIAL>let  {adjust(); return LET;}
<INITIAL>in  {adjust(); return IN;}
<INITIAL>end  {adjust(); return END;}
<INITIAL>of  {adjust(); return OF;}
<INITIAL>break {adjust(); return BREAK;}
<INITIAL>nil  {adjust(); return NIL;}
<INITIAL>var  {adjust(); return VAR;}
<INITIAL>type {adjust(); return TYPE;}
<INITIAL>function {adjust(); return FUNCTION;}
<INITIAL>[0-9]+	 {adjust(); yylval.ival=atoi(yytext); return INT;}
<INITIAL>[a-zA-Z]+[a-zA-Z0-9_]*	{adjust(); yylval.sval = strdup(yytext);return ID;}
<INITIAL>.	 {adjust(); EM_error(EM_tokPos,"illegal token");}
<INITIAL>"/*"	{adjust(); BEGIN COMMENT;}
<COMMENT>"*/"	{adjust(); BEGIN INITIAL;}
<COMMENT>\n	{adjust();continue;}
<COMMENT>.	{adjust();continue;}

