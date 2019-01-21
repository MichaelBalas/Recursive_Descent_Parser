// Michael Balas
/* A recursive descent parser in C for simple arithemic
expressions with a separate scanner called NextToken().
This version builds a parse tree. The function TraverseTree()
traverses the tree and displays the program represented
by the tree.
*/
//  Grammar
//  num is an unsigned decimal number with at most 4
//      digits after . , at most 9 significant digits
//      and no leading zeroes
//  var is an identifier of length at most 4, the
//      first character is a letter (A to Z, a to z)
//      or underscore and the followup characters may
//      be letters or underscore or digits (0 to 9)

//  prog: sts expr | sts
//  sts: st sts | epsilon
//  st: var = expr ;
//  expr: - num | - var | term expr1
//  expr1: + term expr1 | - term expr1 | epsilon
//  term: factor term1
//  term1: * factor term1 | / factor term1 | epsilon
//  factor: num | var | ( expr )


extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
}

typedef struct STRUCT_NODE
{
    int type;
        STRUCT_NODE *ch1, *ch2, *ch3;
            double num_value;
                char* str_value;
} NODE;

int Parse(char* s,NODE** tree);
int prog(char* s,NODE** tree);
int sts(char* s,NODE** tree);
int st(char* s,NODE** tree);
int expr(char* s,NODE** tree);
int expr1(char* s,NODE** tree);
int term(char* s,NODE** tree);
int term1(char* s,NODE** tree);
int factor(char* s,NODE** tree);
int NextToken(char* s);
NODE* mknode(int type);
void Delete(NODE** tree);
void Delete1(NODE* t);
char* mkstring(char* s);
void TraverseTree(NODE* t);

#define isdigit(x) ((x)>='0'&&(x)<='9')
#define isfirst(x) (((x)>='A'&&(x)<='Z')|((x)>='a'&&(x)<='z')|((x)=='_'))
#define isnext(x)  (isfirst(x)|((x)>='0'&&(x)<='9'))
#define isws(x) ((x)==' '||(x)=='\n'||(x)=='\t')
#define SavePosition() spp1 = spp
#define RestorePosition() spp = spp1;


int spp;
double token_fval;
char token_var[6];



// define token names and node types

#define EOI      0
#define var      256
#define num      var+1
#define ERR      num+1
#define _prog     ERR+1
#define _sts     _prog+1
#define _st     _sts+1
#define _expr     _st+1
#define _uminus _expr+1
#define _expr1     _uminus+1
#define _term     _expr1+1
#define _term1     _term+1
#define _factor    _term1+1

// function Parse ---------------------------------------------
int Parse(char* s,NODE** tree) {
    int token, spp2;
        
        spp = 0;
            if(!prog(s,tree)) {
                Delete(tree);
                return 0;
                    }
                        
                        spp2 = spp;
                            token = NextToken(s);
                            if (token == ERR) {
                                printf("Error occured at position %d\n",spp2);
                                exit(1);
                                    }
                                        if (token != EOI) {
                                            printf("Reduntant stuff at position %d\n",spp2);
                                            exit(1);
                                            }
                                                return 1;
                                                    }// end Parse



// prog: sts expr | sts
// function prog ---------------------------------------------
int prog(char* s,NODE** tree) {
    int spp1, spp2;
        NODE *ret1=NULL,*ret2=NULL,*ret=NULL;
            
            SavePosition();

            if (!sts(s,&ret1)) {
                RestorePosition();
                Delete(&ret);
                return 0;
                    }
                        spp2 = spp;
                            if (!expr(s,&ret2)) {
                                spp = spp2;
                                    Delete(&ret2);
                                    *tree = ret1;
                                        return 1;
                                            }else{
                                                ret = mknode(_prog);
                                                ret->ch1=ret1;
                                                    ret->ch2=ret2;
                                                        *tree = ret;
                                                            return 1;
                                                            }
}// end prog




