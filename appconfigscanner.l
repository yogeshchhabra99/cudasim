%{
#include "appconfigscanner.h"
%}

%%
":" return COLON;
"GRID_DIM_x" return GRID_DIM_x;
"GRID_DIM_y" return GRID_DIM_y;
"GRID_DIM_z" return GRID_DIM_z;
"BLOCK_DIM_x" return BLOCK_DIM_x;
"BLOCK_DIM_y" return BLOCK_DIM_y; 
"BLOCK_DIM_z" return BLOCK_DIM_z;
[1-9][0-9]* return INTEGER;
[\t\n] ;
" " ;
. return UNEXPECTED;
%%

int yywrap(void)
{
    return 1; 
}