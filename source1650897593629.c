#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>

int n, k, p;

// Pomocne strukture

//Stek
unsigned long long ID=0;
struct Node;

struct Node *tmp_node;
struct Node *starting_node;
struct Node *winning_node=NULL;



//Queue
struct Queue {

	struct Node *node;
	struct Queue*next;

};
void enqueue(struct Queue**first, struct Queue**last, struct Node* node) {

	struct Queue *new_element = malloc(sizeof(struct Queue));
	
	new_element->node = node;
	if ((*first) == NULL)*first = *last=new_element;
	else {
		(*last)->next = new_element;
	}
	*last = new_element;

}

struct Node * dequeue(struct Queue**first, struct Queue**last) {
	struct Node * return_node = (*first) == NULL ? NULL : (*first)->node;

	if (*first) {
		struct Stack * old = *first;
		if (*first == *last)
			*first = NULL;
		else
			(*first) = (*first)->next;
		free(old);
		if ((*first) == NULL)*last = NULL;
	}

	return return_node;
}




struct Node {
	unsigned long long id;
	int from_bottle;
	int to_bottle;
	struct Node * parent;
	int *bottles;
	int status;
	int level;
	struct Node ** children;
	int children_number;
};


//LCG

int a = 429493445;
int c = 907633385;
unsigned long X0=19;

unsigned long lcg_generator() {
	return X0 = a * X0 + c;
}


int find_top_of_a_bottle(int * bottle, int index_of_bottom) {

	int i = 0;
	for (i = index_of_bottom;i < index_of_bottom + 4;i++) {
		if (bottle[i] == 0)break;
	}
	return i;
}

int can_move_to(struct Node * node, int index_of_bottle_from, int index_of_bottle_to) {
	int i;
	//trazimo prvu poziciju
	i = find_top_of_a_bottle(node->bottles, 4 * index_of_bottle_from);
	
	if (i == 4 * index_of_bottle_from)return 0; // boca od koje presipamo je prazna
	
	int j;

	j = find_top_of_a_bottle(node->bottles, 4 * index_of_bottle_to);

	if (j == 4 * (index_of_bottle_to + 1))return 0; // boca u koju presipamo je puna
	if (j == 4 * index_of_bottle_to)return 1; // boca u koju presipamo je prazna
	
	return node->bottles[i - 1] == node->bottles[j - 1];// gledamo da li je na vrhu obe bocice ista tecnost

}

void move_to(struct Node * node, int index_of_bottle_from, int index_of_bottle_to)
{

	int i;
	i = find_top_of_a_bottle(node->bottles, 4 * index_of_bottle_from);
	int j;
	j = find_top_of_a_bottle(node->bottles, 4 * index_of_bottle_to);

	i--;

	while ((j < (4 * index_of_bottle_to + 4) && i >= 4 * index_of_bottle_from &&(j==4*index_of_bottle_to || node->bottles[i] == node->bottles[j - 1]))) {
		node->bottles[j] = node->bottles[i];
		node->bottles[i] = 0;
		i--;
		j++;

	}



}

int get_number_of_children(struct Node* node){
	int cnt=0;
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			if (i != j && can_move_to(node, i, j)) {
				cnt++;
			}
		}
	}

	return cnt;
}
// pocetno stanje
struct Node * make_start_node() {
	struct Node * start_node = malloc(sizeof(struct  Node));
	start_node->id = ID++;
	start_node->bottles = malloc(n * 4 * sizeof(int));
	
	int nfb=n-k;

	for (int i = 0; i < nfb *4; i++)
	{
		start_node->bottles[i] = (i+4) / 4;
	}
	for (int i = 4* nfb; i < 4*n; i++)
	{
		start_node->bottles[i] = 0;
	}

	for (int i = (nfb * 4 - 1);i > 0;i--) {
		int swap_index=lcg_generator() % (i + 1);

		int tmp = start_node->bottles[i];
		start_node->bottles[i] = start_node->bottles[swap_index];
		start_node->bottles[swap_index] = tmp;
	}
	start_node->parent = NULL;
	start_node->status = 0;
	start_node->level = 0;
}

