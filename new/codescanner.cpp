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
extern FILE *yyin, *yyout;

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

long long registers_max = 65536;
int warpholderspersm = 4;
int threadspersm = 2048;
int WLPeffect = (threadspersm / 32) / 4;
long long registers = 0;

class CodeBlock
{
public:
    int startLine;
    int endLine;
    Type type;
    int cycles;
    int NUMbytes;
    int tWeight;
    bool inLoop;
    CodeBlock()
    {
        inLoop = false;
        cycles = 0;
    }
};

class Loop
{
public:
    int startLine;
    int endLine;
    vector<Loop> childLoops;
    int cycles;
    int iterations;
    int startBlock;
    int endBlock;
    int memoryCycles;
    int otherCycles;
    int CYCcompute;
    int NUMmem;
    int NUMmemShared;
    int compnew;
    int NUMsync;
    long long totalmemcycles;
    long long totalmemSharedcycles;
	int type;

    Loop()
    {
        NUMsync = 0;
        iterations = 0;
        cycles = 0;
        NUMmem = 0;
        NUMmemShared = 0;
        CYCcompute = 0;
        compnew = 0;
    }

    bool operator>(Loop l)
    {
        return this->startLine > l.startLine;
    }
    bool operator<(Loop l)
    {
        return this->startLine < l.startLine;
    }

    bool operator>=(Loop l)
    {
        return this->startLine >= l.startLine;
    }
    bool operator<=(Loop l)
    {
        return this->startLine <= l.startLine;
    }
};

typedef struct mystruct4
{
    int lineNo;
    char *cp;
    string label;
} Instr;

