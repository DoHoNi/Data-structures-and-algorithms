#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0

typedef struct dic{             //사전구성 구조체
    char key[30];               //영어단어
    char value[30];             //의미
}dic;
typedef struct tree_node* tree_ptr;        //tree_node 포인터
typedef struct tree_node                   //node
{
    tree_ptr left_child;                    //왼쪽 자식
    tree_ptr right_child;                   //오른쪽 자식
    short int bf;                           //balance factor
    dic data;                               //사전data
}tree_node;

void avl_insert(tree_ptr *parent, dic x, int* unbalanced);  //트리에 노드 삽입
int avl_search(tree_ptr* parent, dic x);                    //노드 검색
void left_rotation(tree_ptr *parent, int *unbalanced);      //왼쪽 벨런스 오버
void right_rotation(tree_ptr *parent, int *unbalanced);     //오른쪽 벨런스 오버
void init_avl(tree_ptr * proot, int* unbalanced);           //초기화(단어10개 추가)
void insert_word(tree_ptr * root, int * unbalanced );       //단어 넣기 시작 함수
void search_word(tree_ptr * root);                          //단어 검색 시작 함수
void all_word(tree_ptr * root);                             //노드 순회

int main(int argc, const char * argv[]) {
   
    int unbalanced = FALSE;             //불균형여부
    tree_ptr root = NULL;               //시작 노드 포인터
    int type;                           //명령 번호
    
    init_avl(&root, &unbalanced);       //초기화(파일에서 단어10개 읽어오기)
    
    while(1)
    {
        printf("----DohyunDic ver0.01----\n");
        printf("원하시는 기능을 선택하세요.\n");
        printf("1. 단어추가 \n");
        printf("2. 단어검색 \n");
        printf("3. 모든단어보기 \n");
        printf("4. 종료\n");
        printf("번호를 입력하세요 : ");
        scanf("%d",&type);
        
        switch (type) {
            case 1:
                insert_word(&root,&unbalanced);     //단어 추가
                break;
            case 2:
                search_word(&root);                 //단어 검색
                break;
            case 3:
                all_word(&root);                    //모든단어보기
                break;
            case 4:
                exit(0);                            //종료하기
                break;
            default:
                printf("잘못된 입력입니다.");           //default
                exit(0);
                break;
        }
    }
    return 0;
}

void insert_word(tree_ptr * root, int * unbalanced )        //단어 추가 시작함수
{
    dic temp;                                      //추가할 영어단어 입력받기
    printf("추가할 영어단어를 입력하세요 : ");
    scanf("%s",temp.key);
    printf("단어의 의미를 입력하세요 :");
    scanf("%s",temp.value);
    
    avl_insert(root, temp, unbalanced);            //avl_삽입함수
    
}
void search_word(tree_ptr * root)                           //단어 검색 시작 함수
{
    dic temp;                                       //검색할 단어 입력받기
    printf("검색할 영어단어를 입력하세요 : ");
    scanf("%s",temp.key);
    avl_search(root, temp);                         //검색함수
    
}
void all_word(tree_ptr * root)                              //모든 노드 탐색
{
    if((*root) == NULL)return;                      //inorder 방식으로 탐색

    all_word(&((*root)->left_child));
    printf("%s : %s \n", (*root)->data.key, (*root)->data.value);
    all_word(&((*root)->right_child));
    
}


void init_avl(tree_ptr * proot, int* unbalanced)            //파일입력으로 단어 10개 추가
{
    int i;
    dic temp;
    FILE* fp = fopen("/Users/dohuni/Desktop/수업/자구알/hw/ass9/ass9/ass9/dic.txt","r");
    for(i=0; i<10; i++)
    {
        fscanf(fp,"%s %s",temp.key,temp.value);
        avl_insert(proot, temp, unbalanced);
    }
}
int avl_search(tree_ptr* parent, dic x)                     //avl 검색 함수
{
    if(!(*parent))                          //탐색했는데 null인경우
    {
        printf("찾을 수 없습니다.\n");
        return 0;
    }
    else
    {
        if(strcmp(x.key, (*parent)->data.key)<0)            //x.key가 사전순으로 앞쪽일때
        {
            printf("%s -> ",(*parent)->data.key);
            avl_search(&(*parent)->left_child, x);
        }
        else if(strcmp(x.key, (*parent)->data.key)>0)      //x.key가 사전순으로 뒤쪽일때
        {
            printf("%s -> ",(*parent)->data.key);
            avl_search(&(*parent)->right_child, x);
        }
        else                                                //x.key를 찾았을때
        {
            printf("%s\n",(*parent)->data.key);
            printf("%s : %s\n",(*parent)->data.key, (*parent)->data.value);
            return 0;
        }
    }
    return 0;
}


