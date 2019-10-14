//
// Created by Vladyslav Zadorozhny on 13.10.2019.
//
#include <stdio.h>
#include <stdlib.h>

typedef struct {
	/* something like class */
	int current_size;
	int current_max_size;
	char *ptr;
} my_string_t;

void change_max_size(my_string_t* arg){
	/* increases current max size of string */
	(*arg).current_max_size *= 2;
}

void change_current_size(my_string_t* arg){
	/* counts amount of symbols in the string */
	(*arg).current_size += 1;
}

char* create_memory(int arg){
	/* creates memory for string and returns pointer on its first element */
	char *ptr;
	ptr = malloc((arg + 1) * sizeof (*ptr));
	return ptr;
}

void write_in_string(my_string_t* arg_1, char arg_2[]){
	/* writes in string */

	int k = 0;
	while (1){
		int j = (*arg_1).current_size;
		if (arg_2[k] == '\0'){
			break;
		}
		*((*arg_1).ptr + j) = arg_2[k];
		change_current_size(arg_1);
		k++;
	}
}

my_string_t create_string(){
	/* function to create string  */
	my_string_t string;
	string.current_size = 1;
	string.current_max_size = 39;
	my_string_t * pstring = &string;
	string.ptr = create_memory(string.current_max_size);
	return string;
}

int main(){
	/* testing func */
	my_string_t A = create_string();
	write_in_string(&A, "HELLO");
	write_in_string(&A, " WORLD");
	for (int s = 1; s < 12; s++){
		printf("%c", *(A.ptr + s));
	}
	return 0;
}

