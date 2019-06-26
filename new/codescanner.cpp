#include <stdio.h>
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

enum Type
{
    M, // long latency/global memory operation
    S, // low latency memory/scratchpad operation
    B, // Barrier Synchronization
    C, // Computational instruction bloxk
    U,
    D,  //declaration
    CF, //control flow
    E,  //EMPTY

};

typedef struct mystruct3
{
    int startLine;
    int endLine;
    Type type;

} CodeBlock;

typedef struct mystruct4
{
    int lineNo;
    char *cp;
    string label;
} Instr;

vector<Instr> labels;
vector<Instr> branches;
vector<CodeBlock> loops;

typedef struct mystruct2
{
    char *condition;
    int type;
    CodeBlock *codeBlock;
    struct mystruct2 *next;
} Node;

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

class Instruction
{
public:
    int length;
    Type type;

    Instruction(int _length, Type _type)
    {
        length = _length;
        type = _type;
    }
    Instruction()
    {
        length = 1;
    }
};

Instruction getType(int *nToken)
{
    // cout << "line " << lineNo << " token " << *nToken;
    // #define PARAM 16
    // #define LOCAL 17
    // #define REG 18
    // #define MOV 19
    // #define CONVERT 20
    // #define MEMOP 21
    // #define LOAD 22
    // #define COMP 23
    // #define Store 24
    // #define CONTROLFLOW 25
    // #define BRANCH 26
    // #define CALL 27
    // #define RET 28
    // #define EXIT 29
    // #define BAR 30
    // #define MEMBAR 31
    // #define DOT 32
    // #define STORE 33
    // #define SHARED 34
    // #define GLOBAL 35

    int token = *nToken;

    Instruction inst;
    if (token == LOAD || token == STORE)
    {
        if (getToken() != DOT)
            printf("unexpected at line: %d\n", lineNo); //ignoring dot
        int newToken = getToken();

        //shared mem or local memory(stack) used for parameter and return values also called thread memory
        if (newToken == PARAM || newToken == SHARED || newToken == LOCAL)
            inst.type = S;
        else if (newToken == GLOBAL)
            inst.type = M;
        else
            printf("unexpected at line: %d\n", lineNo);
        while (newToken != NEWLINE)
            newToken = getToken();
    }
    else if (token == COMP || token == CONVERT)
    {
        int newToken = getToken();
        while (newToken != NEWLINE)
            newToken = getToken();
        inst.type = C;
    }
    else if (token == NEWLINE)
    {
        inst.type = E;
    }
    else if (token == CURLYBEGIN || token == CURLYEND)
    {
        if (getToken() != NEWLINE)
        {
            cout << "unexpected" << endl;
            inst.type = U;
        }
        else
        {
            inst.type = E;
        }
    }
    else if (token == DOT || token == MOV)
    {
        int newToken = getToken();
        while (newToken != NEWLINE)
            newToken = getToken();
        inst.type = S;
    }
    else if (token == CALL)
    {
        int lines = 1;
        int newToken = getToken();
        while (newToken != SEMICOLON)
        {
            newToken = getToken();
            if (newToken == NEWLINE)
                lines++;
        }
        if (getToken() != NEWLINE)
        {
            cout << "unexpected" << endl;
            inst.type = U;
        }
        else
        {
            inst.length = lines;
            //cout << "length" << inst.length << endl;
            inst.type = CF;
        }
    }
    else if (token == CONTROLFLOW)
    {
        Instr temp;
        temp.lineNo = lineNo;
        int newToken = getToken();
        while (newToken != BRANCH)
            newToken = getToken();
        if (getToken() != DIRECTIVE)
        {
            cout << "unexpected" << endl;
        }
        temp.cp = yytext;
        if (getToken() != SEMICOLON)
        {
            cout << "unexpected" << endl;
        }
        branches.push_back(temp);

        inst.type = CF;
    }
    else if (token == BRACKETBEGIN)
    { //call
        int newToken = getToken();
        while (newToken != BRACKETEND)
            newToken = getToken();
        inst.type = U;
    }
    else if (token == DIRECTIVE)
    {
        Instr temp;
        temp.lineNo = lineNo;
        temp.cp = yytext;
        labels.push_back(temp);
        int newToken = getToken();
        if (newToken != COLON || getToken() != NEWLINE)
        {
            cout << "unexpected";
            inst.type = U;
        }
        else
        {
            inst.type = CF;
        }
    }
    else if (token == RET)
    {
        int newToken = getToken();
        if (newToken != SEMICOLON || getToken() != NEWLINE)
        {
            cout << "unexpected";
            inst.type = U;
        }
        else
        {
            inst.type = CF;
        }
    }
    else
    {
        while (token != NEWLINE)
        {
            //  cout << "test" << endl;
            token = getToken();
        }
        inst.type = U;
    }
    //  cout << " type " << type << endl;
    return inst;
}