vector<Instr> labels;
vector<Instr> branches;
vector<Loop> loops;

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
    int cycles;

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
    //   cout << "lineno" << lineNo << endl;
    int token = *nToken;

    Instruction inst;

    if (token == REG)
    {

        char *c = yytext;
        while (*c != '<')
            c++;
        c++;
        char a[10];
        int i = 0;
        while (*c != '>')
        {
            a[i] = *c;
            i++;
            c++;
        }
        a[i] = '\0';

        c = yytext;
        bool flag = false;
        //    printf("%s\n", c);
        while (*c < '0' || *c > '9')
        {
            c++;
            if (*c == '<')
            {
                flag = true;
                break;
            }
        }
        char b[10];
        if (flag)
        {
            b[0] = '3';
            b[1] = '2';
            b[2] = '\0';
        }
        else
        {
            i = 0;
            while (*c >= '0' && *c <= '9')
            {
                b[i] = *c;
                c++;
                i++;
            }
            b[i] = '\0';
        }

        registers += atoi(b) / 32 * atoi(a);

        int newToken = getToken();

//        cout << "reg size:" << atoi(b) << " number of reg=" << atoi(a) << endl;
        while (newToken != NEWLINE)
            newToken = getToken();
        inst.type = S;
        inst.cycles = 50;
    }
    else if (token == ADD)
    {
        int newToken = getToken();
        while (newToken != NEWLINE)
            newToken = getToken();
        inst.type = C;
        inst.cycles = 15;
    }
    else if (token == MUL)
    {
        int newToken = getToken();
        while (newToken != NEWLINE)
            newToken = getToken();
        inst.type = C;
        inst.cycles = 85;
    }
    else if (token == DIV)
    {
        int newToken = getToken();
        while (newToken != NEWLINE)
            newToken = getToken();
        inst.type = C;
        inst.cycles = 1039;
    }
    else if (token == REM)
    {
        int newToken = getToken();
        while (newToken != NEWLINE)
            newToken = getToken();
        inst.type = C;
        inst.cycles = 1017;
    }
    else if (token == ABS)
    {
        int newToken = getToken();
        while (newToken != NEWLINE)
            newToken = getToken();
        inst.type = C;
        inst.cycles = 30;
    }
    else if (token == SQRT)
    {
        int newToken = getToken();
        while (newToken != NEWLINE)
            newToken = getToken();
        inst.type = C;
        inst.cycles = 399;
    }
    else if (token == LOAD || token == STORE)
    {
        if (getToken() != DOT)
            printf("unexpected1 at line: %d\n", lineNo); //ignoring dot
        int newToken = getToken();

        //shared mem or local memory(stack) used for parameter and return values also called thread memory
        if (newToken == PARAM || newToken == SHARED || newToken == LOCAL)
        {
            inst.type = S;
            inst.cycles = 500;
            //          cout << "shared mem access at" << lineNo << endl;
        }
        else if (newToken == GLOBAL)
        {
            inst.type = M;
            inst.cycles = 1493; //assuming decreased by cache
        }
        else
            printf("unexpected2 at line: %d\n", lineNo);
        while (newToken != NEWLINE)
            newToken = getToken();
    }
    else if (token == COMP || token == CONVERT)
    {
        int newToken = getToken();
        while (newToken != NEWLINE)
            newToken = getToken();
        inst.type = C;
        inst.cycles = 15;
    }
    else if (token == NEWLINE)
    {
        inst.type = E;
        inst.cycles = 0;
    }
    else if (token == CURLYBEGIN || token == CURLYEND)
    {
        if (getToken() != NEWLINE)
        {
            cout << "unexpected3 at lineNo" << lineNo << endl;
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
        inst.cycles = 15; // not sure
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
            cout << "unexpected4 at " << lineNo << endl;
            inst.type = U;
        }
        else
        {
            inst.length = lines;
            //cout << "length" << inst.length << endl;
            inst.type = CF;
            inst.cycles = 15;
        }
    }
    else if (token == CONTROLFLOW)
    {
        Instr temp;
        temp.lineNo = lineNo;
        int newToken = getToken();
        while (newToken != BRANCH)
            newToken = getToken();
        int t = getToken();
        if (t != DIRECTIVE)
        {
            cout << "unexpected5 " << t << " at " << lineNo << endl;
            printf("%s\n", yytext);
        }
        temp.cp = yytext - 1; //donno why yytext is going 1 char ahead
        t = getToken();
        if (t != SEMICOLON)
        {
            cout << "unexpected6 " << t << " at " << lineNo << endl;
        }
        branches.push_back(temp);
        if (getToken() != NEWLINE)
        {
            cout << "unexpected10" << endl;
        }
        inst.type = CF;
        inst.cycles = 15; //not sure
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
            cout << "unexpected7";
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
            cout << "unexpected8";
            inst.type = U;
        }
        else
        {
            inst.type = CF;
            inst.cycles = 15;
        }
    }
    else if (token == BAR)
    {
        int newToken = getToken();
        while (newToken != NEWLINE)
            newToken = getToken();
        inst.type = B;
        inst.cycles = 15; // not sure
    }
    else
    {
        cout << "unexpexted 12 at" << lineNo << " token " << token << endl;
        while (token != NEWLINE)
        {
            //  cout << "test" << endl;
            token = getToken();
        }
        inst.cycles = 15;
        inst.type = U;
    }
    //  cout << " type " << type << endl;
    return inst;
}

class ResultCycles
{
public:
    long long totalCompcycles;
    long long totalmemcycles;
    long long totalmemSharedcycles;

    ResultCycles()
    {
        totalCompcycles = 0;
        totalmemcycles = 0;
        totalmemSharedcycles = 0;
    }
    ResultCycles(long long compC, long long memC, long long smemC)
    {
        totalCompcycles = compC;
        totalmemcycles = memC;
        totalmemSharedcycles = smemC;
    }
};

