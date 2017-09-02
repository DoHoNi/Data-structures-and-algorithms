#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define TRUE 1
#define FALSE 0

typedef struct node* L;
typedef struct node {
    char* key;																//char 각 key마다 길이가 다르므로 동적할당하여 받는다;
    int code; 																//해당 key의 code값;
    L link;																	//linked_list로 연결하기 위한 pointer;
}NODE;

char bit_string[100000] = {'\0', };											//압축된 문자열을 bit_string으로 변경하여 저장하는 배열;
char decode_bit_string[100000] = {'\0', };									//output.txt에서 읽어와 decode후 bit_string을 저장하는 배열;
char string[1000] = {'\0', };												//전체 string의 크기;
int compress_string[1000] = {0, };											//압축한 숫자를 저장하는 integer string;
int compress_size = 0;														//compress_string의 길이;
int decode_string[100] = {0, };												//binary bit로 전환한 코드를 다시 압축코드로 바꾸는 과정;
int decode_size = 0;								 						//decode한 압축코드의 길이;
int MAX = 0;																//binary로 변환시 binary code의 크기;
int ALPHABET[26] = {0,};													//Alphabet의 크기만큼의 배열;
L HASH_TABLE;																//전체 HASH_TABLE에 대한 pointer;
int TABLE_SIZE = 0;															//HASH_TABLE의 SIZE;
L RESTRUCT_TABLE;															//초기 HASH_TABLE저장하여 재구축시 사용;
int RE_TABLE_SIZE = 0;														//HASH_TABLE의 SIZE;

void BINARY_DECODING();														//binary mode로 압축된 것을 다시 푸는 함수;
void BINARY_COMPRESS();														//binary mode로 바꿔서 압축하는 함수;
void COMPRESS();												 			//압축을 실행하는 함수 = bit_string에 배열저장;
void STRUCT_INITIAL_TABLE(L* TABLE, int* TABLE_SIZE);						//초기 HASH_TABLE을 형성하는 함수;
void SUB_STRING(char* copy, char* string, int start, int end);				//원하는 크기만큼의 sub_string반환하는 함수;
void FREE(L* TABLE);														//프로그램 종료시 모든 Memory할당 free;

int main(int argc, char *argv[]) {
    FILE* fp;
    L TEMP;
    int i = 0;
    printf("Enter the string : ");
    if( (fp=fopen("input.txt", "r")) == NULL ){
        printf("Cannot Open File.\n");
        exit(1);
    }
    fgets(string, 100, fp);
    printf("%s\n", string);
    STRUCT_INITIAL_TABLE(&HASH_TABLE, &TABLE_SIZE);
    COMPRESS();
    printf("\n< 압축을 통해 완성된 사전 >\n");
    TEMP = HASH_TABLE;
    for( ; TEMP; TEMP = TEMP->link){
        printf("HASH_TABLE[%d]'s key = %s code = %d\n", i++, TEMP->key, TEMP->code);
    }
    printf("\ncompress_string = ");
    for(i=0; i<compress_size; i++){
        printf("%d", compress_string[i]);
    }
    printf("\n");
    BINARY_COMPRESS();
    BINARY_DECODING();
    FREE(&HASH_TABLE); 														//사용한 HASH_TABLE의 모든 Memory 반환;
    FREE(&RESTRUCT_TABLE);													//사용한 RESTSRUCT_TABLe의 모든 Memory 반환;
    return 0;
}