struct Node* make_child_from_node_by_moving(struct Node* node, int index_of_from_bottle, int index_of_to_bottle) {

	struct Node *newNode = malloc(sizeof(struct Node));
	newNode->bottles = malloc(n * 4 * sizeof(int));
	memcpy(newNode->bottles, node->bottles, n * 4 * sizeof(int));
	newNode->id = ID++;
	move_to(newNode, index_of_from_bottle, index_of_to_bottle);
	newNode->children_number = 0;
	newNode->from_bottle = index_of_from_bottle;
	newNode->to_bottle = index_of_to_bottle;
	newNode->level = node->level + 1;
	newNode->parent = node;
	
	newNode->children = NULL;
	newNode->status = 0;
	for (int i = 0;i < 4 * n;i++) {
		if (i % 4 != 0) {
			if (newNode->bottles[i] != newNode->bottles[i - 1])return newNode;
		}
	}
	newNode->status = 3;
	if (winning_node == NULL)winning_node = newNode;
	struct Node *tmp = newNode->parent;
	while (tmp) {
		tmp->status |= 1;
		tmp = tmp->parent;
	}


	return newNode;
}

void make_decision_tree() {

	// make queue
	struct Queue * first = NULL;
	struct Queue *last = NULL;

	struct Node* current = starting_node;

	while (current) {
		
		if (current->level < p) {

			current->children_number = get_number_of_children(current);
			current->children = calloc(current->children_number, sizeof(struct Node*));

			int cnt = 0;

			for (int i = 0; i < n; i++)
			{
				for (int j = 0; j < n; j++)
				{
					if (i != j && can_move_to(current, i, j)) {

						current->children[cnt] = make_child_from_node_by_moving(current, i, j);
						if (current->children[cnt]->status == 0 ) {
							enqueue(&first, &last, current->children[cnt]);
						}


						cnt++;

					}
				}
			}

		}

		current = dequeue(&first, &last);
	}

}

void make_move() {
	int bottle_from, bottle_to;
	printf("Unesite od koje boce presipate\n");
	scanf_s("%d", &bottle_from);

	printf("Unesite u koju boce presipate\n");
	scanf_s("%d", &bottle_to);
	if (bottle_from == bottle_to || bottle_from < 0 || bottle_to < 0 || bottle_from >= n || bottle_to >= n) {
		printf("Los unos!!!\n");
		return;
	}

	for (int i = 0; i < tmp_node->children_number; i++)
	{
		if (tmp_node->children[i]->to_bottle == bottle_to && tmp_node->children[i]->from_bottle == bottle_from) {
			tmp_node = tmp_node->children[i];
			return;
		}
	}
	
	printf("Taj potez ne postoji. Pokusajte ponovo\n");
}

void get_help() {

	for (int i = 0; i < tmp_node->children_number; i++)
	{
		if (tmp_node->children[i]->status&1) {
			printf("Predjeno u novo stanje tako sto je iz boce %d presuto u bocu %d\n", tmp_node->children[i]->from_bottle, tmp_node->children[i]->to_bottle);
			tmp_node = tmp_node->children[i];
			return;
		}
	}

}

void print_node(struct Node* node) {
	printf("\n");
	for (int i = 3;i >= 0;i--) {
		for (int j = 0; j <n; j++)
		{
			printf("%d ", node->bottles[j * 4 + i]);
		}
		printf("\n");
	}
	printf("\n");
}

