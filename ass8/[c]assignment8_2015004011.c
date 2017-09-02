
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define INF 9999                                //다익스트라에서 무한대개념
#define MAX_SIZE 60                             //전체 역의 갯수

typedef struct station                          //역 구조체
{
    char name[30] ;                             //역 이름 (사용하지 않음/ 번호로 대체)
    int num;                                    //역 번호
    int trans;                                  //환승역 여부
    int line;                                   //호선
    int weight[MAX_SIZE];                       //갈수있는 역의 거리 (다익스트라의 weight개념)

}station;

station metro[MAX_SIZE];                        //역 구조체 배열
int distance[MAX_SIZE];                         //거리
int visit[MAX_SIZE];                            //방문여부
int point[MAX_SIZE];                            //어디로부터 왔는지
int route_stack[MAX_SIZE];                      //경로를 알기 위한 stack
int top =-1;                                    //stack에서 사용할 top
int min_path[MAX_SIZE][MAX_SIZE];

//int name_matching(char * name);               //name을 통하여 역 num찾는 함수
void init_dvp(int start);                       //distace, visit, point 초기화 시키는 함수
int find_shortest_path(int start, int end);     //최단 경로(다익스트라이용)
int metro_fee (int distance);                   //요금계산
void set_metro();                               //txt에서 거리값 파일입출력으로 가져오기
void find_route(int start, int end);            //경로 찾는 함수
void short_path(int start, int end);

int main(int argc, const char * argv[]) {
    
    //char start[30];
    //char end[30];
    int start_num;                              // 출발역의 번호
    int end_num;                                // 도착역의 번호
    int distance;                               // 최단거리
    
    set_metro();                                // 각 역간의 거리정보 받아오는 함수
                                                // = metro 배열 정보입력
    
    printf("출발역을 입력하세요 : ");
    scanf("%d",&start_num);                     //출발역 받기
    printf("도착역을 입력하세요 : ");
    scanf("%d",&end_num);                       //도착역
    
    //start_num = name_matching(start);
    //end_num = name_matching(end);
    
    init_dvp(start_num);                        //distace, visit, point 초기화
    
    distance = find_shortest_path(start_num, end_num);      //최단거리 찾기
    printf("최단거리는 : %.1f km\n",(distance*0.1));                     //최단거리 출력
    int fee = metro_fee(distance);                          //요금 계산
    printf("요금은 : %d \n",fee);                             //요금 출력
    short_path(start_num, end_num);
    return 0;
}

void short_path(int start, int end){                            //최단거리 확인을 위해 floyid알고리즘사용
    int i, j, k;                                                //값이 동일하게 나옴을 확인
    for(i=0; i<MAX_SIZE; i++){
        for(j=0; j<MAX_SIZE; j++){
            min_path[i][j] = metro[i].weight[j];
        }
    }
    for(k=0; k<MAX_SIZE; k++){
        for(i=0; i<MAX_SIZE; i++){
            for(j=0; j<MAX_SIZE; j++){
                if( min_path[i][j] > min_path[i][k] + min_path[k][j] ){
                    min_path[i][j] = min_path[i][k] + min_path[k][j];
                }
            }
        }
    }
    //printf("최단거리 = %d\n비용 = %d\n", min_path[start][end], 1250 + ((min_path[start][end]/100)*80));
}