void BINARY_DECODING(){
    FILE* fp;
    unsigned char change = 1;												//decode하는데 필요한 변수들;
    char* check, *restore;
    char buffer;
    int start = 0, end = 0;
    int num = 0, mul = 1;
    int i, j, k, shift = 7, now = 0, FOUND = FALSE;
    L LEAD, TEMP, NEW;
    if( (fp=fopen("output.txt", "r")) == NULL ){							//output.txt를 read형식으로 열어 사용;
        printf("Cannot Open File.\n");										//예외처리;
        exit(1);
    }
    for(i=0; i<strlen(bit_string)/8; i++){
        buffer = fgetc(fp);													//파일에서 한 문자씩 받아와 decode하는 과정;
        for(j=0; j<8; j++){													//shift로 밀면서 1과 AND연산하여;
            decode_bit_string[now++] = ( change & (buffer>>shift--) ) + 48;	//+48해주어 character로 변환하는 과정;
        }
        shift = 7;															//shift초기화;
    }
    buffer = fgetc(fp);
    for(i=0; i<strlen(bit_string)%8; i++){									//나눠떨어지지 않을경우 남은 부분도 decode;
        decode_bit_string[now++] = ( change & (buffer>>shift--) ) + 48;
    }
    printf("'output.txt'에서 읽어와 압축풀기!!!\n");
    printf("\ndecode_bit_string = %s\n", decode_bit_string);				//decoding한 string 출력해서 확인하는 부분;
    fclose(fp);
    
    for(i=0; i<strlen(decode_bit_string)/MAX; i++){							//decoding한 string을 다시 압축된 code로 바꿔주는 부분;
        for(j = (MAX-1) + (i*MAX); j >= (i*MAX); j--){						//MAX크기마다 새로운 code이므로;
            if(decode_bit_string[j] == '1'){
                for(k=0; k < (MAX-1)-(j%MAX); k++){							//각 자리마다 다른수 1, 2, 4, 8,,, 를곱해야 하므로;
                    mul *= 2;												//해당 문제를 구현;
                }
                num = num + mul;
                mul = 1;
            }
        }
        decode_string[decode_size++] = num;									//숫자가 만들어지면 그 숫자를 decode_string에 넣어준다;
        num = 0; 															//num 초기화;
    }
    printf("decode_string = ");
    for(i=0; i<decode_size; i++){											//decode_string을 확인하는 부분;
        printf("%d", decode_string[i]);
    }
    printf("\n");
    //다시 문자열로 만드는 과정;
    STRUCT_INITIAL_TABLE(&RESTRUCT_TABLE, &RE_TABLE_SIZE);					//초기 HASH_TABLE 한문자크기만 있는 테이블 형성;
    if( (fp=fopen("recoveredinput.txt", "w")) == NULL ){							//recovered.txt를 write형식으로 열어 사용;
        printf("Cannot Open File.\n");										//예외처리;
        exit(1);
    }
    check = (char*)calloc(strlen(string), sizeof(char));
    restore = (char*)calloc(strlen(string), sizeof(char));					//다시 문자열로 만들기 위해 check하는 string과 복구하는 string;
    if( (!check) || (!restore)){
        printf("Cannot Allocate Memory.\n");								//Memory Allocation 예외처리;
        exit(1);
    }
    for(i=0; i<decode_size;i++){
        TEMP = RESTRUCT_TABLE;
        for( ; TEMP; TEMP = TEMP->link ){									//RESTRUCT_TABLE에는 초기 사전만 있으므로 구축해나가며 recoveredinput.txt에 넣는다;
            if(TEMP->code == decode_string[i]){
                fputs(TEMP->key, fp);										//만약 사전에 있으면, 그 key를 text에 입력;
                FOUND = TRUE;												//사전에 있는경우, 없는경우 다르므로 추가된 부분;
                break;
            }
            if(TEMP->link == NULL){											//TEMP를 사용해야 하므로 NULL이 되기전에 탈출;
                break;
            }
        }
        if( i>0 ){
            if(!FOUND){														//만약 사전에 없는 경우라면;
                SUB_STRING(check, LEAD->key, 0, 0);							//이전에 읽어온것에 맨앞 문자를 뒤에 붙이고;
                strcpy(restore, LEAD->key); 								//즉, 이전에읽어온것 + 이전에읽어온것의 맨앞문자;
                strcat(LEAD->key, check);
                strcpy(check, LEAD->key);									//그것을 text에 써준다;
                strcpy(LEAD->key, restore);									//그 후 과정은 사전에 있을경우와 동일;
                fputs(check, fp);
            }
            else{
                SUB_STRING(check, TEMP->key, 0, 0);
                strcpy(restore, LEAD->key);									//맨앞 문자만 잘라서 check에 저장;
                strcat(LEAD->key, check);									//이전에 읽은 key와 방금읽은 맨앞문자 붙인다;
                strcpy(check, LEAD->key);									//붙여서 check에 저장;
                strcpy(LEAD->key, restore);									//변형된 LEAD를 다시 복구하여야 하므로 복구;
                LEAD = TEMP;												//이전노드를 저장;
            }
            NEW = (L)calloc(1, sizeof(NODE));
            NEW->key = (char*)calloc(strlen(check), sizeof(char));			//NODE를 새로 생성하여;
            if( (!NEW) && (!NEW->key) ){
                printf("Cannot Allocate Memory.\n");
                exit(1);
            }
            strcpy(NEW->key, check); 										//만들어진 key를 삽입;
            NEW->code = RE_TABLE_SIZE++;									//code는 TABLE_SIZE에서 하나씩 늘려가며 입력;
            NEW->link = NULL;
            while(TEMP->link != NULL){
                TEMP = TEMP->link;											//맨 뒤에 노드를 찾아 그 부분에 연결시켜;
            }																//linked list를 유지한다;
            TEMP->link = NEW;
            if(!FOUND){
                LEAD = NEW;													//만약 못찾았을 경우에는 새로운 노드에서 읽은것이 되므로;
            }																//이전에 읽은 노드를 새로운 노드로 설정;
        }
        else{																//첫시행에는 이전노드가 없으므로;
            LEAD = TEMP;													//이전노드를 해당 시행의 노드로 설정;
        }
        FOUND = FALSE;														//찾은지 여부를 확인하는 변수 초기화;
    }
    fclose(fp);
    free(check);															//할당한 Memory 더 이상 사용하지 않을 경우 삭제;
    free(restore);
    TEMP = RESTRUCT_TABLE;													//재구축된 TABLE 사전을 출력해서 확인하는 부분;
    printf("\n< 재구축된 사전 >\n");
    for( i=0; TEMP; TEMP = TEMP->link){
        printf("RESTRUCT_TABLE[%d]'s key = %s code = %d\n", i++, TEMP->key, TEMP->code);
    }
}