ResultCycles getCycles(vector<Loop> fLoops, vector<CodeBlock> blocks, vector<int>loopRunning, long long numberOfthreads, bool debug=false,float bytesperCycle= 1.8775,int loadLatency = 300,int sharedLoadLatency = 50)
{
    long long numberOfWarps = numberOfthreads / 32;
    long long newTotalmemcycles = 0;
    long long newTotalmemSharedcycles = 0;
    long long newTotalCompcycles = 0;

    long long totalCompCycles = 0;
    for (int k = 0; k < fLoops.size(); k++)
    {
        long long int NUMmemTotal = 0;
        long long int NUMmemSharedTotal = 0;
        for (int l = fLoops[k].startBlock; l <= fLoops[k].endBlock; l++)
        {
            if (blocks[l].type == M)
            {
                blocks[l].tWeight = (blocks[l].endLine - blocks[l].startLine) + 1;
                fLoops[k].CYCcompute += blocks[l].tWeight;
                fLoops[k].NUMmem += blocks[l].tWeight;
                blocks[l].NUMbytes = blocks[l].tWeight * 4;
            }
            else if (blocks[l].type == S)
            {
                blocks[l].tWeight = (blocks[l].endLine - blocks[l].startLine) + 1;
                fLoops[k].CYCcompute += blocks[l].tWeight;
                fLoops[k].NUMmemShared += blocks[l].tWeight;
                blocks[l].NUMbytes = blocks[l].tWeight * 4;
            }
            else if (blocks[l].type == B)
            {
                fLoops[k].NUMsync++;
            }
            else
            {
                blocks[l].tWeight = (blocks[l].endLine - blocks[l].startLine) + 1;
                fLoops[k].CYCcompute += blocks[l].tWeight * 1;
                blocks[l].NUMbytes = 0;
                fLoops[k].compnew += blocks[l].tWeight * 1;
            }
        }
        int CYCMemory = ((fLoops[k].NUMmem + fLoops[k].NUMmemShared) * 4 * 32) / bytesperCycle; //for a wrap

        int LaencyBW = 0;
        if ((fLoops[k].NUMmemShared + fLoops[k].NUMmem) > 0)
            LaencyBW = max(0, (CYCMemory - fLoops[k].CYCcompute) / (fLoops[k].NUMmemShared + fLoops[k].NUMmem)) + 1;
        float NBCavg = (float)fLoops[k].CYCcompute / (fLoops[k].NUMmem + fLoops[k].NUMmemShared + fLoops[k].NUMsync + 1);
        int LatencyExposed = loadLatency - (WLPeffect - 1) * NBCavg;
        int LatencyExposedShared = sharedLoadLatency; //- (WLPeffect - 1) * NBCavg;
	if(debug)
        cout << "nbc avg " << NBCavg << " cyc compute " << fLoops[k].CYCcompute << " num mem " << fLoops[k].NUMmem << " latency exxposed " << LatencyExposed << " latency exxposed shared " << LatencyExposedShared << endl;

        if ( fLoops[k].type == 1 )
        {
		if(debug){
            cout << "numMem:" << fLoops[k].NUMmem * loopRunning[k] / 4 << endl;
            cout << "numMemShared:" << fLoops[k].NUMmemShared * loopRunning[k] / 4 << endl;
            cout << " numcomp:" << ((fLoops[k].compnew) * (double)(loopRunning[k] / 4)) << endl;
		}
            NUMmemTotal = ((fLoops[k].NUMmem) * (double)(loopRunning[k] / 4) * (numberOfthreads / (20 * 32)));
            NUMmemSharedTotal = ((fLoops[k].NUMmemShared) * (double)(loopRunning[k] / 4) * (numberOfthreads / (20 * 32)));
            newTotalCompcycles += ((fLoops[k].compnew) * (double)(loopRunning[k] / 4) * (numberOfthreads / (20 * 32)));
		
        }
        else
        {
		if(debug){
            cout << "numMem:" << fLoops[k].NUMmem * loopRunning[k] << endl;
            cout << "numMemShared:" << fLoops[k].NUMmemShared * loopRunning[k] << endl;
            cout << " numcomp:" << ((fLoops[k].compnew) * (double)(loopRunning[k])) << endl;
		}
            NUMmemTotal = ((fLoops[k].NUMmem) * (double)(loopRunning[k]) * (numberOfthreads / (20 * 32)));
            NUMmemSharedTotal = ((fLoops[k].NUMmemShared) * (double)(loopRunning[k]) * (numberOfthreads / (20 * 32)));
            newTotalCompcycles += ((fLoops[k].compnew) * (double)(loopRunning[k]) * (numberOfthreads / (20 * 32)));
        }
        //long long int newmemorycycles = fLoops[k].NUMmem * LatencyExposed * (numberofwraps / 20) * 256; //* (1024 / 4);
        //memnew = newmemorycycles;
	
//        cout << "numMemTotal:" << NUMmemTotal << endl;
//        cout << "numMemSharedTotal:" << NUMmemSharedTotal << endl;

        fLoops[k].totalmemcycles = (NUMmemTotal / 32) * LatencyExposed + NUMmemTotal;
        fLoops[k].totalmemSharedcycles = (NUMmemSharedTotal / 32) * LatencyExposedShared + NUMmemSharedTotal;

        newTotalmemcycles += fLoops[k].totalmemcycles;
        newTotalmemSharedcycles += fLoops[k].totalmemSharedcycles;
    }

    return ResultCycles(newTotalCompcycles,
                        newTotalmemcycles,
                        newTotalmemSharedcycles);
}



