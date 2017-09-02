
#include <string.h>
#include <stdio.h>
#include <ctype.h>


enum { lparen, rparen, timess, divide, plus, minus, and, or };
    //  '('     ')'     '*'     '/'    '+'    '-'   '&'  '|'

typedef struct element                              //식에서의 피연산자와 연산자를 넣을 수 있는 구조체 제작
{
    int num;                    //숫자인 경우 값넣기
    char op;                    //연산자인경우 연산자 넣기
    int op_num;                 //연산자인경우 연산자 번호 넣기 (enum으로 저장되어있는 숫자)
    int type;                   //숫자인지 연산자인지 확인하는 변수
    
}element;

void check(char * ex_string, element * first_stack,int * f_top);            //식을 하나하나 확인하면서 구조체배열에 넣기
void push(element * pstack, element pelement, int * ptop );                 //구조체 push
element pop(element * stack, int * ptop);                                   //구조체 pop
void make();                                                                //postfix만들기
void f_push(float * pstack, float pelement, int * ptop );                   //float 배열에 push
float f_pop(float * stack, int * ptop);                                     //float 배열에 pop
void eval();                                                                //postfix 계산하기

element first_stack[100];                   //처음에 식을 체크하며 구조체를 담는 스텍
int f_top=-1;                               //first_stack의 top
element op_stack[100];                      //postfix를 만들기위해 연산자를 담는 stack
int op_top=-1;                              //op_stack의 top
element po_stack[100];                      //postfix를 만들기위해 전체 식을 담는 stack
int po_top=-1;                              //po_stack의 top

int main()
{
    char ex_string[100];                                                //식을 받아오는 char형 배열
    printf("계산을 원하는 식을 입력하세요 : ");
    scanf("%s",ex_string);                                              //식 받아오기
    
    check(ex_string,first_stack, &f_top);                               //식을 하나하나 확인하면서 구조체 배열에 넣기
    pop(first_stack, &f_top);                                           //마지막에 \0 빼주기위해서 pop
    make();                                                             //postfix식 만들기
    eval();                                                             //postfix로 계산하기
    
}

void check(char * ex_string, element * first_stack, int * fp_top)       // 하나하나 확인하며 숫자와 연산자를 구조체로 만드는 함수
{
    int num=0;              //숫자를 만드는 변수
    int num_check=0;        //숫자가 들어온 적이 있는지 확인하는 변수
    element temp;           //임시 구조체
    
    for(int i =0;i<=strlen(ex_string); i++)
    {
        if(isdigit(ex_string[i]))                       //숫자인경우
        {
            if(num_check)                               //(숫자가 이미 한번 들어온경우)
                num =num*10 + (ex_string[i]-'0');       //앞 숫자의 10을 곱해서 자릿수를 높여준다.
            else                                        //(숫자가 들어온적이 없는경우)
            {
                num = ex_string[i]-'0';                 //숫자를 num에 넣어준다.
                num_check++;                            //num_check를 바꿔준다.
            }
        }
        else                                            //숫자가 아닌경우 (연산자)
        {
            if(num_check != 0){                         //이전에 숫자가 들어온 경우(3+ 이런식으로)
                temp.type = 0;                          //숫자 구조체 만들어서 push
                temp.num = num;
                push(first_stack, temp, fp_top);
                
                temp.type = 1;                          //연산자 구조체 만들기
                temp.num = 0;
                temp.op = ex_string[i];
            }
            else{                                       //이전에 숫자가 아닌것이 들어온경우 (-(.. 이런식으로)
                temp.type = 1;                          //연산자 구조체 만들기
                temp.num = 0;
                temp.op = ex_string[i];
            }
            
            switch (ex_string[i])                       // 연산자가 어떤건지에 따라서 op_num을 다르게 넣기
            {
                case '(':
                    temp.op_num = lparen;
                    break;
                case ')':
                    temp.op_num = rparen;
                    break;
                case '*':
                    temp.op_num = timess;
                    break;
                case '/':
                    temp.op_num = divide;
                    break;
                case '+':
                    temp.op_num = plus;
                    break;
                case '-':
                    temp.op_num = minus;
                    break;
                case '&':
                    temp.op_num = and;
                    i +=1;
                    break;
                case '|':
                    temp.op_num = or;
                    i +=1;
                    break;
            }
            push(first_stack, temp, fp_top);           //만들어진 연산자 구조체를 push
            num_check=0; num=0;                        //num, num_check 초기화
        }
    }
    
}