// sts: st sts | epsilon
// function sts ---------------------------------------------
int sts(char* s,NODE** tree) {
    int spp1;
        NODE *ret1=NULL,*ret2=NULL,*ret=NULL;
            
            SavePosition();

            if (!st(s,&ret1)) {
                RestorePosition();
                Delete(&ret1);
                *tree = NULL;
                    return 1; // epsilon
                        }
                            if (!sts(s,&ret2)) {
                                RestorePosition();
                                Delete(&ret1);
                                Delete(&ret2);
                                *tree = NULL;
                                    return 1;  // epsilon
                                        }
                                            ret = mknode(_sts);
                                            ret->ch1 = ret1;
                                                ret->ch2 = ret2;
                                                    *tree = ret;
                                                        return 1;
}// end sts



// st: var = expr ;
// function st ----------------------------------------------
int st(char* s,NODE** tree) {
    int spp1, spp2;
        NODE *ret1=NULL,*ret2=NULL,*ret=NULL;
            
            SavePosition();

            if (NextToken(s) != var) {
                RestorePosition();
                return 0;
                    }
                        ret1 = mknode(var);
                        ret1->str_value = mkstring(token_var);
                    
                        if (NextToken(s) != '=') {
                            RestorePosition();
                            return 0;
                                }
                                    if (!expr(s,&ret2)) {
                                        RestorePosition();
                                        Delete(&ret1);
                                        Delete(&ret2);
                                        return 0;
                                            }
                                                spp2 = spp;
                                                    if (NextToken(s) != ';') {
                                                        printf("Expected token ; at position %d\n",spp2);
                                                        exit(1);
                                                            }
                                                                ret = mknode(_st);
                                                                ret->ch1 = ret1;
                                                                    ret->ch2 = ret2;
                                                                        *tree=ret;
                                                                            return 1;
                                                                                }// end st



// expr: - num | - var | term expr1
// function expr ---------------------------------------------
int expr(char* s,NODE** tree) {
    int spp1, spp2, token;
        NODE *ret1=NULL,*ret2=NULL,*ret=NULL;
            
            SavePosition();
            token = NextToken(s);
            if (token=='-') {
                spp2 = spp;
                    token = NextToken(s);
                    if (token == var) {
                        ret1=mknode(_uminus);
                        ret2=mknode(var);
                        ret2->str_value=mkstring(token_var);
                        ret=mknode(_expr);
                        ret->ch1 = ret1;
                            ret->ch2 = ret2;
                                *tree=ret;
                                    return 1;
                                        }
                                            if (token == num) {
                                                ret1=mknode(_uminus);
                                                ret2=mknode(num);
                                                ret2->num_value=token_fval;
                                                    ret=mknode(_expr);
                                                    ret->ch1 = ret1;
                                                        ret->ch2 = ret2;
                                                            *tree=ret;
                                                                return 1;
                                                                    }
                                                                        printf("Expected token num or var at position %d\n",spp2);
                                                                        exit(1);
                                                                            }
                                                                                // hence it must be the third case: term expr1
                                                                                    // but we must start from the beginning
                                                                                        RestorePosition();
                                                                                        if (!term(s,&ret1)) {
                                                                                            RestorePosition();
                                                                                            Delete(&ret1);
                                                                                            return 0;
                                                                                                }
                                                                                                    if (!expr1(s,&ret2)) {
                                                                                                        RestorePosition();
                                                                                                        Delete(&ret1);
                                                                                                        Delete(&ret2);
                                                                                                        return 0;
                                                                                                            }   
                                                                                                                ret=mknode(_expr);
                                                                                                                ret->ch1=ret1;
                                                                                                                    ret->ch2=ret2;
                                                                                                                        *tree = ret;
                                                                                                                            return 1;
}// end expr


