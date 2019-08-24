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
" " ;
"%" return PERCENT;
"<" return LESSTHAN;
">" return GREATERTHAN;
"32" return THIRTYTWO;
"64" return SIXTYFOUR; 
"entry" return ENTRY;
"param" return PARAM;
"local" return LOCAL;
".reg" return REG;
"mov" return MOV;   //slightly like addi
"ld" return LOAD;
"st" return STORE;
"cvta" return CONVERT; //convert address to global, generic, local etc. 
"cvt" return CONVERT; //convert address to global, generic, local etc. 
"shared" return SHARED;
"global" return GLOBAL;

"add" return ADD;
"sub" return ADD;
"addc" return ADD;
"subc" return ADD;
"mul" return MUL;
"mul24" return MUL;
"mad24" return MUL;
"mad" return MUL;
"sad" return ADD;
"div" return DIV;
"rem" return REM;
"abs" return ABS;
"neg" return COMP;
"min" return ADD;
"max" return ADD;

"fma" return COMP; //float
"rcp" return COMP; //float
"sqrt" return SQRT; //float
"sin" return COMP; //float
"cos" return COMP; //float
"lg2" return COMP; //float
"ex2" return COMP; //float

"set" return COMP; //comparison and selection
"setp" return COMP; //comparison and selection
"selp" return COMP; //comparison and selection
"slct" return COMP; //comparison and selection

"and" return COMP; //LOGICAL SHIFT
"or" return COMP; //LOGICAL SHIFT
"xor" return COMP; //LOGICAL SHIFT
"not" return COMP; //LOGICAL SHIFT
"cnot" return COMP; //LOGICAL SHIFT
"shl" return COMP; //LOGICAL SHIFT
"shr" return COMP; //LOGICAL SHIFT

"bra" return BRANCH; //branch target and lose PC
"call" return CALL; //branch and store PC
"ret" return RET;
"exit" return EXIT;
"bar"   return BAR; //barrier sync
"membar" return MEMBAR; //todo parrallel sync and communication instructions


":" return COLON;
"." return DOT;
"@%"[a-zA-Z0-9] return CONTROLFLOW;

[A-Z][_A-Z0-9]* return DIRECTIVE;
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

