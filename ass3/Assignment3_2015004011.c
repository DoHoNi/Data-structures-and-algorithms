#include <stdio.h>
#include <stdlib.h>
#include <string.h>		

//트리플 구조체 사용
typedef struct triple {
    int row;
    int col;
    int value;
}T;


T* MatrixSetting(T *Matrix, char *text);//행렬 읽어와서 Matrix에 저장하고 반환
T* Transpos(T *Matrix, char *text);		//희소행렬을 전치하는 방법 fast-transpose사용;
void Add(T *Matrix1, T *Matrix2);		//두 희소행렬을 더하는 함수;
void Sub(T *Matrix1, T *Matrix2);		//두 희소행렬을 빼는 함수;
void Mul(T *Matrix1, T *Matrix2);		//두 희소행렬을 곱하는 함수;
T* Mul_T(T *Matrix);					//곱한결과가 row로만 정렬되기때문에 col까지 정렬해주기위한 전치함수;

int main(int argc, char *argv[]) {
    
    T *Matrix1, *Matrix2, *Trans1, *Trans2;
    char text1[100] = "A.txt", text2[100] = "B.txt"; //파일이름 선언
   
    Matrix1 = MatrixSetting(Matrix1, text1);
    //A행렬을 setting하는 함수에 text이름을 같이넣어줘서 구분;
    
    Matrix2 = MatrixSetting(Matrix2, text2);
    //B행렬을 setting하는 함수;
    
    Trans1 = Transpos(Matrix1, text1);
    //A행렬을 전치하면서  TransA.txt 만드는 함수;
    Trans2 = Transpos(Matrix2, text2);
    //B행렬을 전치하면서  TransB.txt 만드는 함수;
    
    Add(Trans1, Trans2);
    //TransA와 TransB의 덧셈결과를 저장하는 Add.txt 만드는함수;
    Sub(Trans1, Trans2);
    //TransA와 TransB의 뺄셈결과를 저장하는 Sub.txt 만드는함수;
    Mul(Trans1, Trans2);
    //TransA와 TransB의 고셈결과를 저장하는 Mul.txt 만드는함수;
    
    //사용이 끝난 동적할당해준 구조체배열들 메모리 풀어준다;
    free(Matrix1);
    free(Matrix2);
    free(Trans1);
    free(Trans2);
    system("pause");
    return 0;
}

//text파일에 있는 행렬을 읽어서 가져오는 함수
T* MatrixSetting (T *Matrix, char *text){
    int i,size=0;
    FILE *fp;
    char CNT[10];
     //file open안될시 예외처리;
    if( (fp = fopen(text, "r")) == NULL ){
       
        printf("Cannot Open Flie\n");
        exit(1);
    }
    //text파일의 line개수를 count하여 Matrix의 size로설정;
    while( fgets(CNT, 10, fp) != NULL)
        size++;
    //Matrix의 동적할당을 calloc으로 해줌으로써 초기화;
    Matrix = (T*)calloc(size, sizeof(T));
    rewind(fp);
    //첫줄부터 int형 변수 3개를 받아와 Matrix[i]에 순서대로 저장;
    for(i=0; i<size; i++)
        fscanf(fp, "%d %d %d", &(Matrix[i].row), &(Matrix[i].col), &(Matrix[i].value));
    fclose(fp);
    return Matrix;
}

