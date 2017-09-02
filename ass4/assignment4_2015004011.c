
#include <stdio.h>
#include <unistd.h>

#define MAX_STACK_SIZE 30

typedef struct Place{ // 좌표값을 가지는 place라는 구조체
    int x;
    int y;
}place;

void MapSetting(int map[][10]);                                                 // 파일입출력을 통한 map초기화
void FindDirection(int map[][10], place pnow ,place * stack, int * ptop);       // 갈 수 있는 길을 탐색 후 stack에 push
void FindRoute(int map[][10],int num, int *first_x, int *first_y);              // stack에 있는 값을 pop해서 이동
void push(place * stack, place element, int * ptop );                           // stack push함수
place pop(place * stack, int * ptop);                                           // stack pop함수
void printmap(map[][10]);                                                       // map을 출력해주는 함수


int main() {
    int map[10][10];  // map을 나타내는 배열
    int first_x=1;    // 미로를 출발 할때 시작 x값
    int first_y=1;    // 미로를 출발 할때 시작 y값
    
    MapSetting(map);                        //파일에서 map을 받아오는 함수 호출
    FindRoute(map,99 ,&first_x,&first_y);   // 99를 찾는 것을 목표로 미로 탐험 시작
    MapSetting(map);                        // map 초기화
    FindRoute(map,100,&first_x,&first_y);   // 100을 찾는 것을 목표로 다시 미로 탐험 시작
    printmap(map);                          // 마지막에 map한번 출력해주기
    
    
    return 0;

}
void MapSetting(int map[][10])  // 파일 입출력을 통한 map 초기화 함수
{
    int i,j;
    FILE *fp;
    if((fp = fopen("map.txt","r"))== NULL)                  // 파일이 안열리는 경우 예외처리
    {
        printf("Cannot open file \n");
        exit(1);
    }
    for(i=0; i<10; i++)                                     // 반복문으로 돌면서 초기화
    {
        if(i==0 || i==9)                                    // 가장자리 벽만들기 (첫째줄  & 마지막줄)
        {
            for(j=0;j<10;j++)
                map[i][j] =1;
        }
        else
        {
        map[i][0] = map[i][9]= 1;                           //가장자리 벽만들기 (오른쪽끝, 왼쪽끝)
        fscanf(fp,"%d %d %d %d %d %d %d %d"                     // 파일에서 값 가져와서 map에 넣기
               ,&map[i][1],&map[i][2],&map[i][3],&map[i][4],
               &map[i][5],&map[i][6],&map[i][7],&map[i][8]);
        }
    }

}
void FindDirection(int map[][10], place pnow ,place * pstack, int * ptop) // 갈 수 있는 방향 찾기
{
    place next;                         // 갈 수 있는 방향을 탐색하기 위한 place변수
    int dir_x[4] = {-1,1,0,0};          // 상 하 좌 우 의 우선 순위로 탐색 (나중에 들어간게 먼저 나오므로)
    int dir_y[4] = {0,0,1,-1};
    int i;
    for(i=0;i<4;i++)
    {
        next.x = pnow.x + dir_x[i];
        next.y = pnow.y + dir_y[i];
        if(map[next.y][next.x]!=1 && map[next.y][next.x]!=3) // 1(벽)이거나 3(이미 지난 길)이 아닌경우
        {
            push(pstack, next , ptop); // push
        }
        
    }
}
void printmap(map[][10])                        //map 출력해주는 함수
{
    int i,j;
    for(i=0; i<10; i++)                         //반복문을 돌면서 map을 출력
    {
        for(j=0; j<10; j++)
        {
            if(map[i][j]==2)                    //2 = 현재위치 -> * 으로 출력
                printf("* ");
            else if(map[i][j]==3)               //3 = 이미 지나간 길 -> 0 으로 출력
                printf("0 ");
            else
                printf("%d ",map[i][j]);        // 나머지는 map값 그대로 출력
        }
        printf("\n");
        
    }

}
void FindRoute(int map[][10],int num, int * first_x, int * first_y)  // 미로 찾아 길따라가기
{
   
    place stack[MAX_STACK_SIZE];                // 내가 갈 수 있는 길을 넣을 스텍
    int top=-1;                                 // stack top
    place now = {*first_x,*first_y};            // 현재 내가 갈 위치
    place last = {0,0};                         // 내가 서있었던 곳의 좌표 _ 그곳을 3으로 만들기 위함
    
    while (map[now.y][now.x]!=num)              // 목표한 숫자까지 loop
    {
        map[now.y][now.x]=2;                    //현재 위치를 2로 둠 _*을 찍기 위함
        printmap(map);                          //map을 출력한다
            
        FindDirection(map, now, stack , &top);  //현재 위치에서 4방향을 탐색하며 0 찾아서 stack에 넣기
        last = now;                             //현재 위치를 last에 넣고

        map[last.y][last.x]=3;                  //last의 map값을 3으로 바꿔즘 _ 이미 지난길이라는 뜻
        
        now = pop(stack , &top);                //now에 stack에 있는 값 하나 pop
        
        printf("\n\n");
        sleep(1);                               //1 초의 딜레이
    }
    map[now.y][now.x]=2;                        // 마지막에 목표한 값을 간 경우
                                                //map의 값을 2로 바꿔서 현재위치를 출력을 용이하게 해줌
    *first_x = now.x;                           // 다음 탐색시 시작지점을 현재 끝난 지점으로 지정
    *first_y = now.y;

}
void push(place * pstack, place  pelement, int * ptop ) //stack의 push함수 구현
{
    
    pstack[++*ptop] =  pelement;
}
place pop(place * stack, int * ptop)    //stack의 pop함수 구현
{
    return stack[(*ptop)--];
}
