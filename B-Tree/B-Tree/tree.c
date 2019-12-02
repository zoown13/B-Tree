#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>  

#define MAXK 4
#define HALFK 2
#define MAX 500


typedef struct arec {
	char word[100];  // 여기에는 단어열 3 개 중에서 첫 단어를 넣는다.
	char pos[10];   // 여기에는 품사열 중 첫 품사를 넣는다.
} rectype;

int found_count = 0;
int top = -1;
typedef struct node* nodeptr;
typedef struct node {
	int fill_cnt;
	rectype rec[MAXK];
	nodeptr ptr[MAXK + 1];
}node;
typedef struct big_node {
	rectype rec[MAXK + 1];
	nodeptr ptr[MAXK + 2];
}big_node;

typedef struct two_Bn {
	nodeptr ptr[2 * MAXK + 1];
	rectype rec[2 * MAXK];
}two_Bnode;
nodeptr root = NULL;
nodeptr stack[MAX];

FILE* Fopen(char* fname, char* fmode);

void push(nodeptr node);
nodeptr pop();

void insert_btree();
int insert_arec(rectype in_rec);

void seqprinter(nodeptr curr,FILE* fp_print);
int deletion(char* out_word);
void redistribution(nodeptr father, nodeptr L_sibling, nodeptr R_sibling, char wcase, int j);
int B_tree_Search(char* str);


int main() {
	FILE* fp_print = fopen("SeqData.txt", "w");
	char input;
	rectype a_insert;
	char word_s[100];
	int num;
	char pos_s[10];
	char find[100];

	insert_btree();
	found_count++;
	while (1) {
		while (getchar() != '\n') {
			continue;
		}
		
		printf("insert : I search : S Delete : D printf : P exit : E >> ");
		scanf("%c", &input);
		

		if (input == 'E' || input == 'e') {
			return 0;
		}
		else if (input == 'I' || input == 'i') {
			printf("input a record [word,pos] : ");
			scanf("%s %s", word_s, pos_s);
			strcpy(a_insert.word, word_s);
			strcpy(a_insert.pos, pos_s);
			top = -1;
			insert_arec(a_insert);
			fflush(stdout);
		}

		else if (input == 'S' || input == 's') {
			printf("Input Word : ");
			scanf("%s", find);
			B_tree_Search(find);
			
			
		}
		else if (input == 'D' || input == 'd') {
			printf("Input Word : ");
			scanf("%s", find);
			deletion(find);
			
		}
		else if (input == 'p' || input == 'P') {
			seqprinter(root,fp_print);
			fclose(fp_print);
			
		}
		else {
			printf("Input error \n");
		}
	}


	

	return 0;
}



FILE* Fopen(char* fname, char* fmode)
{
	FILE* fp;

	fp = fopen(fname, fmode);
	if (fp == NULL) {

		printf("cannot open file %s\n", fname);
		exit(1);
	}
	return fp;
}

void push(nodeptr node)
{
	top++;
	stack[top] = node;
	
}

nodeptr pop()
{
	return stack[top--];
}

void insert_btree()
{
		FILE* fp;
		rectype data;
		int n = 0;
		char word_i[100],chp;
		char s[200];
		int num;
		char pos[10];
		int count = 0;
		int total_line = 0;


		fp = Fopen("wordlist.txt", "r");

		root = NULL;
		while (1) {
			chp=fgets(s, 200, fp);
			strcpy(data.pos, "\0");
			total_line++;
			if (chp == NULL) {
				total_line--;
				break;
			}



			char* ptr = strtok(s, " ");      // " " 공백 문자를 기준으로 문자열을 자름, 포인터 반환
			count = 0;
			if (strcmp(ptr, "\n") == 0) {
				continue;
			}

			while (ptr != NULL)               // 자른 문자열이 나오지 않을 때까지 반복
			{



				if (count == 0) {
					strcpy(data.word, ptr);
				}

				else if (count == 7) {

					if (ptr[strlen(ptr) - 1] == '\n') {
						ptr[strlen(ptr) - 1] = '\0';    // 개행문자 제거
						strcpy(data.pos, ptr);
					}
					else {
						strcpy(data.pos, ptr);
					}
				}

				else if (!((ptr[0] >= 65 && ptr[0] <= 90) || (ptr[0] >= 97 && ptr[0] <= 122)) && count > 7) {
					ptr = strtok(NULL, " ");
					continue;
				}

				count++;



				ptr = strtok(NULL, " ");      // 다음 문자열을 잘라서 포인터를 반환
			}

			top = -1;
			insert_arec(data);

		} //while(1)

		printf("data input succeed\n");
		printf("insert b_tree count = %d \n", total_line);
		fclose(fp);

		
}

