

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define alive 1 //살아있는 경우
#define die 0  //죽은 경우

//팀 멤버 구조체
typedef struct member{
    char *team;
    int num;
    int weapon;
    int islive;
}Member;

void set_member(Member ** team, char * team_name , int team_size); //멤버세팅

int main() {
    
    char our_name[15]; //우리팀 이름
    char opp_name[15]; //상대팀 이름
    int team_size; //팀 명수
    Member * our; // 우리팀 멤버
    Member * opp; // 상대팀 멤버
    
    //팀명정하기
    printf("***** 팀명정하기 *****\n");
    printf("우리 팀 명 :  ");
    scanf("%s",our_name);
    printf("상대 팀 명 :  ");
    scanf("%s",opp_name);

    
    //멤버수 정하기
    printf("***** 멤버수 정하기 *****\n");
    
    while (1) {
        printf("각 팀의 멤버수 : ");
        scanf("%d/n",&team_size);
        
        //음수 예외처리
        if(team_size>0)
            break;
        else
            printf("멤버수는 음수가 될 수 없습니다. 다시입력해주세요\n");
    }
    //멤버 세팅
    printf("***** 멤버수 정하기 *****\n");
    set_member(&our, our_name, team_size);
    set_member(&opp, opp_name, team_size);
   
    //우리팀 출력
    for(int i=0; i<team_size;i++)
    {
        printf("팀명 : %s / 번호 : %d / 생사 : %d / 무기 : %d \n",
               our[i].team, our[i].num, our[i].islive, our[i].weapon);
    }
    printf("\n");

    //우리팀이 모두 죽을때까지
    while(our[team_size-1].islive)
    {
        int i;
        //상대팀이 모두 죽었는지 확인
        for(i=0; i<team_size;i++)
            if(opp[i].islive) break;
        
        if(i==team_size) break;
        
        fight(&our,&opp,team_size); //경기
    }
    
    //우리팀이 모두 죽었을때 = 패배
    (our[team_size-1].islive==0) ?  printf("***패배***\n") : printf("***승리***\n") ;
    
    return 0;
}

//멤버세팅 함수
void set_member(Member ** team, char * team_name , int team_size)
{
    *team = (Member*)malloc(sizeof(Member)*team_size);
    
    srand(time(NULL));

    for(int i=1; i<=team_size; i++)
    {
        (*team)[i-1].team=team_name;
        (*team)[i-1].num = i;
        (*team)[i-1].weapon = rand()%3+1;
        (*team)[i-1].islive = alive;
    }
    
}

//경기 함수
int fight(Member ** our_team, Member ** opp_team, int team_size)
{
    int op_num; //상대방 선수
    
    while(1)
    {
    printf("*****대전상대 선택 :");
    scanf("%d",&op_num);
    op_num--;
        
        //이긴상대인지 체크
        if((*opp_team)[op_num].islive)
            break;
        else
            printf("이미 이긴상대입니다. 다시선택하세요\n");
    }
    
    int my_num; // 우리팀 선수 알아내기
    for(my_num=0;!(*our_team)[my_num].islive;my_num++);
    
    
    while(1)
    {
       srand(time(NULL));

        // 비긴경우
        if((*our_team)[my_num].weapon == (*opp_team)[op_num].weapon){
            
            (*our_team)[my_num].weapon= rand()%3+1;
            (*opp_team)[op_num].weapon= rand()%3+1;
            
            
        }
        //진 경우
        else if(((*our_team)[my_num].weapon+1)%3 == (*opp_team)[op_num].weapon )
        {
            (*our_team)[my_num].islive =die;
            
            if(rand()%2)
            {
                (*opp_team)[op_num].weapon = (*our_team)[my_num].weapon;
                
            }
            
            printf("우리팀 패배 ㅠㅠ\n\n\n");
            break;
            
        }
        //이긴경우
        else
        {
            (*opp_team)[op_num].islive =die;
            printf("우리팀 승리!!!!!\n");

            
            if(rand()%2)
            {
                (*our_team)[my_num].weapon = (*opp_team)[op_num].weapon;
                printf("%d로 무기가 바뀌였습니다.\n",(*our_team)[my_num].weapon);
            }
            printf("\n");
            break;

        }
    }

    return 0;
    
}