void BINARY_COMPRESS(){
    FILE* fp;
    int i, j, k, NUM, now = 0, start = 0, order = 0, count = 0;
    int temp = TABLE_SIZE, share, rest, size, shift = 7;					//2진수화 과정이나 output.txt에 넣기위해 사용되는 변수
    share = rest = 0;
    unsigned char buffer = 0;
    do{
        temp /= 2;
        MAX++;																//binary code의 크기를 설정하기 위해 MAX변수 사용;
    }while(temp != 0);
    for(i=0; i<compress_size; i++){
        NUM = compress_string[i];											//compress_string에서 하나씩 받아와;
        share = NUM;														//그 수를 2진수로 바꾸는 과정;
        start = now;
        do{																	//압축된 코드를 bit_string으로 만드는과정;
            rest = share % 2;												//수를 2로 나눈 나머지를 순서대로 기입하면 2진수로 표현;
            share = share / 2;												//넣어줄때는 48을 더하여 ASCII CODE의 character로 넣어준다;
            decode_bit_string[now++] = rest + 48; 							//나눈 난머지를 역순으로 넣어줘야 하므로, 아직 사용하지않은;
            count++;
        }while(share != 0);													//decode_string을 이용하여 변경시킨다;
        for(k=0; k<MAX-count; k++){
            bit_string[order++] = '0';										//binary 사이즈가 MAX만큼씩 들어가야 나중에 구분이 가능하므로, 빈공간 0으로 채워준다;
        }
        for(j=now-1; j>=start; j--){
            bit_string[order++] = decode_bit_string[j];						//역순으로 목적지인 bit_string에 넣어준다;
        }
        count = 0;
    }
    printf("bit_string = %s\n", bit_string);
    printf("\n'output.txt'에 압축!!!\n");
    if( (fp=fopen("output.txt", "w")) == NULL ){							//파일입력을 위해 write버전으로 output.txt 연다;
        printf("Cannot Open File.\n");
        exit(1);
    }
    now = 0;
    for(i=0; i<strlen(bit_string)/8; i++){									//한 character에 8비트씩 넣으므로, /8만큼 시행;
        for(j=0; j<8; j++){
            buffer = buffer | ( (bit_string[now++]-48)<<(shift--) );		//shift를 이용하여 정보를 입력한다;
        }
        fputc(buffer, fp);													//입력된 문자를 파일에 기입하는과정;
        buffer = 0;
        shift = 7;
    }
    for(i=0; i<strlen(bit_string)%8; i++){									//나눠떨어지지 않는 경우가 있으므로 남은 부분을 압축해주는 과정;
        buffer = buffer | ( (bit_string[now++]-48)<<(shift--) );
    }
    fputc(buffer, fp);
    fclose(fp);																//파일사용이 끝나면 닫아준다;
}