void avl_insert(tree_ptr *parent, dic x, int* unbalanced)       //avl_삽입함수
{
    if(!*parent)                                                //트리가 null일때, 끝까지 내려갔을때
    {
        *unbalanced = TRUE;
        *parent =(tree_ptr)malloc(sizeof(tree_node));           //새로운 노드를 하나 만들어서 추가해주기
        (*parent)->left_child = (*parent)->right_child = NULL;
        (*parent)->bf = 0;
        (*parent)->data =x;
    }
    else if(strcmp(x.key,(*parent)->data.key)<0)                //x.key 가 사전순으로 앞순일때
    {
        avl_insert(&(*parent)->left_child, x, unbalanced);      //null될때까지 재귀로 다시 호출 (parent = left_child)
        
        if(*unbalanced)
        {
            switch ((*parent)->bf)                              //parent의 balance factor값에 따른 시행
            {
                case -1:
                    (*parent)->bf =0;                           //왼쪽에 추가하니까 +1
                    *unbalanced =FALSE;
                    break;
                case 0 :(*parent)->bf = 1;                      //왼쪽에 추가하니까 +1
                    break;
                case 1 : left_rotation(parent,unbalanced);     //bf가 1인상태에서 왼쪽에 추가하면 bf=2가 되서 균형 무너짐
            }                                                  //left_rotation을 통해 균형맞추기
            //printf("avl_insert_left\n");
        }

    }
    else if(strcmp(x.key,(*parent)->data.key)>0)                //x.key가 사전순으로 뒷순일때
    {
        avl_insert(&(*parent)->right_child, x, unbalanced);     //null될때까지 재귀로 다시 호출 (parent = right_child)
        
        if(*unbalanced)
        {
            switch ((*parent)->bf) {                            //parent의 balance factor값에 따른 시행
                case 1: (*parent)->bf = 0;                      //오른쪽에 추가하니까  -1
                        *unbalanced = FALSE;
                    break;
                case 0: (*parent)->bf =-1;                      //오른쪽에 추가하니까  -1
                    break;
                case -1: right_rotation(parent,unbalanced);     //bf=-2가되서 균형무너짐
                                                                //right_rotation을 통해 균형맞추기
            }
            //printf("avl_insert_right\n");
        }

        
    }
    else
    {                                                           //추가하려는 단어가 이미 있는경우
        *unbalanced =FALSE;
        printf("the key is already in the tree\n");
    }
    
}

void left_rotation(tree_ptr *parent, int *unbalanced)           //왼쪽 벨런스 무너진경우 돌리기
{
    tree_ptr grand_child, child;
    child = (*parent)->left_child;
    if(child->bf == 1)                                          //LL
    {
        (*parent)->left_child = child->right_child;
        child->right_child =*parent;
        (*parent)->bf =0;
        (*parent) = child;
    }
    else                                                        //LR
    {
        grand_child = child->right_child;
        child->right_child = grand_child->left_child;
        grand_child->left_child = child;
        (*parent)->left_child = grand_child->right_child;
        grand_child->right_child =*parent;
        switch (grand_child->bf) {
            case 1: (*parent)->bf =-1;
                child->bf =0;
                break;
            case 0: (*parent)->bf = child->bf =0;
                break;
            case -1:(*parent)->bf =0;
                    child->bf =1;
                
        }
        *parent = grand_child;
    }
    (*parent)->bf =0;
    *unbalanced = FALSE;
}

void right_rotation(tree_ptr *parent, int *unbalanced)          //오른쪽 벨런스 무너진경우 돌리기
{
    tree_ptr grand_child, child;
    child = (*parent)->right_child;
    if(child->bf == -1)                                         //RR
    {
        (*parent)->right_child = child->left_child;
        child->left_child =*parent;
        (*parent)->bf =0;
        (*parent) = child;
    }
    else                                                        //RL
    {
        grand_child = child->left_child;
        child->left_child = grand_child->right_child;
        grand_child->right_child = child;
        (*parent)->right_child = grand_child->left_child;
        grand_child->left_child =*parent;
        switch (grand_child->bf) {
            case 1: (*parent)->bf =0;
                child->bf =-1;
                break;
            case 0: (*parent)->bf = child->bf =0;
                break;
            case -1:(*parent)->bf =1;
                child->bf =0;
                
        }
        *parent = grand_child;
    }
    (*parent)->bf =0;
    *unbalanced = FALSE;
}

