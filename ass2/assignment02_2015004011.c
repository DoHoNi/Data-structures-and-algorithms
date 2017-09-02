

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int hanoi3(int pan); //하노이탑 기둥3개 버전
int hanoi4(int pan); //하노이탑 기둥4개 버전
int hanoi5(int pan); //하노이탑 기둥5개 버전

int* arr3 = NULL; // 기둥3개짜리 계산한 값 저장 배열
int* arr4 = NULL; // 기둥4개짜리 계산한 값 저장 배열
int* arr5 = NULL; // 기둥5개짜리 계산한 값 저장 배열

int findMin(int * arr, int size); //최솟값 찾는 함수


int main(int argc, const char * argv[]) {
    
    int pan; // 원판의 갯수
    int gidung; // 기둥의 갯수
    int min; //최솟값
    while(1)
    {
    printf("기둥의 갯수 : ");
    scanf("%d",&gidung);
        if(gidung==0) return 0;
    do{
        printf("원판의 갯수 : ");
        scanf("%d",&pan);
    } while(pan <0);
    
    arr3 = (int *)malloc(sizeof(int)*pan); // arr3 동적할당
    memset(arr3,0,sizeof(int)*pan); //초기화

    switch(gidung)
    {
        case 3 :
            min = hanoi3(pan);
            break;
            
        case 4 :
            arr4 = (int *)malloc(sizeof(int)*pan); //arr4 동적할당
            memset(arr4,0,sizeof(int)*pan); //초기화
            min = hanoi4(pan);
            break;
            
        case 5 :
            arr4 = (int *)malloc(sizeof(int)*pan); //arr 4,5 동적할당 및 초기화
            memset(arr4,0,sizeof(int)*pan);
            arr5 = (int *)malloc(sizeof(int)*pan);
            memset(arr5,0,sizeof(int)*pan);
            min = hanoi5(pan);
            break;
            
        default: //예외처리
            printf("잘못된 입력입니다.\n");
            return -1;
    }
    
    //동적할당 해제
    free(arr3);
    free(arr4);
    free(arr5);
    
    printf("총 %d번 수행되었습니다. \n\n",min);
    }
    return 0;
}



int hanoi3(int pan)  // 기둥3개 버전
{
    int count =0;
    if(arr3[pan-1]) return arr3[pan-1];
    if(pan==1) count++;
    else
        count += 2*hanoi3(pan-1)+1;
    arr3[pan-1] = count;
    return count;
}

int hanoi4(int pan) //기둥 4개 버전
{
    int i;
    int count=0, min=0;
    int *minarr;
    
    if(arr4[pan-1])return arr4[pan-1]; //기존에 계산한 값이 있으면 바로 값 리턴
    
    minarr = (int*)malloc(sizeof(int)*(pan-1)); //최솟값을 찾는 배열 동적할당
    
    if(pan==1){
        count++;
        min = count;
    }
    else //모든 경우를 체크해서 최솟값을 알아냄
    {
        for(i=1;i<pan;i++)
        {
            count =0;
            count += 2 * hanoi4(i) + hanoi3(pan-i);
            minarr[i-1] = count;
        }
        min = findMin(minarr, pan-1);
    }
    free(minarr);
    arr4[pan-1] = min; // (arr4[pan-1]에 값이 없으므로) 계산한 값 넣어주기

    return min;
    
}
int hanoi5(int pan) //기둥 5개 버전
{
    int i;
    int count=0, min=0;
    int *minarr;
    
    if(arr5[pan-1])return arr5[pan-1]; //기존에 계산해둔 값이 있으면 바로 값 리턴
    
    minarr = (int*)malloc(sizeof(int)*(pan-1)); //최솟값을 찾는 배열 동적할당
    
    if(pan==1){
        count++;
        min = count;
    }
    else //모든 경우를 체크해서 최소의 값을 알아냄
    {
        for(i=1;i<pan;i++)
        {
            count = 0;
            count += 2 * hanoi5(i) + hanoi4(pan-i);
            minarr[i-1] =count;
        }
        min = findMin(minarr, pan-1);
    }
    
    free(minarr);
    arr5[pan-1] = min; // (arr5[pan-1]에 값이 없으므로) 계산한 값 넣어주기
    return min;
    
}

int findMin(int * arr, int size) //최솟값찾는 함수
{
    int min;
    int i;
    min = arr[0];
    for(i=1; i<size; i++)
    {
        if(min>arr[i])
            min=arr[i];
    }
    return min;
}