void level_order_print_tree() {


	struct Queue * first = NULL;
	struct Queue *last = NULL;

	struct Node* current = starting_node;
	int level = -1;
	while (current) {

		if (current->children) {

			//print level
			if (level != current->level) {
				level = current->level;
				printf("-----------------------------------LEVEL%d--------------------------\n",level);
			}
			for (int i = 0;i < current->level;i++) {
				printf("  ");
			}
			int pid = current->parent == NULL ? -1 : current->parent->id;
			printf("LEVEL : %d || ID : %d || parentID : ", current->level,current->id);
			printf("%d\n", pid);
			for (int i = 3;i >= 0;i--) {
				for (int i = 0;i < current->level;i++) {
					printf("  ");
				}
				for (int j = 0; j < n; j++)
				{
					printf("%d ", current->bottles[j * 4 + i]);
				}
				printf("\n");
			}
			printf("\n");


			for (int i = 0; i < current->children_number; i++)
			{
				enqueue(&first, &last, current->children[i]);
			}

		}

		current = dequeue(&first, &last);
	}

}



void delete_tree(struct Node*root){

	struct Queue * first = NULL;
	struct Queue *last = NULL;

	struct Node* current = root;

	while (current) {

		for (int i = 0; i < current->children_number; i++)
		{
			enqueue(&first, &last, current->children[i]);
		}
			

		free(current->bottles);
		free(current->children);
		free(current);

		current = dequeue(&first, &last);
	}


}

void prune_tree() {
	struct Queue * first = NULL;
	struct Queue *last = NULL;

	struct Node* current = starting_node;

	while (current) {

		if (current->status == 0) {
			for (int i = 0; i < current->children_number; i++)
			{
				delete_tree(current->children[i]);
			}
			free(current->children);
			current->children = NULL;
			current->children_number = 0;
		}
		else
			for (int i = 0; i < current->children_number; i++)
			{
				enqueue(&first, &last, current->children[i]);
			}

		current = dequeue(&first, &last);
	}

}

int main() {

	printf("Unesite n:\n");
	scanf_s("%d", &n);

	printf("Unesite k:\n");
	scanf_s("%d", &k);

	printf("Unesite p:\n");
	scanf_s("%d", &p);
	
	starting_node= tmp_node = make_start_node();
	make_decision_tree();
	prune_tree();
	int opcija;
	

	printf("1. Ispisi trenutno stanje\n");
	printf("2. Odigraj potez\n");
	printf("3. Zatrazi pomoc\n");
	printf("4. Prikazi stablo ishoda\n");
	printf("5. Ispisi validno stanje\n");
	printf("0. Exit\n");
	scanf_s("%d", &opcija);

	if(starting_node->status==0 || starting_node->status==3){
		printf("Nije moguce doci do validnog resenja!!! \n");
		opcija = 0;
	}

	while (opcija != 0) {

		if (opcija == 1) {
			print_node(tmp_node);
		}
		else if (opcija == 2)
		{
			make_move();
			print_node(tmp_node);
			if ((tmp_node->status & 1) == 0) {
				printf("IZGUBILI STE!!!\n");
				printf("KRAJ IGRE!!!");
				break;
			}
			else if ((tmp_node->status & 3) == 3) {
				printf("POBEDILI STE!!!\n");
				printf("KRAJ IGRE!!!");
				break;
			}
		}
		else if (opcija == 3)
		{
			get_help();
			print_node(tmp_node);
			if ((tmp_node->status & 3) == 3) {
				printf("POBEDILI STE!!!\n");
				printf("KRAJ IGRE!!!");
				break;
			}
		}
		else if (opcija == 4)
		{
			level_order_print_tree();
		}
		else if (opcija == 5)
		{
			print_node(winning_node);
		}
		else if (opcija == 0)
		{
			break;
		}
		else {
			printf("Unos ne odgovara!\n");
		}

		printf("1. Ispisi trenutno stanje\n");
		printf("2. Odigraj potez\n");
		printf("3. Zatrazi pomoc\n");
		printf("4. Prikazi stablo ishoda\n");
		printf("5. Ispisi validno stanje\n");
		printf("0. Exit\n");
		scanf_s("%d", &opcija);

	}

	delete_tree(starting_node);
	system("pause");
	return 0;
}

