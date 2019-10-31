#include <bits/stdc++.h>
#include "codescanner.h"
#include "structs.h"

extern int yylex();
extern int yylineno;
extern char* yytext;
extern FILE *yyin, *yyout;

using namespace std;
CudaConfig getConfig();



long long getLong(){
	char *c = yytext;
	//printf("string %s\n",c);
        char a[25];
        int i = 0;
        while (*c != '\n')
        {
            a[i] = *c;
            i++;
            c++;
        }
        a[i] = '\0';
	return atoi(a);
}

float getFloat(){
	long double d=0;
	long div=1;
	char *c = yytext;
	bool flag=false;
    while (*c != '\n'){
		if(*c!='.'){
			d +=(int)( *c - '0');
			d*=10;
			c++;
		}
		else{
			
			flag=true;
			c++;
			//continue;		
		}
		if(flag)
			div*=10;
    }
        return (float)(d/div);

}

CudaConfig getConfig(){
	//cout<<"1"<<endl;
	yyin= fopen("config.txt","r");
    int ntoken, vtoken;
    CudaConfig config;
    ntoken=yylex();
    while(ntoken){
	cout<<"2"<<endl;
        
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
        if(token3!=INTEGER && token3!=FLOAT){
            printf("Config values must be integers\n");
            break;
        }
      //  printf("%d\n",atoi(yytext));
        switch(ntoken){
            case NUMTHREADS:
		//printf("%s",yytext);
				config.numthreads=getLong();
				break;
			case BYTESPERCYCLE:
				config.bytesPerCycle =getFloat();
				break;
			case LOADLATENCY:
				config.loadLatency=(int)getLong();
				break;
			case SHAREDLOADLATENCY:
				config.sharedLoadLatency=(int)getLong();
				break;
            default:
                printf("unknown setting");
				break;
        }
		printf("%d %d %d %d\n",yylineno,ntoken,token2,token3);
        ntoken=yylex();
    }
	return config;
    
}

//int main(){
	//cout<<"debug1"<<endl;
	//printConfig(getConfig());

//}





















