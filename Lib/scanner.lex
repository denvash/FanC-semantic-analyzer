%{
#include "parser.tab.hpp"
#include "parser.hpp"
#include "output.hpp"
#include "types.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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

void                                                                  {yylval.e_type=TYPE_VOID;return VOID;}
int                                                                   {yylval.e_type=TYPE_INT;return INT;}
byte                                                                  {yylval.e_type=TYPE_BYTE;return BYTE;}
b                                                                     {yylval.e_type=TYPE_B;return B;}
bool                                                                  {yylval.e_type=TYPE_BOOL;return BOOL;}
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
[a-zA-Z][a-zA-Z0-9]*                                                  {yylval.str_value=yytext;return ID;}
0|[1-9][0-9]*                                                         {yylval.i_value=atoi(yytext);return NUM;}
\"([^\n\r\"\\]|\\[rnt"\\])+\"                                         {yylval.str_value=yytext;return STRING;}
{CR}|{LF}|{TAB}|{SPACE}                                               /* ignore */;
"//"[^\r\n]*[\r|\n|\r\n]?                                             /* ignore */;
.                                                                     _ERROR(yylineno);
%%


