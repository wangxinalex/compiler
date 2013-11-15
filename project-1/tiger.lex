%{
#include <string.h>
#include "util.h"
#include "tokens.h"
#include "errormsg.h"

#define MAX_LEN 512
int charPos=1;
int start_pos = 0;
char str[MAX_LEN];
char* str_ptr;
int str_len = 0;

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

void adjust_string(void){
	EM_tokPos = charPos - 1;
	charPos += yyleng;
}

void start_string(){
	str_len = 0;
	start_pos = EM_tokPos;
	str_ptr = str;
	*str_ptr = '\0';
}
void end_string(){
	if(!str_len){
		strcpy(str_ptr, "(null)");
	}
	EM_tokPos = start_pos;
}
void handle_string(char *input){
	strcpy(str_ptr, input);
	str_len += strlen(input);	
}
void handle_char(char c){
	while(*str_ptr)
		str_ptr++;
	*str_ptr++ = c;
	str_len++;
	*str_ptr = '\0';
}
%}

%Start COMMENT STRING_STATE

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
<INITIAL>array {adjust(); return ARRAY;}
<INITIAL>if  {adjust(); return IF;}
<INITIAL>then {adjust(); return THEN;}
<INITIAL>else {adjust(); return ELSE;}
<INITIAL>while {adjust(); return WHILE;}
<INITIAL>for  	 {adjust(); return FOR;}
<INITIAL>to  {adjust(); return TO;}
<INITIAL>do  {adjust(); return DO;}
<INITIAL>let  {adjust(); return LET;}
<INITIAL>in  {adjust(); return IN;}
<INITIAL>end  {adjust(); return END;}
<INITIAL>of  {adjust(); return OF;}
<INITIAL>break {adjust(); return BREAK;}
<INITIAL>nil  {adjust(); return NIL;}
<INITIAL>function {adjust(); return FUNCTION;}
<INITIAL>var  {adjust(); return VAR;}
<INITIAL>type {adjust(); return TYPE;}
<INITIAL>[0-9]+	 {adjust(); yylval.ival=atoi(yytext); return INT;}
<INITIAL>[a-zA-Z]+[a-zA-Z0-9_]*	{adjust(); yylval.sval = strdup(yytext);return ID;}

<INITIAL>\"	{adjust(); start_string();BEGIN STRING_STATE;}
<STRING_STATE>\\[nN] {adjust(); handle_char('\n');}
<STRING_STATE>\\[tT] {adjust(); handle_char('\t');}
<STRING_STATE>\\^@ {adjust(); handle_char(0x00);}
<STRING_STATE>\\^[aA] {adjust(); handle_char(0x01);}
<STRING_STATE>\\^[bB] {adjust(); handle_char(0x02);}
<STRING_STATE>\\^[cC] {adjust(); handle_char(0x03);}
<STRING_STATE>\\^[dD] {adjust(); handle_char(0x04);}
<STRING_STATE>\\^[eE] {adjust(); handle_char(0x05);}
<STRING_STATE>\\^[fF] {adjust(); handle_char(0x06);}
<STRING_STATE>\\^[gG] {adjust(); handle_char(0x07);}
<STRING_STATE>\\^[hH] {adjust(); handle_char(0x08);}
<STRING_STATE>\\^[iI] {adjust(); handle_char(0x09);}
<STRING_STATE>\\^[jJ] {adjust(); handle_char(0x0a);}
<STRING_STATE>\\^[kK] {adjust(); handle_char(0x0b);}
<STRING_STATE>\\^[lL] {adjust(); handle_char(0x0c);}
<STRING_STATE>\\^[mM] {adjust(); handle_char(0x0d);}
<STRING_STATE>\\^[nM] {adjust(); handle_char(0x0e);}
<STRING_STATE>\\^[oO] {adjust(); handle_char(0x0f);}
<STRING_STATE>\\^[pP] {adjust(); handle_char(0x10);}
<STRING_STATE>\\^[qQ] {adjust(); handle_char(0x11);}
<STRING_STATE>\\^[rR] {adjust(); handle_char(0x12);}
<STRING_STATE>\\^[sS] {adjust(); handle_char(0x13);}
<STRING_STATE>\\^[tT] {adjust(); handle_char(0x14);}
<STRING_STATE>\\^[uU] {adjust(); handle_char(0x15);}
<STRING_STATE>\\^[vV] {adjust(); handle_char(0x16);}
<STRING_STATE>\\^[wW] {adjust(); handle_char(0x17);}
<STRING_STATE>\\^[xX] {adjust(); handle_char(0x18);}
<STRING_STATE>\\^[yY] {adjust(); handle_char(0x19);}
<STRING_STATE>\\^[zZ] {adjust(); handle_char(0x1a);}
<STRING_STATE>\\^\[ {adjust(); handle_char(0x1b);}
<STRING_STATE>\\^\\ {adjust(); handle_char(0x1c);}
<STRING_STATE>\\^\] {adjust(); handle_char(0x1d);}
<STRING_STATE>\\^^ {adjust(); handle_char(0x1e);}
<STRING_STATE>\\^_ {adjust(); handle_char(0x1f);}
<STRING_STATE>\\^? {adjust(); handle_char(0x7f);}
<STRING_STATE>\\[0-9]{3}	{adjust(); handle_char(atoi(yytext));}
<STRING_STATE>\\\"	{adjust(); handle_char(0x22); }
<STRING_STATE>\\\\	{adjust(); handle_char(0x5c); }
<STRING_STATE>\\[[:space:]]+\\	{adjust();}
<STRING_STATE>[^"]+ {adjust(); handle_string(yytext);};
<STRING_STATE>\"	{adjust(); 
					end_string(); 
					yylval.sval = strdup(str);
					BEGIN INITIAL; 
					return STRING;} 

<INITIAL>"/*"	{adjust(); BEGIN COMMENT;}
<COMMENT>"*/"	{adjust(); BEGIN INITIAL;}
<COMMENT>\n	{adjust();continue;}
<COMMENT>.	{adjust();continue;}

<INITIAL>.	 {adjust(); EM_error(EM_tokPos,"illegal token");}
