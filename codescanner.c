#include <stdio.h>
#include "codescanner.h"
#include <stdlib.h>
#include<string.h>
#include"structs.h"
extern yylex();
extern int yylineno;
extern char* yytext; 


typedef struct mystruct3{
    int startLine;
    int endLine;
}CodeBlock;

typedef struct mystruct2{
    char* condition;
    int type;
    CodeBlock* codeBlock;
    struct mystruct2* next;
}Node;

int lineNo=1;
Node ** elsefun(Node** ptr);
Node ** eliffun(Node** ptr);
Node ** conditional(Node** ptr);

int getToken(){
    int t=yylex();
    if(t==NEWLINE){
        lineNo++;
        return getToken();
    }
    else
        return t; 
     
}
void printData(Node* head){
   // printf("printing data %p",head);
    while(head!=NULL){
        printf("Startline: %d EndLine: %d\n",head->codeBlock->startLine,head->codeBlock->endLine);
        head=head->next;
    }
}
void insert(Node* node, Node**head){
    if(head==NULL){
        *head=node;
    }
}

// Node* fun(){
//     int ntoken;
//     Node* head = NULL;
//     Node** ptr = &head;
//     while(1){
//         printf("loop1\n");
//         ntoken = getToken();
//         if(!ntoken)
//             return head;
//         CodeBlock* codeBlock = (CodeBlock*)malloc(1*sizeof(CodeBlock));
//         codeBlock->startLine = lineNo;
//         while(ntoken!=IFSTMT){
//             ntoken = getToken();
//             if(!ntoken){
//                 break;
//             }
//         }
//         codeBlock->endLine = lineNo;
//         getToken(); // ignoring the (
//         int brackets=1;
//         while(brackets!=0){
//             printf("loop2 brackets: %d token: %d\n",brackets,ntoken);
//             if(ntoken==BRACKETBEGIN){
//                 brackets++;
//                 printf("increasing\n");
//             }
//             else if(ntoken==BRACKETEND){
//                 brackets--;
//                 printf("decreasing\n");
//             }   
//             else if(!ntoken)
//                 break;
//             ntoken=getToken();
//         }
//         Node* node = (Node*) malloc(1*sizeof(Node));
//         node->next = (Node**) malloc(1*sizeof(Node*));
//         node->codeBlock=codeBlock;
//         node->children=1;
//         *ptr = node;
//         ptr = node->next;
//         *ptr=NULL;   
//     }
//     return head;
// }