// expr1: + term expr1 | - term expr1 | epsilon
// function expr1 -------------------------------------------
int expr1(char* s,NODE** tree) {
    int token, spp1;
        NODE *ret1=NULL,*ret2=NULL,*ret=NULL;
            
            SavePosition();

            token = NextToken(s);
                if (token == '+') {
                    if (!term(s,&ret1)) {
                        RestorePosition();
                        Delete(&ret1);
                        *tree = NULL;
                            return 1;            // epsilon
                                }
                                    if (!expr1(s,&ret2)) {
                                        RestorePosition();
                                        Delete(&ret1);
                                        Delete(&ret2);
                                        *tree = NULL;
                                            return 1;            // epsilon
                                                }
                                                    ret=mknode(_expr1);
                                                    ret->ch1=mknode('+');
                                                    ret->ch2=ret1;
                                                        ret->ch3=ret2;
                                                            *tree = ret;
                                                                return 1;
                                                                    }
                                                                        if (token == '-') {
                                                                            if (!term(s,&ret1)) {
                                                                                RestorePosition();
                                                                                Delete(&ret1);
                                                                                *tree = NULL;
                                                                                    return 1;            // epsilon
                                                                                        }
                                                                                            if (!expr1(s,&ret2)) {
                                                                                                RestorePosition();
                                                                                                Delete(&ret1);
                                                                                                Delete(&ret2);
                                                                                                *tree = NULL;
                                                                                                    return 1;            // epsilon
                                                                                                        }
                                                                                                            ret=mknode(_expr1);
                                                                                                            ret->ch1=mknode('-');
                                                                                                            ret->ch2=ret1;
                                                                                                                ret->ch3=ret2;
                                                                                                                    *tree = ret;
                                                                                                                        return 1;
                                                                                                                            }
                                                                                                                                
                                                                                                                                RestorePosition();
                                                                                                                                *tree=NULL;
                                                                                                                                    return 1;  //epsilon
}// end expr1



// term: factor term1
// function term -------------------------------------------
int term(char* s,NODE** tree) {
    int  spp1;
        NODE *ret1=NULL,*ret2=NULL,*ret=NULL;
            
            SavePosition();

            if (!factor(s,&ret1)) {
                RestorePosition();
                Delete(&ret1);
                return 0;
                    }
                        
                        if (!term1(s,&ret2)) {
                            RestorePosition();
                            Delete(&ret1);
                            Delete(&ret2);
                            return 0;
                                }
                                    ret=mknode(_term);
                                    ret->ch1=ret1;
                                        ret->ch2=ret2;
                                            *tree=ret;
                                                return 1;

}// end term

// term1: * factor term1 | / factor term1 | epsilon
// function term1 -------------------------------------------
int term1(char* s,NODE** tree) {
    int spp1, token;
        NODE *ret1=NULL,*ret2=NULL,*ret=NULL;
            
            SavePosition();

            token = NextToken(s);
                
                if (token == '*') {
                    if (!factor(s,&ret1)) {
                        RestorePosition();
                        Delete(&ret1);
                        *tree=NULL;
                            return 1;                // epsilon
                                }
                                    if (!term1(s,&ret2)) {
                                        RestorePosition();
                                        Delete(&ret1);
                                        Delete(&ret2);
                                        *tree=NULL;
                                            return 1;                // epsilon
                                                }
                                                    ret=mknode(_term1);
                                                    ret->ch1=mknode('*');
                                                    ret->ch2=ret1;
                                                        ret->ch3=ret2;
                                                            *tree=ret;
                                                                return 1;
                                                                    }
                                                                        if (token == '/') {
                                                                            if (!factor(s,&ret1)) {
                                                                                RestorePosition();
                                                                                Delete(&ret1);
                                                                                *tree=NULL;
                                                                                    return 1;                // epsilon
                                                                                        }
                                                                                            if (!term1(s,&ret2)) {
                                                                                                RestorePosition();
                                                                                                Delete(&ret1);
                                                                                                Delete(&ret2);
                                                                                                *tree=NULL;
                                                                                                    return 1;                // epsilon
                                                                                                        }
                                                                                                            ret=mknode(_term1);
                                                                                                            ret->ch1=mknode('/');
                                                                                                            ret->ch2=ret1;
                                                                                                                ret->ch3=ret2;
                                                                                                                    *tree=ret;
                                                                                                                        return 1;
                                                                                                                            }
                                                                                                                                RestorePosition();
                                                                                                                                return 1;  // epsilon
                                                                                                                                    }// end term1