void set_metro()         //metro 배열 정보 입력
{
    int i,j,k,trans,num;                                  // 반복문변수, 환승여부, 역번호
    int weight;                                           // 역간 거리
    FILE *fp = fopen("/Users/dohuni/Desktop/metro.txt","r");   //파일입출력
                                                                //(맥에서는 경로를 지정해야만 파일입출력을 사용할 수 있어서 이렇게 해두었습니다. /metro.txt로 수정후 사용해주세요)
    num=0;
    for(i=1;i<=6;i++)                                           //호선수만큼 반복
    {
        for(j=0;j<10;j++)                                       //한 호선당 10개의 역
        {
            //trans=0;
             fscanf(fp,"%s",metro[num].name);                   //이름 읽어오기
            //metro[num].name=num;
            for(k=0;k<MAX_SIZE;k++)                             //각 역간의 거리 받아오기
            {
                fscanf(fp,"%d",&weight);
                if(weight <=20)                                 //각 역간의 최대거리보다 작으면
                    metro[num].weight[k] = weight;              //역간거리로 인식
                else                                            //각 역간의 최대거리보다 크면
                    metro[num].weight[k] = INF;                 // 무한대로 인식
                
               // if(metro[num].weight[k]==0)                   // 환승역의 갯수
                 //   trans++;
               // if(metro[num].weight[k]<INF)                  //연결되어있는 역의 갯수
                  //  count++;
            }
            metro[num].num = num;                               //역 번호 입력
            metro[num].trans = trans>1 ? 1 : 0;                 //환승역 여부
            metro[num].line = i;                                //호선 입력
            //metro[num].count = count;
            num++;
        }
    }

}
/*
int name_matching(char * st_name)
{
    int i;
    printf("%s",st_name);
    printf("      %d",strcmp(st_name,"중앙"));
    for(i=0; i<MAX_SIZE; i++)
    {
        if(!strcmp(st_name, metro[i].name))
            break;
    }
    return i;
}*/

void init_dvp(int start)                                        //distace, visit, point 초기화
{
    int i;
    for(i=0;i<MAX_SIZE;i++)
    {
        distance[i] = metro[start].weight[i];                   //시작점 기준 거리 입력
        visit[i]=0;                                             //방문여부
        point[i]= distance[i]<INF ? start : 99;                 //어디로부터 왔는지 확인
    }                                                           //거리가 무한대보다 작으면 start점과 연결되있다고 고려
    
}

int find_shortest_path(int start, int end)                      //다익스트라 알고리즘을 이용한 최단거리 확인
{
    int i,j,min,v;                                              //반복문변수, 최소값, 현재 방문한 역번호
    
    distance[start] =0;                                         //start점 거리, 방문여부
    visit[start] =1;

    for(i=0;i<MAX_SIZE;i++)                                     // 다익스트알고리즘으로 최소거리 찾기
    {
        min = INF;
        for(j=0;j<MAX_SIZE;j++)
        {
            if(visit[j]==0 && min>distance[j])
            {
                min= distance[j];
                v = j;
            }
        }
        visit[v]=1;
        
        for(j=0;j<MAX_SIZE;j++)
        {
            if(distance[j] > distance[v]+metro[v].weight[j])
            {
                distance[j] = distance[v]+metro[v].weight[j];
                point[j] = v;
            }
        }
    }
   
    find_route(start, end);                                     //경로찾는 함수
    
    return distance[end];
}

void find_route(int start, int end)                             //경로찾는 함수
{
    int p, f_p,f_l,l_p,l_l = 0;                               //f_p 먼저 방문한 역, f_l 먼저방문한 호선
                                                                //l_p 다음에 방문한 역, l_l 다음에 방문한 호선
    route_stack[++top] = p = end;                               //end부터 start까지 point배열을 쫒아가면서
    while(start!=p)                                             //route_stack에 저장
    {
        p = point[p];
        route_stack[++top]=p;
    }
    
    f_p = route_stack[top--];                                   //stack에서 빼내면서 저장
    f_l = metro[f_p].line;
    while(top>=0)
    {
        l_p = route_stack[top--];
        l_l = metro[l_p].line;
        
        printf("%d",f_p);
        
        if(f_l !=l_l)                                           //앞에서 방문한 호선과 다음에 방문한 호선이 다르면 환승!
            printf("(환승)");
        
        printf(">> ");
        
        f_p = l_p;
        f_l = l_l;
        
    }
    printf("%d",f_p);
    
    if(f_l != l_l)
        printf("(환승)");
    printf("\n");
}

int metro_fee (int distance)                                    //요금계산
{
    int fee = 1250 + (distance/100)*80;
    return fee;
}
