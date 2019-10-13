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

my_string_t create_string(){
	my_string_t string;
	string.current_size = 1;
	string.current_max_size = 39;
	my_string_t * pstring = &string;
	string.ptr = create_memory(pstring);
	return string;
}

int main(){
	my_string_t A = create_string();
	printf("%i", A.current_max_size);


//	my_string_t * pVladOS = &VladOS;
//	change_size(pVladOS);
//	printf("%i", VladOS.current_size);
	return 0;
}

