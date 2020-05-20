%{

#include "output.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "parser.hpp"


#define _(TOKEN) { return TOKEN; }
#define _ERROR(NUM) { output::errorLex(NUM); exit(0); }

%}

%option noyywrap
%option yylineno



CR      (\x0D)
LF      (\x0A)
SPACE   (\x20)
TAB     (\x09)
%%

void                                                                  {yylval=new Type(TYPE_VOID);return VOID;}
int                                                                   {yylval=new Type(TYPE_INT);return INT;}
byte                                                                  {yylval=new Type(TYPE_BYTE);return BYTE;}
b                                                                     {yylval=new Type(TYPE_B);return B;}
bool                                                                  {yylval=new Type(TYPE_BOOL);return BOOL;}
and                                                                   _(AND);
or                                                                    _(OR);
not                                                                   _(NOT);
true                                                                  _(TRUE);
false                                                                 _(FALSE);
return                                                                _(RETURN);
if                                                                    _(IF);
else                                                                  _(ELSE);
while                                                                 _(WHILE);
break                                                                 _(BREAK);
continue                                                              _(CONTINUE);
;                                                                     _(SC);
,                                                                     _(COMMA);
\(                                                                    _(LPAREN);
\)                                                                    _(RPAREN);
\{                                                                    _(LBRACE);
\}                                                                    _(RBRACE);
=                                                                     _(ASSIGN);
>=|<=|<|>                                                             _(RELOP);
==|!=                                                                 _(EQ_RELOP);
\+|-                                                                  _(PLUS_MINUS)
\*|\/                                                                 _(MUL_DIV)
[a-zA-Z][a-zA-Z0-9]*                                                  {yylval.str_value=strdup(yytext);return ID;}
0|[1-9][0-9]*                                                         {yylval.i_value=atoi(yytext);return NUM;}
\"([^\n\r\"\\]|\\[rnt"\\])+\"                                         {yylval.str_value=strdup(yytext);return STRING;}
{CR}|{LF}|{TAB}|{SPACE}                                               /* ignore */;
"//"[^\r\n]*[\r|\n|\r\n]?                                             /* ignore */;
.                                                                     _ERROR(yylineno);
%%