Node** conditional(Node** ptr){
    printf("conditional\n");
    getToken(); // ignoring the (
    int brackets=1;
    int ntoken=getToken();
    while(brackets!=0){
      //  printf("loop2 brackets: %d token: %d\n",brackets,ntoken);
        if(ntoken==BRACKETBEGIN){
            brackets++;
    //        printf("increasing\n");
        }
        else if(ntoken==BRACKETEND){
            brackets--;
    //        printf("decreasing\n");
        }   
        else if(!ntoken)
            break;
        ntoken=getToken();
    }

    CodeBlock* codeBlock = (CodeBlock*)malloc(1*sizeof(CodeBlock));
    codeBlock->startLine = lineNo;
    if(ntoken==IFSTMT)
        ptr=conditional(ptr);
    if(ntoken!=CURLYBEGIN){
        
    }
    else{
    //    printf("found curlybegin");
        while(ntoken!=CURLYEND){
       //     printf("looking for curly end");
            ntoken=getToken();
            if(ntoken==IFSTMT)
               ptr= conditional(ptr);
        }
    }
    codeBlock->endLine=lineNo;
                Node* node = (Node*) malloc(1*sizeof(Node));
                node->codeBlock=codeBlock;
                node->type=IFSTMT;
                *ptr = node;
                ptr =&(node->next) ;
                *ptr=NULL;
        return ptr;
}
Node** elsefun(Node ** ptr){
    printf("else fun\n");
    getToken(); // ignoring the (
    int brackets=1;
    int ntoken=getToken();
    CodeBlock* codeBlock = (CodeBlock*)malloc(1*sizeof(CodeBlock));
    codeBlock->startLine = lineNo;
    if(ntoken==IFSTMT)
       ptr= conditional(ptr);
    if(ntoken==ELSESTMT){
                ptr=elsefun(ptr);
            }
            if(ntoken==ELIFSTMT){
                ptr=eliffun(ptr);
            }
    if(ntoken!=CURLYBEGIN){
        
    }
    else{
    //    printf("found curlybegin");
        while(ntoken!=CURLYEND){
       //     printf("looking for curly end");
            ntoken=getToken();
            if(ntoken==IFSTMT)
                ptr=conditional(ptr);
            if(ntoken==ELSESTMT){
                ptr=elsefun(ptr);
            }
            if(ntoken==ELIFSTMT){
                ptr=eliffun(ptr);
            }
        }
    }
    codeBlock->endLine=lineNo;
                Node* node = (Node*) malloc(1*sizeof(Node));
                node->codeBlock=codeBlock;
                node->type=ELSESTMT;
                *ptr = node;
                ptr =&(node->next);
                *ptr=NULL;
        return ptr;
}
Node** eliffun(Node ** ptr){
    printf("elif fun\n");
    getToken(); // ignoring the (
    int brackets=1;
    int ntoken=getToken();
    while(brackets!=0){
      //  printf("loop2 brackets: %d token: %d\n",brackets,ntoken);
        if(ntoken==BRACKETBEGIN){
            brackets++;
        //    printf("increasing\n");
        }
        else if(ntoken==BRACKETEND){
            brackets--;
        //    printf("decreasing\n");
        }   
        else if(!ntoken)
            break;
        ntoken=getToken();
    }

    CodeBlock* codeBlock = (CodeBlock*)malloc(1*sizeof(CodeBlock));
    codeBlock->startLine = lineNo;
    if(ntoken==IFSTMT)
       ptr= conditional(ptr);
    if(ntoken!=CURLYBEGIN){
        
    }
    else{
      //  printf("found curlybegin");
        while(ntoken!=CURLYEND){
       //     printf("looking for curly end");
            ntoken=getToken();
            if(ntoken==IFSTMT)
                ptr=conditional(ptr);
            if(ntoken==ELSESTMT){
                ptr=elsefun(ptr);
            }
            if(ntoken==ELIFSTMT){
                ptr=eliffun(ptr);
            }
        }
    }
    codeBlock->endLine=lineNo;
                Node* node = (Node*) malloc(1*sizeof(Node));
                node->codeBlock=codeBlock;
                node->type=ELIFSTMT;
                *ptr = node;
                ptr =&(node->next) ;
                *ptr=NULL;
        return ptr;
}

Node* fun2(){
    int ntoken=1;
    Node* head = NULL;
    Node** ptr = &head;
    while(1){
        printf("=====new block\n");
        if(!ntoken){
            break;
        }
        CodeBlock* codeBlock = (CodeBlock*)malloc(1*sizeof(CodeBlock));
        codeBlock->startLine = lineNo;
        while(1){
           // printf("loop3\n");
            ntoken=getToken();
            if(ntoken==0){
            //    if(lineNo==codeBlock->startLine)
            //        break;
                codeBlock->endLine=lineNo;
                Node* node = (Node*) malloc(1*sizeof(Node));
                node->codeBlock=codeBlock;
                node->type=0;
                *ptr = node;
                ptr =&(node->next);
                *ptr=NULL;
                break;
            }
            if(ntoken==ELSESTMT){
                 codeBlock->endLine=lineNo;
                Node* node = (Node*) malloc(1*sizeof(Node));
                node->codeBlock=codeBlock;
                node->type=0;
                *ptr = node;
                ptr =&(node->next);
                *ptr=NULL;
                ptr=elsefun(ptr);
                break;
            }
            if(ntoken==ELIFSTMT){
                 codeBlock->endLine=lineNo;
                Node* node = (Node*) malloc(1*sizeof(Node));
                node->codeBlock=codeBlock;
                node->type=0;  
                *ptr = node;
                ptr =&(node->next);
                *ptr=NULL;
                ptr=eliffun(ptr);
                break;
            }
            
                
            if(ntoken==IFSTMT){
                codeBlock->endLine=lineNo;
                Node* node = (Node*) malloc(1*sizeof(Node));
                node->codeBlock=codeBlock;
                node->type=0;
                *ptr = node;
                ptr =&(node->next);
                *ptr=NULL;
                ptr=conditional(ptr);
                break;
            }
            else{
                //do nothin here just line is added
            }
        }
    }
    return head;
}


int main(void){
    Node * head = fun2();
    printData(head);
}

