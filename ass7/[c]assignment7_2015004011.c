

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void insertionSort (int list[], int size);                  //삽입정렬 알고리즘
void SelectionSort(int list[] , int size);                  //선택정렬 알고리즘
void merge(int list[], int left, int mid, int right);       //합병 알고리즘
void mergeSort(int list[], int left, int right);            //합병정렬 알고리즘
void heapSort(int list[] , int n);                          //힙정렬 알고리즘
int delete_minheap();                                       //힙 삭제 알고리즘
void insert_minheap();                                      //힙 삽입 알고리즘
void randomNum(int list[], int size);                       //난수생성 알고리즘

int heap_size;              //전역변수로 heap size
int heap[60001];            //전역변수로 heap 배열 생성
clock_t START, END;         //실행시작시간, 끝나는시간

void main()
{
    clock_t COUNT;
    
    int i,size;                                 //반복문변수, 정렬할 list size
    int list[60000];                            //list
    int sortnum = 0;                            //정렬 알고리즘 번호
    double ET;                                  //excution time
    printf("정렬을 수행할 알고리즘을 선택해 주세요\n");
    printf("1. 삽입정렬 \n");
    printf("2. 선택정렬 \n");
    printf("3. 합병정렬 \n");
    printf("4. 힙정렬 \n");
    printf("정렬알고리즘 번호 : ");
    scanf("%d",&sortnum);                       //정렬알고리즘 선택
    
    for(i =1;i <=6 ;i++)                        //6번 반복하며 선택한 난수 정렬 알고리즘 시작
    {
        size = i*10000;
        randomNum(list, size);                  //난수생성 알고리즘
        
        switch (sortnum) {
            case 1:                             //1.삽입정렬 알고리즘
                START = clock();                //시작시간
                insertionSort (list, size);     ////삽입정렬
                END = clock();                  //종료시간
                ET = (double)(END - START)/CLOCKS_PER_SEC;  //실행시간
                printf("[삽입정렬]%d개 정렬의 실행시간은 %g초 입니다.\n",size, ET);     //실행시간 출력
                break;
            case 2:                             //2.선택정렬 알고리즘
                START = clock();
                SelectionSort(list, size);      //선택정렬
                END = clock();
                ET = (double)(END - START)/CLOCKS_PER_SEC;
                 printf("[선택정렬]%d개 정렬의 실행시간은 %g초 입니다.\n",size, ET);
                break;
            case 3:
                START = clock();
                mergeSort(list, 0, size-1);
                END = clock();
                ET = (double)(END - START)/CLOCKS_PER_SEC;
                printf("[합병정렬]%d개 정렬의 실행시간은 %g초 입니다.\n",size, ET);
                break;
            case 4:
                START = clock();
                heapSort(list, size);
                END = clock();
                ET = (double)(END - START)/CLOCKS_PER_SEC;
                printf("[힙정렬]%d개 정렬의 실행시간은 %g초 입니다.\n",size, ET);
                break;
            default:        //예외처리
                printf("잘못된 입력입니다\n");
                break;
        }
    }
}

void randomNum(int list[], int size)
{
    int i, temp;
    int r1, r2;
    for(i=0;i<=size;i++)    //배열에 순차적으로 입력하며 초기화
        list[i] =i;
    
    srand(time(NULL));      //시드값 설정
   
    
    for(i=1;i<=size;i++)    //랜덤적으로 2개의 숫자를 바꾸는 방식으로
    {                       //겹치지 않는 난수 생성
        r1 = rand()%size;
        r2 = rand()%size;
        
        temp = list[r1];
        list[r1] = list[r2];
        list[r2] = temp;
    }
    
}
void insert_minheap(int item)       //힙정렬 삽입하기
{
    int i;
    i = ++heap_size;
    
    while((i != 1) && (item < heap[i/2]))   // item이 부모보다 작은지 확인
    {
        heap[i] = heap[i/2];                // 부모의 값을 자식에 넣고
        i /=2;                              // i를 부모의 index로 설정
    }
    heap[i] = item;                         // item 넣기
}

