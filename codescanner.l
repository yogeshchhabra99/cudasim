%{
#include "codescanner.h"
%}

%%
"," return COMMA;
";" return SEMICOLON;
"{" return CURLYBEGIN;
"}" return CURLYEND;
"[" return SQUAREBEGIN;
"]" return SQUAREEND;
"(" return BRACKETBEGIN;
")" return BRACKETEND;
"for" return FORSTMT;
"if" return IFSTMT;
"int" return INTDECLARATION;
"float" return FLOATDECLARATION;
"else" return ELSESTMT;
"else if" return ELIFSTMT;
"while" return WHILESTMT;
[_a-z][_a-z0-9]* return VARNAME;
[1-9][0-9]* return INTEGER;
[\t] ;
[\n] return NEWLINE;
. return UNEXPECTED;
%%

int yywrap(void)
{
    return 1; 
}