int solve(long long numberOfthreads,float bytesperCycle= 1.8775,int loadLatency = 300,int sharedLoadLatency = 50)
{
	
	yyin= fopen("generatedptx.txt","r");
	//yyin= fopen("test.txt","r");

	bool debug=false;
	if(debug){
    		cout << "wlp effect" << WLPeffect << endl;
    printf("M=%d, S=%d, U=%d, C=%d, B=%d, D=%d E=%d CF=%d\n", M, S, U, C, B, D, E, CF);
	}
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
        //   printf("%d %d\n", lineNo - 1, inst.type);
        types.push_back(inst);
    }
    printf("------------------------\n");
    for (int i = 0; i < types.size(); i++)
    {
        //    printf("%d : %d\n", line1 + i, types[i].type);
        if (types[i].type == U)
        {
            cout << "unexpected11 at " << line1 + i << endl;
        }
    }

    //get codeblocks
    Type type = types[0].type;
    CodeBlock block;
    block.startLine = line1;
    block.endLine = line1;
    block.type = types[0].type;
    block.cycles = types[0].cycles;
    int surplus = 0;
    for (int i = 1; i < types.size(); i++)
    {
        if (types[i].type == block.type)
        {
            block.endLine += 1;
            block.cycles += types[i].cycles;
        }
        else
        {
            blocks.push_back(block);
            block.startLine = surplus + line1 + i;
            block.endLine = surplus + line1 + i + types[i].length - 1;
            block.type = types[i].type;
            block.cycles = types[i].cycles;
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
        //       printf("%d %d %d\n", blocks[i].startLine, blocks[i].endLine, blocks[i].type);
        //  }
    }

    //   cout << " branches" << endl;
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
        //       cout << branches[i].lineNo << branches[i].label << endl;
    }
    //    cout << " Labels" << endl;
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
        //      cout << labels[i].lineNo << labels[i].label << endl;
    }
    for (int i = 0; i < branches.size(); i++)
    {
        for (int j = 0; j < labels.size(); j++)
        {
            if (branches[i].label == labels[j].label)
            {
                if (branches[i].lineNo > labels[j].lineNo)
                {
			if(debug)
                    cout << "backward call at " << branches[i].lineNo << " to " << labels[j].lineNo << endl;
                    Loop loop;
                    loop.startLine = labels[j].lineNo + 1;
                    loop.endLine = branches[i].lineNo - 1;
                    loops.push_back(loop);
                }
            }
        }
    }

    sort(loops.begin(), loops.end());

    vector<Loop> fLoops;
    vector<Loop> nonLoops;

    for (int k = 0; k < loops.size() - 1; k++)
    {

        if (loops[k].endLine > loops[k + 1].startLine)
        {
            Loop l = loops[k];
            Loop c = loops[k + 1];
            l.childLoops.push_back(c);
            fLoops.push_back(l);
            k++;
        }
        else
        {
            Loop l = loops[k];
            fLoops.push_back(l);
        }
    }
    fLoops.push_back(loops[loops.size() - 1]);

    // vector<Loop> *nLoops = &fLoops;
    // cout << "size" << loops.size() << endl;
    // for (int k = 0; k < loops.size(); k++)
    // {
    //     if ((*nLoops).size() == 0)
    //     {
    //         //    cout << "debug10\n";
    //         (*nLoops).push_back(loops[k]);
    //         continue;
    //     }
    //     else
    //     {
    //         //    cout << "debug20\n";
    //         for (int i = 0; i < (*nLoops).size(); i++)
    //         {
    //             if (loops[k].startLine < (*nLoops)[i].startLine && loops[k].endLine > (*nLoops)[i].endLine)
    //             {
    //                 k--;
    //                 nLoops = &((*nLoops)[i].childLoops);
    //                 continue;
    //             }
    //         }
    //         (*nLoops).push_back(loops[k]);
    //         nLoops = &(fLoops);
    //     }
    // }

    //cout << "debug start" << loops[0].startLine << endl;
    for (int k = 0; k < fLoops.size(); k++)
    {
	int type=1;
        int l = 0;
        while (l < blocks.size() && blocks[l].startLine < fLoops[k].startLine)
        {
            l++;
        }
        fLoops[k].startBlock = l;
        blocks[l].inLoop = true;
        //        cout << "start block:" << fLoops[k].startBlock << endl;
        while (l < blocks.size() && blocks[l].endLine < fLoops[k].endLine)
        {
            l++;
            blocks[l].inLoop = true;
        }
        fLoops[k].endBlock = l;
	
		
	for(int i=fLoops[k].startBlock;i<=fLoops[k].endBlock;i++){
		if(blocks[i].type == S)	type=2;
	}
	fLoops[k].type=type;
	
    }
    //    cout << "debug" << endl;
    int t = 0;
    while (t != blocks.size())
    {
        while (blocks[t].inLoop == true)
        {
            t++;
            if (t >= blocks.size())
                break;
        }
        if (t >= blocks.size())
            break;
        Loop nonLoop;
        nonLoop.startBlock = t;
        while (blocks[t].inLoop == false)
        {
            t++;
            if (t >= blocks.size())
                break;
        }
        nonLoop.endBlock = t - 1;
        nonLoops.push_back(nonLoop);
    }