//fast_traspose함수
T* Transpos(T *Matrix, char *text){
    char TEXT[20]="Trans";//text이름앞에 Trans를 붙여 TransA.txt, Transb.txt만들기위한 변수;
    strcat(TEXT, text);	//strcat함수를 사용해 붙여준다;
    FILE *fp;
    T *Trans;
    //Matrix.value+1을 해줘야 행렬정보 입력하는 0번째 구조체생성;
    Trans = (T*)calloc((Matrix[0].value+1), sizeof(T));
    //행렬정보넣기
    int COL = Trans[0].col = Matrix[0].row;
    int ROW = Trans[0].row = Matrix[0].col;
    int VALUE = Trans[0].value = Matrix[0].value;
    int ROWTERMS[ROW];				//fast-transpose함수의 특징인 ROWTERM배열로 ROW의 빈도를 계산한다;
    int Starting_Pos[ROW];			//Starting_Pos배열로 각 ROW의 시작위치를 정해준다;
    int i,j;
    for(i=0; i<ROW; i++)
        ROWTERMS[i]=0;
    for(i=1; i<= VALUE; i++)
        ROWTERMS[Matrix[i].col]++;
    Starting_Pos[0] = 1;
    for(i=1; i<ROW; i++)
        Starting_Pos[i] = Starting_Pos[i-1] + ROWTERMS[i-1]; //Starting_Pos는 그전ROW의 시작위치 + 그전ROW의 빈도수로 결정한다;
    for(i=1; i<= VALUE; i++){
        j = Starting_Pos[Matrix[i].col]++;
        Trans[j].row = Matrix[i].col;
        Trans[j].col = Matrix[i].row;
        Trans[j].value = Matrix[i].value;
    }
    if( (fp = fopen(TEXT, "w")) == NULL ){						//마찬가지로 file-open이 되지않으면 예외처리;
        printf("Cannot Open File.\n");
        exit(1);
    }
    for(i=0; i<= VALUE; i++)									//"w"용도로 열어서 TEXT이름의 파일이 없으면 생성해서 내용 입력해준다;
        fprintf(fp, "%d %d %d\n", Trans[i].row, Trans[i].col, Trans[i].value);
    fclose(fp);													//마찬가지로 fclose해주고, Trans는 return해야하므로 free해주지 않는다;
    return Trans;
}