int delete_minheap()
{
    int parent, child;
    int item,temp;
    
    item = heap[1];
    temp = heap[heap_size--];
    parent =1;
    child =2;
    
    while(child <=heap_size)            //힙 배열의 크기넘어가면 종료
    {
        if((child<heap_size) && (heap[child] > heap[child+1]))  // 오른쪽 자식이 더 큰 경우
           child++;

        if(temp <= heap[child])     //temp가 자리를 찾은경우
           break;
        
        heap[parent] = heap[child];     //한단계 내려가기
        parent = child;
        child *=2;
    }
    heap[parent] = temp;
    return item;
}
           
void heapSort(int list[] , int n)
{
    int i;
    
    heap_size =0;
    
    for(i=0;i<n;i++)                //힙배열에 삽입
    {
        insert_minheap(list[i]);
    }
    for(i =0; i<=n-1; i++)          //삭제하면서 정렬
    {
        list[i] = delete_minheap();
    }
    
    /*for(i=0;i<n;i++)
    {
        printf("%d ",list[i]);
    }*/
    
}
    

void mergeSort(int list[], int left, int right)     //합병정렬 알고리즘
{
    int mid,i;
    
    if(left <right)
    {
        mid = (left +right)/2;                      //중앙값 찾기
        
        mergeSort(list, left, mid);                 //처음부터 중간까지
        mergeSort(list, mid+1, right);              //중간부터 끝까지
        merge(list, left, mid, right);              //합병하기
    }
    
    
}

void merge(int list[], int left, int mid, int right)
{
    int i,j,k,m;
    
    i = left;                       //첫번째배열시작인덱스
    j = mid+1;                      //두번째배열시작인덱스
    k = left;                       //결과 배열의 인덱스
    
    int tempArray[60000];
    
    
    while (i <= mid && j <= right) {                        //left부터 mid 까지의 블록과 mid+1부터 right까지의 블록을 서로 비교하는 부분
        if (list[i] < list[j]){                             //left index 값이 right index 값보다 작으면 left index 값을 결과 result에 복사
            tempArray[k] = list[i];                         //결과배열에 첫번째 배열원소 추가
            i++;                                            //첫번째배열 인덱스 증가
        }else{                                              //아니라면 right index 값을 결과 result에 복사
            tempArray[k] = list[j];                         //결과배열에 두번째 배열 원소 추가
            j++;                                            //두번째 배열 인덱스 증가
        }
        k++;
    }
    
    
    
    
    if (i > mid){                                           // left 블록의 값은 다 처리되었는데 right 블록의 index가 아직 남아있을 경우
        for (m = j; m <= right; m++){                       // right index를 순차적으로 결과 result에 복사
            tempArray[k] = list[m];
            k++;
        }
    } else {                                                // left 블록의 index가 아직 남아있을 경우 left index를 순차적으로 결과 result에 복사
        for (m = i; m <= mid; m++){
            tempArray[k] = list[m];
            k++;
        }
    }
    
    for(m = left; m <= right; m++){                         //정렬된 리스트 넘기기
        list[m] = tempArray[m];
    }
}

void SelectionSort(int list[] , int size)                    //선택정렬 알고리즘
{
    int i,j,min;
    int temp =0;
    
    for(i=0; i<size;i++)
    {
        min = list[i];
        for(j=i+1;j<size -1; j++)                 // i 번째 이후에서 가장 작은 값 찾기
        {
            if(min > list[j])
            {
                min =list[j];                 //가장 작은 값 설정
                temp = j;
            }
        }
        list[temp] = list[i];                   //swap
        list[i] = min;
    }
    
}


void insertionSort (int list[], int size)                   //삽입정렬 알고리즘
{
    int i, j, temp, in;
    
    for(i=1; i < size; i++)
    {
        in = list[i];
        for(j=i-1; (j>=0) && (in<list[j]); j--)     //자기 자리보다 작은 자리에서 자신보다 크면 한칸씩 위로 올리기
        {
            list[j+1] = list[j];
        }
        list[j+1] =in;                              //자기자리 찾으면 삽입
    }
    
    
}