void SUB_STRING(char* copy, char* string, int start, int end){				//C에서는 string을 사용할 수 없기에;
    int i;
    for(i=0; i <= (end-start); i++){										//시작부분과 끝부분을 parameter로 넣어주면;
        copy[i] = string[start + i];
    }																		//그 부분만큼 입력하고;
    copy[i] = '\0';															//마지막부분에 '\0'을 입력해 string임을 알려준다;
}

void COMPRESS(){
    int i, j, start = 0, end = 0, FOUND = FALSE;
    L TEMP = HASH_TABLE;
    L SEARCH = HASH_TABLE;													//압축을 하기위해 사용되는, 찾고, 새로만들고, 마지막 부분을 알려주는 변수들;
    L NEW = NULL;
    L LAST = NULL;
    char* check = (char*)calloc( strlen(string), sizeof(char) );			//길이만큼 처음에 저장하여 '\0'입력해주어 크기를 잘라주어 한번만 할당을해도 되게 구현;
    char* print = (char*)calloc( strlen(string), sizeof(char) );			//압축을 하기 위해 마지막 문자 제외한 prefix받아오는 문자열 필요;
    if( (!check) || (!print) ){
        printf("Cannot Allocate Memory.\n");
        exit(1);
    }
    for(i=0; i<strlen(string); i++){										//길이를 strlen(string)만큼 읽어야 모두 확인가능;
        SUB_STRING(check, string, start, end);										//string을 원하는 길이 start-end start에서 시작 end위치에서 끝나는 substring을 얻는다;
        for( ; !(FOUND) && TEMP ; TEMP = TEMP->link ){						//HASH는 linked-list로 연결되어 있으므로, 반복문 이렇게 설정;
            if(!strcmp(check, TEMP->key)){									//중간에 발견되면 뒤에는 볼필요가 없으므로;
                end++;														//한칸 더 늘려서 확인하기 위해 end늘리고;
                FOUND = TRUE;												//찾았다는 표시를 하고 for문 탈출;
                if(end == strlen(string)){
                    compress_string[compress_size++] = TEMP->code;			//마지막에 남은 부분을 넣어줘야 하므로, end가 string의 크기면 확인한 code입력;
                }
            }
            LAST = TEMP;													//마지막 NODE를 찾기위하여 변수 설정;
        }
        if(!FOUND){															//만약 마지막까지 찾지 못하였으면;
            NEW = (L)calloc(1, sizeof(NODE));								//새로운 NODE를 HASH_TABLE에 추가해야 하므로;
            NEW->code = TABLE_SIZE++;										//code는 TABLE_SIZE를 하나 늘리고 들어간다;
            NEW->link = NULL;												//link는 NULL;
            NEW->key = (char*)calloc(strlen(check), sizeof(char));			//key도 char*이므로 사용할때는 크기를 check만큼 받아오므로, 그 크기만큼 할당해서 사용;
            if( !(NEW->key) ){
                printf("Cannot Allocate Memory.\n");						//Memory할당할 수 없을 경우 Error = 예외처리;
                exit(1);
            }
            strcpy(NEW->key, check);										//없다고 판단된 key를 HASH_TABLE에 삽입;
            LAST->link = NEW;												//마지막 NODE를 저장한 LAST의 link를 NEW로 설정;
            SUB_STRING(print, string, start, end-1);
            for( ; SEARCH; SEARCH = SEARCH->link ){
                if(!strcmp(SEARCH->key, print)){							//마지막 문자를 제외하고 prefix를 HASH_TABLE에서 찾아서 압축;
                    compress_string[compress_size++] = SEARCH->code;		//compres_string에 해당 code를 입력;
                }
            }
            SEARCH = HASH_TABLE;
            if(end+1 == strlen(string)){									//만약 문장의 마지막 문자였다면, 앞에 문자만 넣어주면 마지막 문자는 생략되므로;
                SUB_STRING(print, string, end, end);
                for( ; SEARCH; SEARCH = SEARCH->link ){						//마지막문자까지 찾아서 넣어주는 부분 = 예외처리;
                    if(!strcmp(SEARCH->key, print)){
                        compress_string[compress_size++] = SEARCH->code;
                    }
                }
            }
            start = end; 													//마지막 문자부터 다시 읽으므로, start = end;
            end++;															//마지막 문자는 크기가 1이므로 이미 HASH_TABLE에 있으므로 두 character확인 하기 위해;
            SEARCH = HASH_TABLE;
        }
        TEMP = HASH_TABLE;													//TEMP는 다시 HASH_TABLE로 설정;
        FOUND = FALSE;														//다시 탐색을 수행해야 하므로 FOUND는 FALSE;
        LAST = NULL;														//LAST도 초기화;
    }
    free(check);
    free(print);															//사용후에는 더이상 필요없는 Memory해제;
}