// factor: num | var | ( expr )
// function factor -----------------------------------------
int factor(char* s,NODE** tree) {
    int  spp1,spp2,token;
        NODE *ret1=NULL,*ret=NULL;
            
            SavePosition();

            token = NextToken(s);
                
                if (token == num) {
                    ret=mknode(_factor);
                    ret1=mknode(num);
                    ret1->num_value=token_fval;
                        ret->ch1=ret1;
                            *tree=ret;
                                return 1;
                                    }
                                        if (token == var) {
                                            ret=mknode(_factor);
                                            ret1=mknode(var);
                                            ret1->str_value=mkstring(token_var);
                                            ret->ch1=ret1;
                                                *tree=ret;
                                                    return 1;
                                                        }
                                                            if (token == '(') {
                                                                if (!expr(s,&ret1)) {
                                                                    RestorePosition();
                                                                    Delete(&ret1);
                                                                    return 0;
                                                                        }
                                                                            spp2 = spp;
                                                                                if (NextToken(s) != ')') {
                                                                                    printf("Expected token ( at position %d\n",spp2);
                                                                                    exit(1);
                                                                                        }
                                                                                            ret=mknode(_factor);
                                                                                            ret->ch1=mknode('(');
                                                                                            ret->ch2=ret1;
                                                                                                ret->ch3=mknode(')');
                                                                                                *tree=ret;
                                                                                                    return 1;
                                                                                                        }
                                                                                                            if (token == EOI) {
                                                                                                                RestorePosition();
                                                                                                                return 0;
                                                                                                                    }
                                                                                                                        
                                                                                                                        printf("Exptected  token var or num or ( at position %d\n",spp1);
                                                                                                                        exit(1);

}// end factor