//postfix 만들기
void make()
{
    int i, temp;                                    //반복문변수, 마지막에 op_top을 담는 임시변수
    int isp[]={0,20,15,15,10,10,5,5};               //in_stack 우선순위
    int icp[]={20,20,15,15,10,10,5,5};              //incoming 우선순위
    
    for(i=0;i<= f_top ;i++)
    {
        if(first_stack[i].type==0)                                  //숫자일때
        {
            push(po_stack, first_stack[i] ,&po_top);                //postfix를 담는 스텍에 push
        }
        else if(first_stack[i].op_num==rparen)                      //')'일때
        {
            while(op_stack[op_top].op_num!=lparen)                  //'('가 나올때까지
                push(po_stack,pop(op_stack, &op_top),&po_top);      // 연산자 스텍에서 po_stack으로 연산자 이동
            pop(op_stack,&op_top);                                  //마지막 '('은 그냥 pop해서 날리기
        }
        else
        {
            if(op_top==-1){                                         //op_stack에 아무것도없는 경우
                push(op_stack,first_stack[i],&op_top);              //비교없이 바로 넣는다.
            }
            else
            {
                while(isp[ op_stack[op_top].op_num ] >= icp[first_stack[i].op_num]) //안에있는 연산자의 우선순위 >= 밖에있는 연산자인 동안
                    push(po_stack,pop(op_stack,&op_top),&po_top);                   //연산자를 op_stack에서 po_stack으로 이동
                push(op_stack,first_stack[i],&op_top);                              //그리고 밖에있는 연산자를 op_stack에 넣기
            }
        }
    }
    
    temp = op_top;                                                  //반복문을 돌면서 op_top이 변화하므로 미리 담아둔다.
    for(i=temp; i>-1; i--){
        push(po_stack, pop(op_stack, &op_top), &po_top);            //op_stack에 남아있는 모든 연산자들을 po_stack에 옮겨준다.
    }
    
    printf("postfix로 변환한 식 : ");                                 //profix로 변환한 식 출력
    for(i=0;i<=po_top;i++)
    {
        if(po_stack[i].type==0)
            printf("%d",po_stack[i].num);
        else
            printf("%c",po_stack[i].op);
    }
    printf("\n");
}

void eval()                                                         //postfix로 계산하기
{
    int i;                                  //반복문변수
    float num1, num2 , result = 0.0;        //po_stack에서 값을 가져온 num1,num2 / 계산결과를 담는 result
    float num_stack[30];                          //숫자들을 담는 float stack
    int num_top=-1;                         //num_stack의 top
    
    for(i=0; i<=po_top;i++ )
    {
        if(po_stack[i].type==0)                                 //po_stack[i]가 숫자인경우
        {
            f_push(num_stack, (float)po_stack[i].num, &num_top);      //num_stack에 push
        }
        else                                                    //po_stack[i]가 연산자인경우
        {
            num1 = f_pop(num_stack, &num_top);                        //num_stack에서 pop
            num2 = f_pop(num_stack, &num_top);                        //num_stack에서 pop
            
            switch (po_stack[i].op)                                   //op에 따른 다른 연산처리
            {
                case '*':                       //곱셈
                    result = num1 * num2;
                    break;
                case '/':                       //나눗셈
                    result = num1 / num2;
                    break;
                case '+':                       //덧셈
                    result = num1 + num2;
                    break;
                case '-':                       //뺄셈
                    result = num1 - num2;;
                    break;
                case '&':                       //and
                    result = num1 && num2;
                    break;
                case '|':                       //or
                    result = num1 || num2;
                    break;
            }
            f_push(num_stack, result, &num_top);                    //결과값을 다시 num_stack에 push
        }
    }
    
    result = f_pop(num_stack, &num_top);                                  //가장 마지막에 stack안에 남아있는 최종결과값 가져오기
    printf("결과값은 : %.2f",result);                                            //result 출력
    
}
void push(element * pstack, element pelement, int * ptop ) //구조체stack의 push함수 구현
{
    pstack[++(*ptop)] =  pelement;
}
element pop(element * stack, int * ptop)    //구조체stack의 pop함수 구현
{
    return stack[(*ptop)--];
}
void f_push(float * pstack, float pelement, int * ptop ) //float stack의 push함수 구현
{
    pstack[++(*ptop)] =  pelement;
}
float f_pop(float * stack, int * ptop)    //float stack의 pop함수 구현
{
    return stack[(*ptop)--];
}