int insert_arec(rectype in_rec)
{
	int i, j;
	nodeptr curr, child, new_ptr, tptr = NULL;
	//rectype empty = { "\0","\0"};
	big_node bnode;
	int found, finished;
	

	if (!root) {		//root가 NULL이면..
		root = (nodeptr)malloc(sizeof(node));	//nodeptr형태로 node크기만큼 할당받아 시작주소는 root가 가짐   
		root->rec[0] = in_rec;				//key값을 root->rec[0]에 넣는다. 
		root->ptr[0] = root->ptr[1] = NULL; //p0과 p1에 NULL을 넣는다. 
		root->fill_cnt = 1; return 1;         //fill_cnt는 1 set해준다. 
	}
	//root가 null이 아닐 경우 이곳으로.
	found = 0;								//found를 false로 set 
	curr = root;								//root의 값을 curr에 넣는다. 

	do {
		for (i = 0; i < curr->fill_cnt; i++) {     //curr의 필카운트가 i보다 작을 때 까지 i를 증가  
			if (strcmp(in_rec.word,curr->rec[i].word)<0)
				break; //만약에 커의 rec[i].stno가 in_key보다 크다면 멈춘다
			else if (strcmp(in_rec.word ,curr->rec[i].word)==0) //in_key와 rec[i].stno가 같다면 키값이 이미 존재 
			{
				found = 1;                   //found를 true로 세트 
				break;
			}
		}
		child = curr->ptr[i];                      //curr의 ptr[i]를 p에 넣고 
		if (child)									//p가 NULL이 아니면 curr를 stack에 넣고 
		{
			push(curr);
			curr = child;                           //p가 curr이다. 
		}
	} while (!found && child);                     //found가 false고 p가 null일 때 까지 leap node 찾는 while문 

	if (found) {	//중복된 key값이 있을 때 
		
		if (found_count > 0) {
			printf("key already exists. \n");
		}
		return 0;
	}

	finished = 0;					//finished는 false로 set 
	do {
		// The pair <in_rec, child>  를 curr 에 넣어야 한다. (주: in_rec, child 는 값이 채워져서 여기로 온다.)

		if (curr->fill_cnt < MAXK) {	//*********** curr 에 빈자리가 있다면 curr 노드에 넣는다.
			for (i = 0; i < curr->fill_cnt; i++) //i를 curr->fill_cnt까지 증가하면서...
				if (strcmp(in_rec.word, curr->rec[i].word) < 0) //만약 stno보다 in_key가 더 작으면 멈춤
					break;
			for (j = curr->fill_cnt; j > i; j--) { //포인터와 레코드 한칸씩 시프트 
				curr->ptr[j + 1] = curr->ptr[j];
				curr->rec[j] = curr->rec[j - 1];
			}

			curr->rec[i] = in_rec;	//레코드 삽입 
			curr->ptr[i + 1] = child;       //포인터 삽입 
			curr->fill_cnt++;         //fill 카운트 증가시킴 

			return 0; // Terminate alg. with success.
		}
		else {							//************빈자리가 없다면 빅노드 이용  
			for (i = 0; i < MAXK; i++) {			//MAXK까지 증가 시킴 
				if (strcmp(in_rec.word, curr->rec[i].word) < 0) // 만약 stno보다 in_key가 더 작으면 멈춤
					break;
			}

			bnode.ptr[0] = curr->ptr[0]; //현재 ptr[0]을 bnode.ptr[0]에 넣고 
			for (j = 0; j < i; j++) {			  // bnode를 MAXK까지 레코드와 포인터를 넣음. 
				bnode.rec[j] = curr->rec[j];
				bnode.ptr[j + 1] = curr->ptr[j + 1];
			}
			bnode.rec[j] = in_rec; //레코드를 bnode.rec[3?]에 넣고 
			bnode.ptr[j + 1] = child;  //포인터에는 NULL 
			j++; //j 증가시킴 

			while (i < MAXK) {		//MAXK 가 i보다 작으면 아래의 동작을 반복 
				bnode.rec[j] = curr->rec[i];  //curr의 레코드를 bnode에 넣고 
				bnode.ptr[j + 1] = curr->ptr[i + 1]; //포인터도 마찬가지로.. 

				j++;		//i와 j를 증가 시킨다. 
				i++;
			}
			//new node 
			for (i = 0; i < HALFK; i++) {			//MAXK를 반으로 나눈다.  	
				curr->ptr[i] = bnode.ptr[i];    //빅노드의 앞의 반을 현재 노드에 넣어준다. [레코드, 포인터]
				curr->rec[i] = bnode.rec[i];

			}
			curr->fill_cnt = HALFK;
			curr->ptr[i] = bnode.ptr[i];		//bnode.ptr[i]는 curr.ptr[i]를 가르킨다. 
			curr->ptr[i + 1] = NULL;

			new_ptr = (nodeptr)malloc(sizeof(node)); //뒤의 남은 반을위해 malloc으로 자리할당 

			for (i = 0; i < HALFK; i++) {					//뒤의 반을 새노드에 저장 
				new_ptr->ptr[i] = bnode.ptr[i + 1 + HALFK];
				new_ptr->rec[i] = bnode.rec[i + 1 + HALFK];
				
			}
			new_ptr->ptr[i] = bnode.ptr[MAXK + 1];	//bnode의 끝을 새노드에 이음. 
			new_ptr->fill_cnt = HALFK;
			in_rec = bnode.rec[HALFK];			//bnode의 가운데 레코드를 in_rec에 넣음 
			child = new_ptr;								//tptr을 child에 넣음 

			if (top >= 0) {							//root가 아닐경우 
				tptr = curr;
				curr = pop();
				finished = 0;
			}
			else {    //root를 새로지정 
				//스텍이 empty면 
				tptr = (nodeptr)malloc(sizeof(node));
				tptr->rec[0] = in_rec;
				tptr->ptr[0] = root;
				tptr->ptr[1] = child;
				tptr->fill_cnt = 1;
				root = tptr;
				//total_height++;
				return 1;
			} // else.
		} // else.
	} while (!finished);

	

	return 0;
}



