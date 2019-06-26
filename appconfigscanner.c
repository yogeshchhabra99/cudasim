#include <stdio.h>
#include "appconfigscanner.h"
#include <stdlib.h>
#include<string.h>
#include "structs.h"
extern yylex();
extern int yylineno;
extern char* yytext;

char *names[]={
    "GRID_DIM_x",
    "GRID_DIM_y",
    "GRID_DIM_z",
    "BLOCK_DIM_x",
    "BLOCK_DIM_y",
    "BLOCK_DIM_z",
    "INTEGER",
};

void printConfig(CudaConfig config){
    printf("Grid Dimension X: %d\n",config.gridDimX);
    printf("Grid Dimension Y: %d\n",config.gridDimY);
    printf("Grid Dimension Z: %d\n",config.gridDimZ);
    printf("Block Dimension X: %d\n",config.blockDimX);
    printf("Block Dimension Y: %d\n",config.blockDimY);
    printf("Block Dimension Z: %d\n",config.blockDimZ);
    printf("Total Threads: %d\n", config.gridDimX*config.gridDimY*config.gridDimZ*config.blockDimX*config.blockDimY*config.blockDimZ);
}
int main(void){
    int ntoken, vtoken;
    CudaConfig config;
    ntoken=yylex();
    while(ntoken){
        printf("%d",yylineno);
        int token2 = yylex();
        int token3 = yylex();
        if (ntoken == UNEXPECTED){
            printf("Unexpected config option in the file\n");
            break;
        }
        if(token2!=COLON){
            printf("You forgot to insert a colon, found %d \n",token2);
            break;
        }
        if(token3!=INTEGER){
            printf("Config values must be integers\n");
            break;
        }
      //  printf("%d\n",atoi(yytext));
        switch(ntoken){
            case BLOCK_DIM_x:
                 config.blockDimX=atoi(yytext);
                 break;
            case BLOCK_DIM_y:
                 config.blockDimZ=atoi(yytext);
                 break;
            case BLOCK_DIM_z:
                 config.blockDimZ=atoi(yytext);
                 break;
            case GRID_DIM_x:
                 config.gridDimX=atoi(yytext);
                 break;
            case GRID_DIM_y:
                 config.gridDimY=atoi(yytext);
                 break;
            case GRID_DIM_z:
                 config.gridDimZ=atoi(yytext);
                 break;
            default:
                printf("unknown setting");
        }
        ntoken=yylex();
    }
    printConfig(config);
}

