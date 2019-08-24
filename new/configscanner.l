%{
#include "configscanner.h"
%}

%%
":" return COLON;

"NUMTHREADS" return NUMTHREADS;
"BYTESPERCYCLE" return BYTESPERCYCLE;
"LOADLATENCY" return LOADLATENCY;
"SHAREDLOADLATENCY" return SHAREDLOADLATENCY;

[0-9][0-9]*"."[0-9][0-9]* return FLOAT;
[1-9][0-9]* return INTEGER;
[\t\n] ;
" " ;
. return UNEXPECTED;
%%

int yywrap(void)
{
    return 1; 
}