int main(void)
{
    printf("M=%d, S=%d, U=%d, C=%d, B=%d, D=%d E=%d CF=%d\n", M, S, U, C, B, D, E, CF);
    vector<CodeBlock> blocks;
    vector<Instruction> types;

    int nToken = getToken();
    while (nToken != ENTRY)
        nToken = getToken();
    while (nToken != CURLYBEGIN)
        nToken = getToken();
    //printf("%d", lineNo);
    while (getToken() != NEWLINE)
    {
    }
    int curly = 1;
    int line1 = lineNo;
    while (true)
    {
        nToken = getToken();
        //   cout << "nToken" << nToken << endl;
        // if (!nToken)
        //     break;
        if (nToken == CURLYBEGIN)
            curly++;
        if (nToken == CURLYEND)
            curly--;
        if (curly == 0)
            break;
        Instruction inst = getType(&nToken);
        //    printf("%d\n", type);
        types.push_back(inst);
    }
    printf("------------------------\n");
    for (int i = 0; i < types.size(); i++)
    {
        //    printf("%d : %d\n", line1 + i, types[i]);
        if (types[i].type == U)
        {
            cout << "unexpected" << endl;
        }
    }

    //get codeblocks
    Type type = types[0].type;
    CodeBlock block;
    block.startLine = line1;
    block.endLine = line1;
    block.type = types[0].type;
    int surplus = 0;
    for (int i = 1; i < types.size(); i++)
    {
        if (types[i].type == block.type)
        {
            block.endLine += 1;
        }
        else
        {
            blocks.push_back(block);
            block.startLine = surplus + line1 + i;
            block.endLine = surplus + line1 + i + types[i].length - 1;
            block.type = types[i].type;
            if (types[i].length != 1)
            {
                surplus += (types[i].length - 1);
            }
        }
    }
    blocks.push_back(block);
    for (int i = 0; i < blocks.size(); i++)
    {
        //printf("%d %d %d\n", blocks[i].startLine, blocks[i].endLine, blocks[i].type);
        //  if (blocks[i].type == CF)
        //  {
        printf("%d %d %d\n", blocks[i].startLine, blocks[i].endLine, blocks[i].type);
        //  }
    }

    cout << " branches" << endl;
    for (int i = 0; i < branches.size(); i++)
    {
        char *ptr = branches[i].cp;
        ptr++; //ignoring space
        char label[100];
        int j = 0;
        while (ptr[j] != ';')
        {
            label[j] = ptr[j];
            j++;
        }
        label[j] = '\0';
        branches[i].label = string(label);
        cout << branches[i].lineNo << branches[i].label << endl;
    }
    cout << " Labels" << endl;
    for (int i = 0; i < labels.size(); i++)
    {
        char *ptr = labels[i].cp;
        char label[100];
        int j = 0;
        while (ptr[j] != ':')
        {
            label[j] = ptr[j];
            j++;
        }
        label[j] = '\0';
        labels[i].label = string(label);
        cout << labels[i].lineNo << labels[i].label << endl;
    }
    for (int i = 0; i < branches.size(); i++)
    {
        for (int j = 0; j < labels.size(); j++)
        {
            if (branches[i].label == labels[j].label)
            {
                if (branches[i].lineNo > labels[i].lineNo)
                {
                    cout << "backward call at " << branches[i].lineNo << " to " << labels[j].lineNo << endl;
                    CodeBlock loop;
                    loop.startLine = labels[i].lineNo + 1;
                    loop.endLine = branches[i].lineNo - 1;
                    loops.add()
                }
            }
        }
    }
}