//덧셈연산하는 함수
void Add(T *Matrix1, T *Matrix2){
    FILE *fp;
    if( ( fp = fopen("Add.txt", "w")) == NULL ){					//file-open되지 않을시 예외처리로 exit(1);
        printf("Cannot File Open.\n");
        exit(1);
    }

    if( ( Matrix1[0].row == Matrix2[0].row ) && ( Matrix1[0].col == Matrix2[0].col ) ){   //행렬의 덧셈은 각행렬의 ROW와 COL이 모두 같아야만 연산할수있다;
        int i, j, Current=1, CNT=0, SIZE, CMP=1;
        
        T *Matrix3;
        for(i=0; i< Matrix1[0].value; i++){
            for(j=0; j<Matrix2[0].value; j++){
                if( (Matrix1[i].row == Matrix2[j].row) && (Matrix1[i].col == Matrix2[j].col) )
                    CNT++;				//두행렬에서 같은 위치가 있는지 Count해준다;
            }
        }
        Matrix3 = (T*)calloc( ( SIZE = Matrix1[0].value + Matrix2[0].value - CNT + 1), sizeof(T)); //calloc으로 할당해줌으로써 초기화;
        Matrix3[0].row = Matrix1[0].row;								//배열크기결정시 A행열 유효숫자개수 + B행열 유효숫자개수 - 같은위치 + 1 ( 1은 행렬정보 );
        Matrix3[0].col = Matrix1[0].col;
        Matrix3[0].value = SIZE-1;
        i = j = 1;
        do {
            if( Matrix1[i].row == Matrix2[j].row ){                    //우선 원소가 ROW가 같은지 확인해준다;
                if( Matrix1[i].col == Matrix2[j].col ){  //COL까지 같다면 같은위치 이므로, Matrix3에 덧셈연산해 입력하고, i, j둘다 ++;
                    Matrix3[Current].row = Matrix1[i].row;
                    Matrix3[Current].col = Matrix1[i].col;
                    Matrix3[Current++].value = Matrix1[i++].value + Matrix2[j++].value;
                }
                else if( Matrix1[i].col > Matrix2[j].col ){	//COL이 A행렬이 더크다면 작은 B행렬의 원소부터 입력해주고 다음 B행렬과 비교; / j++;
                    Matrix3[Current].row = Matrix2[j].row;
                    Matrix3[Current].col = Matrix2[j].col;
                    Matrix3[Current++].value =  Matrix2[j++].value;
                }
                else{				//반대로 B행렬 위치가 더크면 A행렬 원소 입력하고 i++;
                    Matrix3[Current].row = Matrix1[i].row;
                    Matrix3[Current].col = Matrix1[i].col;
                    Matrix3[Current++].value = Matrix1[i++].value;
                }
            }
            else if( Matrix1[i].row > Matrix2[j].row){					//ROW부터 A행렬의 원소가 더크다면 B행렬원소 입력하고 j++;
                Matrix3[Current].row = Matrix2[j].row;
                Matrix3[Current].col = Matrix2[j].col;
                Matrix3[Current++].value = Matrix2[j++].value;
            }
            else {														//ROW값이 B행렬의 원소가 더크다면 A행렬원소 입력하고 i++;
                Matrix3[Current].row = Matrix1[i].row;
                Matrix3[Current].col = Matrix1[i].col;
                Matrix3[Current++].value = Matrix1[i++].value;
            }
            
            if(i>Matrix1[0].value && j<=Matrix2[0].value){   //아직 B행렬의 원소는 비교가 되지않은 원소가 있을시 모두 Matrix3에 입력해주는 반복문;
                while(j<=Matrix2[0].value){
                    Matrix3[Current].row = Matrix2[j].row;
                    Matrix3[Current].col = Matrix2[j].col;
                    Matrix3[Current++].value = Matrix2[j++].value;
                }
            }
            
            else if(i<=Matrix1[0].value && j>Matrix2[0].value){//마찬가지로 B행렬의 원소는 다들어갔는데 A행렬의 원소가 들어가지 않은 경우;
                while(i<=Matrix1[0].value){
                    Matrix3[Current].row = Matrix1[i].row;
                    Matrix3[Current].col = Matrix1[i].col;
                    Matrix3[Current++].value = Matrix1[i++].value;
                }
            }
        } while( ( i <= Matrix1[0].value ) || ( j <= Matrix2[0].value ) ); //i, j가 개수만큼 실행되고 +1이 되었을경우 반복문을 끝낸다;
        for(i=0; i< SIZE; i++)										//마찬가지로 file-open을 "w"용도로 열어 "Add.txt"파일을 생성해 입력하고 저장한다;
            fprintf(fp, "%d %d %d\n", Matrix3[i].row, Matrix3[i].col, Matrix3[i].value);
        free(Matrix3);												//동적할당한 Matrix3을 return하는것 아니므로 사용이 끝나면 Memory반환;
        fclose(fp);													//flie-close;
    }
    else {
        fprintf(fp,"두행렬의 row와 col이 같아야 행렬덧셈이 가능합니다.\n");//행렬의 덧셈이 가능하지 않을경우 파일생성할수없다는 메세지 출력;
        printf("Add.txt생성 X\n");
    }
}