// function NextToken -------------------------------------
int NextToken(char* s)
{
    int div, s1;
        char c;
            unsigned long ival;
                
                while(isws(s[spp])) spp++;   // eat ws
                if (s[spp]== '\0') {
                    return EOI;
                        }else if (s[spp]=='=') {
                            spp++;                    // eat =
                                return '=';
                                    }else if (s[spp]=='-') {
                                        spp++;                    // eat -
                                            return '-';
                                                }else if (s[spp]=='+') {
                                                    spp++;                    // eat +
                                                        return '+';
                                                            }else if (s[spp]=='*') {
                                                                spp++;                    // eat *
                                                                    return '*';
                                                                        }else if (s[spp]=='/') {
                                                                            spp++;                    // eat /
                                                                                return '/';
                                                                                    }else if (s[spp]=='(') {
                                                                                        spp++;                    // eat (
                                                                                                                          return '(';
                                                                                                                          }else if (s[spp]==')') {
                                                                                                                          spp++;                    // eat (
                                                                                                                                                            return ')';
                                                                                                                                                            }else if (s[spp]==';') {
                                                                                                                                                            spp++;
                                                                                                                                                            return ';';          // eat ;
                                                                                                                                                            }else if (isfirst(s[spp])) {
                                                                                                                                                            s1 = spp;                 // remember start of variable name
                                                                                                                                                            spp++;                    // eat first char of variable name
                                                                                                                                                            while(isnext(s[spp])) spp++;  // eat all the follow up letters of var. name
                                                                                                                                                            // hence s[spp] is not a part of variable name
                                                                                                                                                            c = s[spp];
                                                                                                                                                            s[spp]='\0';
                                                                                                                                                            if (strlen(&s[s1]) > 5) {
                                                                                                                                                            printf("variable name too long\n");
                                                                                                                                                            exit(1);
                                                                                                                                                            }
                                                                                                                                                            strcpy(token_var,&s[s1]);
                                                                                                                                                            s[spp]=c;
                                                                                                                                                            return var;
                                                                                                                                                            }else{
                                                                                                                                                            if (s[spp]=='0') {
                                                                                                                                                            token_fval = 0;
                                                                                                                                                            spp++;            // eat 0
                                                                                                                                                            if (s[spp] != '.')
                                                                                                                                                            return num;
                                                                                                                                                            spp++;            // eat .
                                                                                                                                                            token_fval = 0.0;
                                                                                                                                                            div = 10;
                                                                                                                                                            while(isdigit(s[spp])) {
                                                                                                                                                            token_fval += (((double)(s[spp]-'0'))/div);
                                                                                                                                                            div *= 10;
                                                                                                                                                            spp++;   // eat all the followup digits
                                                                                                                                                            }
                                                                                                                                                            return num;
                                                                                                                                                            }else if (isdigit(s[spp])) {
                                                                                                                                                            ival = (s[spp]-'0');
                                                                                                                                                            spp++;   // eat first digit
                                                                                                                                                            while(isdigit(s[spp])) {
                                                                                                                                                            ival = ival*10 + (s[spp]-'0');
                                                                                                                                                            spp++;   // eat all the followup digits
                                                                                                                                                            }
                                                                                                                                                            if (s[spp] != '.') {
                                                                                                                                                            if (ival > 999999) {
                                                                                                                                                            printf("integer value too big\n");
                                                                                                                                                            exit(1);
                                                                                                                                                            }
                                                                                                                                                            token_fval=(double) ival;
                                                                                                                                                            return num;
                                                                                                                                                            }
                                                                                                                                                            spp++;            // eat .
                                                                                                                                                            token_fval = (double) ival;
                                                                                                                                                            div = 10;
                                                                                                                                                            while(isdigit(s[spp])) {
                                                                                                                                                            token_fval += (((double)(s[spp]-'0'))/div);
                                                                                                                                                            spp++;   // eat all the followup digits
                                                                                                                                                            }
                                                                                                                                                            if (token_fval-(unsigned long) token_fval < 0.0000001) {
                                                                                                                                                            printf("value after decimal point too small\n");
                                                                                                                                                            exit(1);
                                                                                                                                                            }
                                                                                                                                                            return num;
                                                                                                                                                            }
                                                                                                                                                            return ERR;
                                                                                                                                                            }
                                                                                                                                                            }//end NextToken
                                                                                                                                                            
                                                                                                                                                            
                                                                                                                                                            // function mknode -------------------------------------
                                                                                                                                                            NODE* mknode(int type)
                                                                                                                                                            {
                                                                                                                                                            NODE *n;
                                                                                                                                                            n = (NODE*) malloc(sizeof(NODE));
                                                                                                                                                            n->type=type;
                                                                                                                                                            n->ch1=n->ch2=n->ch3=NULL;
                                                                                                                                                            return n;
                                                                                                                                                            }//end mknode
                                                                                                                                                            
                                                                                                                                                            
                                                                                                                                                            // function Delete -------------------------------------
                                                                                                                                                            void Delete(NODE** tree)
                                                                                                                                                            {
                                                                                                                                                            NODE* t;
                                                                                                                                                            t = *tree;
                                                                                                                                                            Delete1(t);
                                                                                                                                                            *tree = NULL;
                                                                                                                                                            }//end Delete
                                                                                                                                                            
                                                                                                                                                            
                                                                                                                                                            
                                                                                                                                                            // function Delete1 -------------------------------------
                                                                                                                                                            void Delete1(NODE* t)
                                                                                                                                                            {
                                                                                                                                                            if (t==NULL)
                                                                                                                                                            return;
                                                                                                                                                            Delete1(t->ch1);
                                                                                                                                                            Delete1(t->ch2);
                                                                                                                                                            Delete1(t->ch3);
                                                                                                                                                            if (t->type==var)
                                                                                                                                                            free((void*) t->str_value);
                                                                                                                                                            free((void*)t);
                                                                                                                                                            }//end Delete1
                                                                                                                                                            
                                                                                                                                                            
                                                                                                                                                            // function mkstring ------------------------------------
                                                                                                                                                            char* mkstring(char* s)
                                                                                                                                                            {
                                                                                                                                                            char* t;
                                                                                                                                                            t = (char*)malloc(strlen(s)+1);
                                                                                                                                                            strcpy(t,s);
                                                                                                                                                            return t;
                                                                                                                                                            }//end mkstring
                                                                                                                                                            
                                                                                                                                                            
                                                                                                                                                            // function TraverseTree --------------------------------
                                                                                                                                                            void TraverseTree(NODE* t)
                                                                                                                                                            {
                                                                                                                                                            if (t==NULL)
                                                                                                                                                            return;
                                                                                                                                                            if (t->type==var) {
                                                                                                                                                            printf("%s",t->str_value);
                                                                                                                                                            return;
                                                                                                                                                            }else if (t->type==num) {
                                                                                                                                                            printf("%f",t->num_value);
                                                                                                                                                            return;
                                                                                                                                                            }else if (t->type==_prog) {
                                                                                                                                                            TraverseTree(t->ch1);
                                                                                                                                                            TraverseTree(t->ch2);
                                                                                                                                                            return;
                                                                                                                                                            }else if (t->type==_sts) {
                                                                                                                                                            TraverseTree(t->ch1);
                                                                                                                                                            TraverseTree(t->ch2);
                                                                                                                                                            return;
                                                                                                                                                            }else if (t->type==_st) {
                                                                                                                                                            printf("%s=",t->ch1->str_value);
                                                                                                                                                            TraverseTree(t->ch2);
                                                                                                                                                            printf(";\n");
                                                                                                                                                            return;
                                                                                                                                                            }else if (t->type==_expr) {
                                                                                                                                                            TraverseTree(t->ch1);
                                                                                                                                                            TraverseTree(t->ch2);
                                                                                                                                                            return;
                                                                                                                                                            }else if (t->type==_uminus) {
                                                                                                                                                            printf("-");
                                                                                                                                                            return;
                                                                                                                                                            }else if (t->type==_expr1) {
                                                                                                                                                            TraverseTree(t->ch1);
                                                                                                                                                            TraverseTree(t->ch2);
                                                                                                                                                            TraverseTree(t->ch3);
                                                                                                                                                            return;
                                                                                                                                                            }else if (t->type==_term) {
                                                                                                                                                            TraverseTree(t->ch1);
                                                                                                                                                            TraverseTree(t->ch2);
                                                                                                                                                            return;
                                                                                                                                                            }else if (t->type==_term1) {
                                                                                                                                                            TraverseTree(t->ch1);
                                                                                                                                                            TraverseTree(t->ch2);
                                                                                                                                                            TraverseTree(t->ch3);
                                                                                                                                                            return;
                                                                                                                                                            }else if (t->type==_factor) {
                                                                                                                                                            TraverseTree(t->ch1);
                                                                                                                                                            TraverseTree(t->ch2);
                                                                                                                                                            TraverseTree(t->ch3);
                                                                                                                                                            return;
                                                                                                                                                            }else if (t->type=='+') {
                                                                                                                                                            printf("+");
                                                                                                                                                            return;
                                                                                                                                                            }else if (t->type=='-') {
                                                                                                                                                            printf("+");
                                                                                                                                                            return;
                                                                                                                                                            }else if (t->type=='*') {
                                                                                                                                                            printf("*");
                                                                                                                                                            return;
                                                                                                                                                            }else if (t->type=='/') {
                                                                                                                                                            printf("/");
                                                                                                                                                            return;
                                                                                                                                                            }else if (t->type=='(') {
                                                                                                                                                            printf("(");
                                                                                                                                                            return;
                                                                                                                                                            }else if (t->type==')') {
                                                                                                                                                            printf(")");
                                                                                                                                                            return;
                                                                                                                                                            }else{
                                                                                                                                                            printf("unknown node type d\n",t->type);
                                                                                                                                                            exit(1);
                                                                                                                                                            }
                                                                                                                                                            }//end TraverseTree
                                                                                                                                                            
                                                                                                                                                            
                                                                                                                                                            
                                                                                                                                                            int main()
                                                                                                                                                            {
                                                                                                                                                            char string[]="A=3/((23+5)*4);";
                                                                                                                                                            NODE* tree;
                                                                                                                                                            
                                                                                                                                                            if (Parse(string,&tree)) {
                                                                                                                                                            printf("OK\n");
                                                                                                                                                            TraverseTree(tree);
                                                                                                                                                            }else
                                                                                                                                                            printf("SYNTAX ERROR\n");
                                                                                                                                                            return 0;
                                                                                                                                                            }
