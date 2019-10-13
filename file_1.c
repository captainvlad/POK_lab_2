//
// Created by Vladyslav Zadorozhny on 13.10.2019.
//
#include <stdio.h>
#include <stdlib.h>

typedef struct {
	int current_size;
	int current_max_size;
	char *ptr;
} my_string_t;

void change_max_size(my_string_t* arg){
	(*arg).current_max_size *= 2;
}

void change_current_size(my_string_t* arg){
	(*arg).current_size += 1;
}

char* create_memory(my_string_t* arg){
	char *ptr;
	ptr = malloc(((*arg).current_max_size + 1) * sizeof (*ptr));
	return ptr;
}

void write_in_string(my_string_t* arg_1, char arg_2[]){
	printf("Not finished yet");
}

int main(){
	my_string_t VladOS;
	VladOS.current_size = 1;
	VladOS.current_max_size = 39;
	my_string_t * pVladOS = &VladOS;
	VladOS.ptr = create_memory(pVladOS);
	printf("%c", *(VladOS.ptr));


//	my_string_t * pVladOS = &VladOS;
//	change_size(pVladOS);
//	printf("%i", VladOS.current_size);
	return 0;
}