//뺄셈 함수
void Sub(T *Matrix1, T *Matrix2){
    //뺄셈은 덧셈의 방식과 동일하고, B부분을 빼주는 연산기호만 다른 방식입니다.
    FILE *fp;
    if( ( fp = fopen("Sub.txt", "w")) == NULL ){
        printf("Cannot File Open.\n");
        exit(1);
    }
    if( ( Matrix1[0].row == Matrix2[0].row ) && ( Matrix1[0].col == Matrix2[0].col ) ){
        int i, j, Current=1, CNT=0, SIZE, CMP=1;
        
        T *Matrix3;
        for(i=0; i< Matrix1[0].value; i++){
            for(j=0; j<Matrix2[0].value; j++){
                if( (Matrix1[i].row == Matrix2[j].row) && (Matrix1[i].col == Matrix2[j].col) )
                    CNT++;
            }
        }
        Matrix3 = (T*)calloc( ( SIZE = Matrix1[0].value + Matrix2[0].value - CNT + 1), sizeof(T));
        Matrix3[0].row = Matrix1[0].row;
        Matrix3[0].col = Matrix1[0].col;
        Matrix3[0].value = SIZE-1;
        i = j = 1;
        do {
            if( Matrix1[i].row == Matrix2[j].row ){
                if( Matrix1[i].col == Matrix2[j].col ){
                    Matrix3[Current].row = Matrix1[i].row;
                    Matrix3[Current].col = Matrix1[i].col;
                    Matrix3[Current++].value = Matrix1[i++].value - Matrix2[j++].value;
                }
                else if( Matrix1[i].col > Matrix2[j].col ){
                    Matrix3[Current].row = Matrix2[j].row;
                    Matrix3[Current].col = Matrix2[j].col;
                    Matrix3[Current++].value =  -Matrix2[j++].value;                             }
                else{
                    Matrix3[Current].row = Matrix1[i].row;
                    Matrix3[Current].col = Matrix1[i].col;
                    Matrix3[Current++].value = Matrix1[i++].value;
                }
            }
            else if( Matrix1[i].row > Matrix2[j].row){
                Matrix3[Current].row = Matrix2[j].row;
                Matrix3[Current].col = Matrix2[j].col;
                Matrix3[Current++].value = -Matrix2[j++].value;
            }
            else {
                Matrix3[Current].row = Matrix1[i].row;
                Matrix3[Current].col = Matrix1[i].col;
                Matrix3[Current++].value = Matrix1[i++].value;
            }
            
            if(i>Matrix1[0].value && j<=Matrix2[0].value){
                while(j<=Matrix2[0].value){
                    Matrix3[Current].row = Matrix2[j].row;
                    Matrix3[Current].col = Matrix2[j].col;
                    Matrix3[Current++].value = -Matrix2[j++].value;
                }
            }
            
            else if(i<=Matrix1[0].value && j>Matrix2[0].value){
                while(i<=Matrix1[0].value){
                    Matrix3[Current].row = Matrix1[i].row;
                    Matrix3[Current].col = Matrix1[i].col;
                    Matrix3[Current++].value = Matrix1[i++].value;
                }
            }
        } while( ( i <= Matrix1[0].value ) || ( j <= Matrix2[0].value ) );
        
        for(i=0; i< SIZE; i++)
            fprintf(fp, "%d %d %d\n", Matrix3[i].row, Matrix3[i].col, Matrix3[i].value);
        free(Matrix3);
        fclose(fp);
    }
    else {
        fprintf(fp,"두행렬의 row와 col이 같아야 행렬뺄셈이 가능합니다.\n");
        printf("Sub.txt생성 X\n");
    }
}

