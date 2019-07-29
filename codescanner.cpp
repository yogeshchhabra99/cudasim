#include <bits/stdc++.h>
#include "codescanner.h"
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include <vector>
#include <iostream>

using namespace std;

extern int yylex();
extern int yylineno;
extern char *yytext;

int lineNo = 1;

int getToken()
{
    int t = yylex();
    if (t == NEWLINE)
    {
        lineNo++;
        //return getToken();
    }
    //else
    return t;
}

class Loop
{
public:
    int start, end;
    Loop *child;
};

int main(void)
{
    int token = getToken();
}