void seqprinter(nodeptr curr,FILE *fp_print)
{
	
	
	int i;
	if (curr) {
		for (i = 0; i < curr->fill_cnt; i++) {
			seqprinter(curr->ptr[i],fp_print);
			//printf("%s  %s\n", curr->rec[i].word, curr->rec[i].pos);
			fprintf(fp_print,"%s  %s\n", curr->rec[i].word, curr->rec[i].pos);
			
		}
		seqprinter(curr->ptr[i],fp_print);
		
	}
	
	return ;
}

int deletion(char* out_word)
{
	nodeptr R_siblings, L_siblings, father, Pt;
	nodeptr leftptr, rightptr;
	int i,j,k, found = 0,finished = 0;
	int R_OK, L_OK;
	nodeptr curr = root;
	do {
		for (i = 0; i < curr->fill_cnt; i++) {
			if (strcmp(out_word, curr->rec[i].word) < 0) {
				break;
			}
			else if (strcmp(out_word, curr->rec[i].word) == 0) {
				found = 1;
				break;
			}
		}
		if (found == 1) {
			break;
		}
		else {
			Pt = curr->ptr[i];
			if (Pt) {
				push(curr);
				curr = Pt;
			}
			else {
				break;
			}
		}
	} while (!found);
	if (!found) {
		printf("Key to delete does not exist.\n");
		return 0;
	}

	if (curr->ptr[0]) {
		Pt = curr->ptr[i + 1];
		push(curr, stack);
		
		while (Pt->ptr[0]) {
			push(Pt, stack);
			Pt = Pt->ptr[0];
		}

		curr->rec[i] = Pt->rec[0];
		curr = Pt;
		out_word = (Pt->rec[0]).word;
		i = 0;


	}

	finished = 0;
	do {

		for (j = i + 1; j < curr->fill_cnt; j++) {
			curr->rec[j - 1] = curr->rec[j];
			curr->ptr[j] = curr->ptr[j + 1];
		}
		curr->fill_cnt = curr->fill_cnt - 1;

		if (curr == root) {
			if (curr->fill_cnt == 0) {
				root = root->ptr[0];
				free(curr);
			}
			return 1;
		}
		if (curr->fill_cnt >= HALFK) {
			return 1;
		}

		father = pop(stack);
		for (j = 0; j <= father->fill_cnt; j++) {
			if (father->ptr[j] == curr) {
				break;
			}
		}

		if (j >= 1) {
			L_siblings = father->ptr[j - 1];
		}
		else
		{
			L_siblings = NULL;
		}

		if (j < father->fill_cnt) {
			R_siblings = father->ptr[j + 1];
		}
		else {
			R_siblings = NULL;
		}

		R_OK = 0;
		L_OK = 0;

		if (R_siblings && R_siblings->fill_cnt > HALFK)R_OK = 1;
		else if(L_siblings && L_siblings->fill_cnt > HALFK)L_OK = 1;
		if (R_OK || L_OK) {
			if (R_OK) {
				redistribution(father, curr, R_siblings, 'R', j);
			}
			else if (L_OK) {
				redistribution(father, L_siblings,curr , 'L', j);
			}
			
			return 1;
		}
		else {
			if (R_siblings) {
				leftptr = curr;
				rightptr = R_siblings;
			}
			else {
				leftptr = L_siblings; rightptr = curr;
			}

			for (i = 0; i < father->fill_cnt; i++) {
				if (father->ptr[i] == leftptr) {
					break;
				}
			}
			j = leftptr->fill_cnt;
			leftptr->rec[j] = father->rec[i];

			j++;
			for (k = 0; k < rightptr->fill_cnt; k++, j++) {
				leftptr->ptr[j] = father->ptr[k];
				leftptr->rec[j] = rightptr->rec[k];
			}
			leftptr->ptr[j] = rightptr->ptr[k];
			leftptr->fill_cnt += 1 + rightptr->fill_cnt;

			free(rightptr);
			printf("Merging has been done.\n");
			
			curr = father;
		}

	} while (!finished);

	
}