//곱셈함수
void Mul(T *Matrix1, T *Matrix2){
    
    FILE *FP;
    if( (FP = fopen("Mul.txt", "w")) == NULL ){
        printf("Cannot Open File.\n");
        exit(1);
    }

    if( Matrix1[0].col == Matrix2[0].row ){		//연산을 하기위해 곱셈조건인 A행렬의 COL과 B행렬의 ROW가 같은지 비교해줍니다;
        T* T_Matrix;	//T_Matrix구조체는 곱셈 연산결과를 저장해주는 구조체배열입니다;
        
        int i, j, k, SIZE = 0, CNT = 1, SAME = 0;			//반복문 사용위한 변수, 곱셈연산결과 구조체배열의 크기, 같은위치판별 변수정의;
        int *COLTERM, *ROWTERM;								//A배열의 COLTERM과 B배열의 ROWTERM COL빈도와, ROW빈도를 구해주어서;
        COLTERM = (int*)calloc( (Matrix1[0].col), sizeof(int) );
        ROWTERM = (int*)calloc( (Matrix2[0].row), sizeof(int) );
        //같은 숫자끼리 곱해주면 나올수있는 최대 곱셈연산결과의 value값이 구해집니다;
        for(i=1; i<=Matrix1[0].value; i++)
            COLTERM[Matrix1[i].col]++;
        for(i=1; i<=Matrix2[0].value; i++)
            ROWTERM[Matrix2[i].row]++;
        for(i=0; i<Matrix1[0].col; i++)
            SIZE += COLTERM[i] * ROWTERM[i];
        //이렇게 구한값으로 우선 연산결과전 Matrix3과 T_Matrix크기를 설정합니다;
        T_Matrix = (T*)calloc( SIZE , sizeof(T) );
        T Matrix3[SIZE];
        for(i=1; i<=Matrix1[0].value; i++){
            for(j=1; j<=Matrix2[0].value; j++){
                if(Matrix1[i].col == Matrix2[j].row){  					//연산과정에서 연산결과의 위치가 같은 위치가 있다면;
                    for(k=1; k<CNT; k++){								//새로 추가하는것이 아닌 그 위치의 value값에 더해주는 연산을 해야하므로;
                        if( Matrix3[k].row == Matrix1[i].row && Matrix3[k].col == Matrix2[j].col){
                            Matrix3[k].value += Matrix1[i].value * Matrix2[j].value; //행렬의 정보를 저장하는 Matrix3에서 넣으려고하는 부분전에 같은 위치가 발견되면;
                            SAME = 1;  //SAME을 1로 설정해주어 그위치에 값을 더해주고;
                        }
                    }
                    if(!SAME) {						//같은위치가 발견되지 않으면 SAME을 0으로하여 새로운 위치를 생성;
                        Matrix3[CNT].row = Matrix1[i].row;
                        Matrix3[CNT].col = Matrix2[j].col;
                        Matrix3[CNT++].value = Matrix1[i].value * Matrix2[j].value;
                    }
                    SAME = 0;
                }
            }
        }
        Matrix3[0].row = Matrix1[0].row;		//연산결과의 행렬에 ROW, COL, VALUE값을 입력해주는 부분;
        Matrix3[0].col = Matrix2[0].col;
        Matrix3[0].value = CNT-1;   			//CNT라는 변수는 연산결과의 VALUE갯수를 세는 변수인데 마지막에 ++해서 나오게되므로;
        T_Matrix = Mul_T( Mul_T(Matrix3) );	//이부분은 Matrix3을 col부분까지 같이 정렬해주기 위해 두번 trasnspose 하는 부분;
        
        
        for(i=0; i< CNT; i++)
            fprintf(FP, "%d %d %d\n", T_Matrix[i].row, T_Matrix[i].col, T_Matrix[i].value);
        fclose(FP);
        free(T_Matrix);
        free(COLTERM);
        free(ROWTERM);
    }
    else {
        fprintf(FP,"Matrix1의 Column과 Matrix2의 Row과 같은행렬만 곱셈이 가능합니다.\n");
        exit(1);																//행렬곱셈의 조건이 맞지않을경우
    }
}

//fast_transpose부분인데 Trans.txt파일 생성하지않고 전치만 해주는 함수
//곱셈함수에서 정렬시킬때 쓰기위해서 간단히 따로 만들어줌
T* Mul_T(T *Matrix){
    T* TRANS;
    TRANS = (T*)calloc(Matrix[0].value+1, sizeof(T));
    int i,j=0;
    int *TERM, *START;
    TERM = (int*)calloc(Matrix[0].col, sizeof(int));
    START = (int*)calloc(Matrix[0].col, sizeof(int));
    START[0] = 1;
    TRANS[0].row = Matrix[0].col;
    TRANS[0].col = Matrix[0].row;
    TRANS[0].value = Matrix[0].value;
    for(i=1; i<=Matrix[0].value; i++)
        TERM[Matrix[i].col]++;
    for(i=1; i<Matrix[0].col; i++)
        START[i] = START[i-1] + TERM[i-1];
    for(i=1; i<=TRANS[0].value; i++){
        j = START[Matrix[i].col]++;
        TRANS[j].row = Matrix[i].col;
        TRANS[j].col = Matrix[i].row;
        TRANS[j].value = Matrix[i].value;
    }
    free(TERM);
    free(START);
    return TRANS;
}