void STRUCT_INITIAL_TABLE(L* TABLE, int* TABLE_SIZE){
    int i, j, now = 0;														//ASCII CODE 97 ~ 122;
    L TEMP, ORDER;
    for(i=0; i<strlen(string); i++){
        ALPHABET[string[i] - 97]++;											//빈도수를 파악하기 위하여 ALPHABET배열을 이용; 
    }
    for(i=0; i<26; i++){
        if(ALPHABET[i] != 0){												//0이 아니면 나왓다는 말이 되므로, 나온것만 사용; 
            TEMP = (L)calloc(1, sizeof(NODE));
            TEMP->key = (char*)calloc(1, sizeof(char));						//각각의 key값도 길이가 어떻게 될지 모르므로, 동적할당; 
            TEMP->key[0] = i + 97;									
            TEMP->link = NULL;												//새로운 노드를 만들어; 
            if(now == 0){
                (*TABLE) = TEMP;											//처음에는 Header가 가리키도록 한다; 
                ORDER = TEMP;
            }
            else{															//처음이 아닌경우 linked list로 연결; 
                ORDER->link = TEMP;
                ORDER = TEMP;
            }
            TEMP->code = now++;												//code값을 하나씩 늘려주며 입력; 
            (*TABLE_SIZE)++;													//총 TABLE_SIZE도 나중에 유용하게 사용되므로 구해준다; 
        }
    }
    TEMP = (*TABLE);
    now = 0;																//HASH_TABLE의 구성요소를 알아보기 위한 부분; 
    printf("\n< 초기 사전 상태 >\n"); 
    for( ; TEMP ; TEMP = TEMP->link ){
        printf("HASH_TABLE[%d] key = [%s] code = %d\n", now++, TEMP->key, TEMP->code );
    }
    printf("TABLE_SIZE = %d\n", *TABLE_SIZE);
}

void FREE(L* TABLE){
    L TEMP;
    TEMP = (*TABLE);														
    for( ; TEMP; TEMP = TEMP->link ){										//key부분을 각각 동적할당 해준거라; 
        free(TEMP->key);													//하나씩 모두 동적할당 해제; 
    }
    do{
        TEMP = (*TABLE);													//각 노드도 linked_list로 동적할당하여 연결한것이므로; 
        (*TABLE) = (*TABLE)->link;											//하나씩 모두 free해주는 부분; 
        free(TEMP);
    }while(HASH_TABLE == NULL);
}