void redistribution(nodeptr father, nodeptr L_sibling, nodeptr R_sibling, char wcase, int j)
{
	int i, k, m, n, h;
	two_Bnode twoB;

	if (wcase == 'L') {
		j = j - 1;
	}

	else if (wcase == 'R') {
		j = j;
	}

	for (i = 0; i < L_sibling->fill_cnt; i++) {
		twoB.ptr[i] = L_sibling->ptr[i];
		twoB.rec[i] = L_sibling->rec[i];
	}

	twoB.ptr[i] = L_sibling->ptr[i];

	twoB.rec[i] = father->rec[j];
	i++;
	for (k = 0; k < R_sibling->fill_cnt; k++, i++) {
		twoB.ptr[i] = R_sibling->ptr[k];
		twoB.rec[i] = R_sibling->rec[k];
	}

	twoB.ptr[i] = R_sibling->ptr[k];

	h = i / 2;

	for (n = 0; n < h; n++) {
		L_sibling->ptr[n] = twoB.ptr[n];
		L_sibling->rec[n] = twoB.rec[n];

	}
	L_sibling->ptr[n] = twoB.ptr[n];
	L_sibling->fill_cnt = h;

	n++;
	for (m = 0; m < (i - h - 1); m++, n++) {
		R_sibling->ptr[m] = twoB.ptr[n];
		R_sibling->rec[m] = twoB.rec[n];
	}
	R_sibling->ptr[m] = twoB.ptr[n];
	R_sibling->fill_cnt = i - h - 1;

	father->rec[j] = twoB.rec[h];


}


int B_tree_Search(char* str) {
	int flag = 0;
	nodeptr P, curr = root;
	int i;
	int height = 1;

	do {
		for (i = 0; i < curr->fill_cnt; i++) {
			if (strcmp(str, curr->rec[i].word) < 0) {
				break;
			}
			else if (strcmp(str, curr->rec[i].word) == 0) {
				flag = 1;
				break;
			}
		}
		P = curr->ptr[i];
		if (!flag && P) {
			curr = P;
			height++;
		}
	} while (!flag && P);


	if (flag == 0) {
		printf("검색 실패 : %s\n", str);
		return 0;
	}
	else {
		printf("검색 성공 : %s 노드 깊이 : %d \n", str, height);
		printf("첫 단어 : %s \t 첫 품사 : %s\n", curr->rec[i].word, curr->rec[i].pos);
		return 1;
	}
}