#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mvm.h"

#define MAXSTR 4000
#define PRINTSTR 20000


mvmcell* newNode(char* key, char *data){
	mvmcell* new;
	new = (mvmcell*)malloc(sizeof(mvmcell));
	new->key = (char*)calloc(MAXSTR, sizeof(char));
	new->data = (char*)calloc(MAXSTR, sizeof(char));
	if(new == NULL || new->key == NULL || new->data == NULL){
		ON_ERROR("Newnode(), Malloc Failed.\n");
	}
	strcpy(new->key, key);
	strcpy(new->data, data);
	new->next = NULL;
	return new;
}

mvm* mvm_init(void){
	mvm* m;
	m = (mvm*)calloc(1, sizeof(mvm));
	if (m == NULL){
		ON_ERROR("mvm_init(), Malloc Failed.\n");
	}
	m->head = (mvmcell*)calloc(1, sizeof(mvmcell));
	if (m->head == NULL){
		ON_ERROR("mvm_init(), Malloc Failed\n");
	}
	m->numkeys = 0;
	return m;
}

int mvm_size(mvm* m){
	if (m == NULL){
		return 0;
	}
	return m->numkeys;
}

void mvm_insert(mvm* m, char* key, char* data){
	mvmcell *new, *temp;
	if(key == NULL || m == NULL || data == NULL){
		return;
	}
	new = newNode(key, data);
	temp = m->head->next;
	m->head->next = new;

	new->next = temp;	
	m->numkeys += 1;
}

char* mvm_print(mvm* m){
	char *str, *printStr;
	mvmcell *head;
	if (m == NULL){
		return NULL;
	}
	head = m->head->next;
	str = (char*)calloc(MAXSTR, sizeof(char));
	printStr = (char*)calloc(PRINTSTR, sizeof(char));
	if(str == NULL || printStr == NULL){
		ON_ERROR("mvm_print, Malloc failed");
	}
	while(head != NULL){	
		sprintf(str, "[%s](%s) ", head->key, head->data);
		strcat(printStr, str);
		head = head->next;
	}
	free(str);
	return printStr;
}

void mvm_delete(mvm* m, char* key){
	mvmcell *head, *temp;
	if(key == NULL || m == NULL || m->head->next == NULL){
		return;
	}
	head = m->head;
	if(head->next->next == NULL){
		temp = head->next;
		free(temp->data);
		free(temp->key);
		free(temp);
		head->next = NULL;
		m->numkeys -= 1;
		return;
	}
	while(head->next != NULL){
		if(strcmp(head->next->key, key) == 0){
			temp = head->next;	
			head->next = head->next->next;
			free(temp->data);
			free(temp->key);
			free(temp);
			m->numkeys -= 1;
		}
		head = head->next;
	}
	return;
}

char* mvm_search(mvm* m, char* key){
	mvmcell* head;
	if(m == NULL || key == NULL){
		return NULL;
	}
	head = m->head->next;
	while(head != NULL){
		if(strcmp(head->key, key) == 0){
			return head->data;
		}
		head = head->next;
	}
	return NULL;
}

char** mvm_multisearch(mvm* m, char* key, int* n){
	char** vals;
	int i;
	mvmcell* head;	
	if(key == NULL || m == NULL || n == NULL){
		return NULL;
	}
	head = m->head->next;
	i = *n;
	vals = (char**)calloc(PRINTSTR, sizeof(char) * MAXSTR);
	if(vals == NULL){
		ON_ERROR("mvm_multisearch, Malloc failed.\n");
	}
	while(head != NULL){
		if(strcmp(head->key, key) == 0){
			vals[i] = head->data;
			i++;
		}
		head = head->next;
	}
	*n = i;
	return vals;
}

void mvm_free(mvm** p){
	mvm *m;
	mvmcell *temp;
	if(p == NULL){
		return;
	}
	m = *p;
	while(m->head != NULL){
		temp = m->head;
		m->head = m->head->next;
		free(temp->data);
		free(temp->key);
		free(temp);
	}
	free(m);

 	*p = NULL;
}