if(debug){
    cout << "Loops\n";
    for (int i = 0; i < fLoops.size(); i++)
    {
        cout << i + 1 << " start block=" << fLoops[i].startBlock << "  end block=" << fLoops[i].endBlock << endl;
    }
    cout << "Non Loops\n";
    for (int i = 0; i < nonLoops.size(); i++)
    {
        cout << i + 1 << " start block=" << nonLoops[i].startBlock << "  end block=" << nonLoops[i].endBlock << endl;
    }
}
    // cout << "debug1" << endl;
    long long int memnew = 0;
    long long compnew = 0;
    int memCycles = 0;
    int otherCycles = 0;
    int CYCcompute = 0;
    //for a wrap
    //long long loadLatency = 250;
    int WLPeffect = 20;
    long long LatencyExposed;
    long long LatencyExposedShared;
    int NUMsync = 0;
    float NBCavg;

    //int looptype[2] = {2, 1}; // 1 is unwrapped  2 is not
    //int loopRunning[2] = {32 * 2, 31};

    vector<int> loopRunning;
    vector<int> looptype;
	int x;
	cout<<"Note: If your kernal had nested loops and we have not shown here, then give the stat for outer loop only"<<endl;
	for(int i=0;i<fLoops.size();i++){
		cout<<"How many (expected)times does the loop 1 run"<<endl;
		cin>>x;
		loopRunning.push_back(x);	
	}
	
		
	
    //int loopRunning[1] = {1024 * 2};

    //long long numberOfthreads = threads;

    long long newTotalmemcycles = 0;
    long long newTotalmemSharedcycles = 0;
    long long newTotalCompcycles = 0;
    //640 see this
    long long totalMemoryCycles = 0;
    long long totalOtherCycles = 0;

	vector<int> temp;
	for(int i=0;i<nonLoops.size();i++){
		temp.push_back(1);
		nonLoops[i].type=2;
	}
    //int temp[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    //int temp2[] = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};

	
	
    ResultCycles res = getCycles(fLoops, blocks, loopRunning, numberOfthreads);
    newTotalCompcycles = res.totalCompcycles;
    newTotalmemcycles = res.totalmemcycles;
    newTotalmemSharedcycles = res.totalmemSharedcycles;

if(debug){
    cout << "new total mem cycles" << newTotalmemcycles << endl;
    cout << "new total mem cycles shared" << newTotalmemSharedcycles << endl;
    cout << "new total comp  cycles" << newTotalCompcycles << endl;
}
    res = getCycles(nonLoops, blocks, temp, numberOfthreads);

    newTotalCompcycles += res.totalCompcycles;
    newTotalmemcycles += res.totalmemcycles;
    newTotalmemSharedcycles += res.totalmemSharedcycles;

    cout << "total cycles" << newTotalCompcycles + newTotalmemcycles + newTotalmemSharedcycles << endl;
    cout << "time:" << (double)(newTotalCompcycles + newTotalmemcycles + newTotalmemSharedcycles) / 1493000000 << endl;
}
