#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>  

#define MAXK 4
#define HALFK 2
#define MAX 500


typedef struct arec {
	char word[100];  // ���⿡�� �ܾ 3 �� �߿��� ù �ܾ �ִ´�.
	char pos[10];   // ���⿡�� ǰ�翭 �� ù ǰ�縦 �ִ´�.
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



			char* ptr = strtok(s, " ");      // " " ���� ���ڸ� �������� ���ڿ��� �ڸ�, ������ ��ȯ
			count = 0;
			if (strcmp(ptr, "\n") == 0) {
				continue;
			}

			while (ptr != NULL)               // �ڸ� ���ڿ��� ������ ���� ������ �ݺ�
			{



				if (count == 0) {
					strcpy(data.word, ptr);
				}

				else if (count == 7) {

					if (ptr[strlen(ptr) - 1] == '\n') {
						ptr[strlen(ptr) - 1] = '\0';    // ���๮�� ����
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



				ptr = strtok(NULL, " ");      // ���� ���ڿ��� �߶� �����͸� ��ȯ
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
	

	if (!root) {		//root�� NULL�̸�..
		root = (nodeptr)malloc(sizeof(node));	//nodeptr���·� nodeũ�⸸ŭ �Ҵ�޾� �����ּҴ� root�� ����   
		root->rec[0] = in_rec;				//key���� root->rec[0]�� �ִ´�. 
		root->ptr[0] = root->ptr[1] = NULL; //p0�� p1�� NULL�� �ִ´�. 
		root->fill_cnt = 1; return 1;         //fill_cnt�� 1 set���ش�. 
	}
	//root�� null�� �ƴ� ��� �̰�����.
	found = 0;								//found�� false�� set 
	curr = root;								//root�� ���� curr�� �ִ´�. 

	do {
		for (i = 0; i < curr->fill_cnt; i++) {     //curr�� ��ī��Ʈ�� i���� ���� �� ���� i�� ����  
			if (strcmp(in_rec.word,curr->rec[i].word)<0)
				break; //���࿡ Ŀ�� rec[i].stno�� in_key���� ũ�ٸ� �����
			else if (strcmp(in_rec.word ,curr->rec[i].word)==0) //in_key�� rec[i].stno�� ���ٸ� Ű���� �̹� ���� 
			{
				found = 1;                   //found�� true�� ��Ʈ 
				break;
			}
		}
		child = curr->ptr[i];                      //curr�� ptr[i]�� p�� �ְ� 
		if (child)									//p�� NULL�� �ƴϸ� curr�� stack�� �ְ� 
		{
			push(curr);
			curr = child;                           //p�� curr�̴�. 
		}
	} while (!found && child);                     //found�� false�� p�� null�� �� ���� leap node ã�� while�� 

	if (found) {	//�ߺ��� key���� ���� �� 
		
		if (found_count > 0) {
			printf("key already exists. \n");
		}
		return 0;
	}

	finished = 0;					//finished�� false�� set 
	do {
		// The pair <in_rec, child>  �� curr �� �־�� �Ѵ�. (��: in_rec, child �� ���� ä������ ����� �´�.)

		if (curr->fill_cnt < MAXK) {	//*********** curr �� ���ڸ��� �ִٸ� curr ��忡 �ִ´�.
			for (i = 0; i < curr->fill_cnt; i++) //i�� curr->fill_cnt���� �����ϸ鼭...
				if (strcmp(in_rec.word, curr->rec[i].word) < 0) //���� stno���� in_key�� �� ������ ����
					break;
			for (j = curr->fill_cnt; j > i; j--) { //�����Ϳ� ���ڵ� ��ĭ�� ����Ʈ 
				curr->ptr[j + 1] = curr->ptr[j];
				curr->rec[j] = curr->rec[j - 1];
			}

			curr->rec[i] = in_rec;	//���ڵ� ���� 
			curr->ptr[i + 1] = child;       //������ ���� 
			curr->fill_cnt++;         //fill ī��Ʈ ������Ŵ 

			return 0; // Terminate alg. with success.
		}
		else {							//************���ڸ��� ���ٸ� ���� �̿�  
			for (i = 0; i < MAXK; i++) {			//MAXK���� ���� ��Ŵ 
				if (strcmp(in_rec.word, curr->rec[i].word) < 0) // ���� stno���� in_key�� �� ������ ����
					break;
			}

			bnode.ptr[0] = curr->ptr[0]; //���� ptr[0]�� bnode.ptr[0]�� �ְ� 
			for (j = 0; j < i; j++) {			  // bnode�� MAXK���� ���ڵ�� �����͸� ����. 
				bnode.rec[j] = curr->rec[j];
				bnode.ptr[j + 1] = curr->ptr[j + 1];
			}
			bnode.rec[j] = in_rec; //���ڵ带 bnode.rec[3?]�� �ְ� 
			bnode.ptr[j + 1] = child;  //�����Ϳ��� NULL 
			j++; //j ������Ŵ 

			while (i < MAXK) {		//MAXK �� i���� ������ �Ʒ��� ������ �ݺ� 
				bnode.rec[j] = curr->rec[i];  //curr�� ���ڵ带 bnode�� �ְ� 
				bnode.ptr[j + 1] = curr->ptr[i + 1]; //�����͵� ����������.. 

				j++;		//i�� j�� ���� ��Ų��. 
				i++;
			}
			//new node 
			for (i = 0; i < HALFK; i++) {			//MAXK�� ������ ������.  	
				curr->ptr[i] = bnode.ptr[i];    //������ ���� ���� ���� ��忡 �־��ش�. [���ڵ�, ������]
				curr->rec[i] = bnode.rec[i];

			}
			curr->fill_cnt = HALFK;
			curr->ptr[i] = bnode.ptr[i];		//bnode.ptr[i]�� curr.ptr[i]�� ����Ų��. 
			curr->ptr[i + 1] = NULL;

			new_ptr = (nodeptr)malloc(sizeof(node)); //���� ���� �������� malloc���� �ڸ��Ҵ� 

			for (i = 0; i < HALFK; i++) {					//���� ���� ����忡 ���� 
				new_ptr->ptr[i] = bnode.ptr[i + 1 + HALFK];
				new_ptr->rec[i] = bnode.rec[i + 1 + HALFK];
				
			}
			new_ptr->ptr[i] = bnode.ptr[MAXK + 1];	//bnode�� ���� ����忡 ����. 
			new_ptr->fill_cnt = HALFK;
			in_rec = bnode.rec[HALFK];			//bnode�� ��� ���ڵ带 in_rec�� ���� 
			child = new_ptr;								//tptr�� child�� ���� 

			if (top >= 0) {							//root�� �ƴҰ�� 
				tptr = curr;
				curr = pop();
				finished = 0;
			}
			else {    //root�� �������� 
				//������ empty�� 
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
		printf("�˻� ���� : %s\n", str);
		return 0;
	}
	else {
		printf("�˻� ���� : %s ��� ���� : %d \n", str, height);
		printf("ù �ܾ� : %s \t ù ǰ�� : %s\n", curr->rec[i].word, curr->rec[i].pos);
		return 1;
	}
}