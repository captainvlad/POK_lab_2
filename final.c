//
// Created by Vladyslav Zadorozhny on 19.10.2019.
//

#include "final.h"
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <mem.h>

typedef struct {
	size_t capacity_m; // Розмір блока
	size_t size_m;       // Фактичний розмір стрічки
	char *data;       // Вказівник на блок пам'яті
} my_str_t;


int my_str_create(my_str_t *str, size_t buf_size) {
	char *arr = malloc(buf_size + 1);
	if (arr) {
		int i;
		for (i = 0; i < buf_size; ++i) {
			arr[i] =  i;
		}
	}
	str->data = arr;
	str->capacity_m = (size_t) buf_size + 1;
	str->size_m = 0;
	return 0;
}

//! Збільшує буфер стрічки, із збереженням вмісту,
//! якщо новий розмір більший за попередній,
//! не робить нічого, якщо менший або рівний.
//! (Як показує практика, це -- корисний підхід).
//! Для збільшення виділяє новий буфер, копіює вміст
//! стрічки (size_m символів -- немає сенсу копіювати
//! решту буфера) із старого буфера та звільняє його.
//! У випадку помилки повертає різні від'ємні числа, якщо все ОК -- 0.
int my_str_reserve(my_str_t *str, size_t buf_size) {
	if (buf_size > str->capacity_m) {
		char *new;
		new = malloc(buf_size + 1);   // <=== Виділили
		for (int i = 0; i < str->size_m; i++) {
			*(new + i) = *(str->data + i);
		}
		free(str->data);
		str->data = new;
		str->capacity_m = buf_size;
		return 0;
	}
	return -1;
}

//! Звільняє пам'ять, знищуючи стрічку.
//! Аналог деструктора інших мов.
void my_str_free(my_str_t *str) {
	free(str->data);
	str->size_m = 0;
	str->capacity_m = 0;
	str->data = NULL;
}

//! Створити стрічку із буфером вказаного розміру із переданої С-стрічки.
//! Якщо розмір, buf_size == 0, виділяє блок, рівний розміру С-стрічки, якщо
//! менший за її розмір -- вважати помилкою, не змінювати стрічку.
//! Пам'ять виділяється динамічно. Не слід викликати my_str_create(),
//! більше підійде my_str_resize() або my_str_reserve().
//! Коди завершення:
//! 0 -- якщо все ОК, -1 -- недостатній розмір буфера, -2 -- не вдалося виділити пам'ять
int my_str_from_cstr(my_str_t *str, const char *cstr, size_t buf_size) {
	size_t old = str->capacity_m;
	str->size_m = buf_size;

	if (str->capacity_m < buf_size) {
		my_str_reserve(str, buf_size);
	}
	if (buf_size == 0) {
		my_str_reserve(str, strlen(cstr));
	}

	memcpy(str->data, cstr, (size_t) strlen(cstr) + 1);

	if (str->capacity_m < old) {
		return -2;
	} else if (str->capacity_m < sizeof(cstr)) {
		return -1;
	}
	return 0;
}

//! Звільняє пам'ять, знищуючи стрічку.
//! Аналог деструктора інших мов.
//! Ремарка: free() нормально працює із NULL.

//!============================================================================
//! Інформація про стрічку
//!============================================================================

//! Повертає розмір стрічки.
//! Для нульового вказівника -- 0.
size_t my_str_size(const my_str_t *str) {
	return str->size_m;
}

//! Повертає розмір буфера.
//! Для нульового вказівника -- 0.
size_t my_str_capacity(const my_str_t *str) {
	return str->capacity_m;
}

//! Повертає булеве значення, чи стрічка порожня:
char my_str_empty(const my_str_t *str) {
	if (str->size_m == 0) return 0;
	else return -1;
}

//!===========================================================================
//! Доступ до символів стрічки
//!===========================================================================

//! Повертає символ у вказаній позиції, або -1, якщо вихід за межі стрічки,
//! включаючи переданий нульовий вказівник.
//! Тому, власне, int а не char
char my_str_getc(my_str_t* s, size_t index) {
	if (index > s->size_m || index < 0) return -1;
	return s->data[index];
}

char* strchr(const char *string, int c) {
	while( *string ) {
		if( *string == c )
			return (char *)string;
		string++;
	}

	return (char *)0;
}

//! Записує символ у вказану позиції (заміняючи той, що там був),
//! Повертає 0, якщо позиція в межах стрічки,
//! Поветає -1, не змінюючи її вмісту, якщо ні.
int my_str_putc(my_str_t *str, size_t index, char c){
	if (0 < index < str->size_m) {
		str->data[index] = c;
		return 0;
	}
	return -1;
}


//! Повернути вказівник на С-стрічку, еквівалентну str.
//! Вважатимемо, що змінювати цю С-стрічку заборонено.
//! Вважатимемо, що як тільки my_str_t змінено, цей вказівник
//! може стати некоректним.
//! Якщо в буфері було зарезервовано на байт більше за макс. розмір, можна
//! просто додати нульовий символ в кінці та повернути вказівник data.
const char *my_str_get_cstr(my_str_t *str){
	str->data[str->capacity_m] = '\0';
	return str->data;
}

//!===========================================================================
//! Модифікації стрічки, що змінюють її розмір і можуть викликати реалокацію.
//!===========================================================================
//! Якщо буфер недостатній -- ці функції збільшують його,
//! викликом my_str_reserve().
//! Розумним є буфер кожного разу збільшувати в 1.8-2 рази.
//! ==========================================================================

//! Додає символ в кінець.
//! Повертає 0, якщо успішно,
//! -1 -- якщо передано нульовий вказівник,
//! -2 -- помилка виділення додаткової пам'яті.

int my_str_pushback(my_str_t *str, char c) {
	if (str == NULL) {
		return -1;
	}
	if (str->size_m == str->capacity_m) {
		if (my_str_reserve(str, (str->capacity_m) * 2) == -1) {
			return -2;
		}
	}
	str->data[str->size_m] = c;
	str->size_m++;
	str->data[str->size_m + 1] = '\0';
	return 0;
}

//! Викидає символ з кінця.
//! Повертає його, якщо успішно,
//! -1 -- якщо передано нульовий вказівник,
//! -2 -- якщо стрічка порожня.
int my_str_popback(my_str_t *str){
	if (str == NULL){
		return -1;
	}
	else if(str->size_m == 0){
		return -2;
	}
	char element = str->data[str->size_m - 1];
	str->data[str->size_m - 1] = 0;
	str->size_m--;
	return element;
}

//! Копіює стрічку. Якщо reserve == true,
//! то із тим же розміром буферу, що й вихідна,
//! інакше -- із буфером мінімального достатнього розміру.
//! (Старий вміст стрічки перед тим звільняє, за потреби).
//! Повертає 0, якщо успішно, різні від'ємні числа для діагностики
//! проблеми некоректних аргументів.
//! Копіює стрічку. Якщо reserve == true,
//! то із тим же розміром буферу, що й вихідна,
//! інакше -- із буфером мінімального достатнього розміру.
//! (Старий вміст стрічки перед тим звільняє, за потреби).
//! Повертає 0, якщо успішно, різні від'ємні числа для діагностики
//! проблеми некоректних аргументів.
int my_str_copy(const my_str_t *from, my_str_t *to, int reserve) {
	if (from == NULL) {
		return -1;     //check the arguments
	}
	else if (to == NULL) {
		return -2;
	}

	size_t buf;
	if (reserve == 1) {
		buf = from->capacity_m;
	}
	else {
		buf = from->size_m;
	}

	if (to->size_m > 0) {
		free(to);
	}

	my_str_reserve(to, buf);
	for (size_t i=0; i < from->size_m; i++) {
		to->data[i] = from->data[i];
		to->size_m += 1;
	}
	return 0;

}

//! Очищає стрічку -- робить її порожньою. Складність має бути О(1).
//! Уточнення (чомусь ця ф-ція викликала багато непорозумінь):
//! стрічка продовжує існувати, буфер той самий, того ж розміру, що був,
//! лише містить 0 символів -- єдине, що вона робить, це size_m = 0.
void my_str_clear(my_str_t *str){
	str->size_m = 0;
}

//! Вставити символ у стрічку в заданій позиції, змістивши решту символів праворуч.
//! За потреби -- збільшує буфер.
//! У випадку помилки повертає різні від'ємні числа, якщо все ОК -- 0.
int my_str_insert_c(my_str_t *str, char c, size_t pos){
	if (str->size_m == str->capacity_m){
		my_str_reserve(str, str->capacity_m + 1);
	}
	char temp = c;
	char temp_2;
	for(int i = pos; i < str->capacity_m; i++){
		temp_2 = str->data[i];
		str->data[i] = temp;
		temp = temp_2;
	}
	str->size_m++;
	return 0;
}

//! Вставити стрічку в заданій позиції, змістивши решту символів праворуч.
//! За потреби -- збільшує буфер.
//! У випадку помилки повертає різні від'ємні числа, якщо все ОК -- 0.
int my_str_insert(my_str_t *str, my_str_t *from, size_t pos){
	for (int i = 0; i < from->size_m; i++){
		my_str_insert_c(str, from->data[i], pos + i);
	}
	return 0;
}


//! Вставити C-стрічку в заданій позиції, змістивши решту символів праворуч.
//! За потреби -- збільшує буфер.
//! У випадку помилки повертає різні від'ємні числа, якщо все ОК -- 0.
int my_str_insert_cstr(my_str_t *str, char *from, size_t pos){
	for (int i = 0; i < strlen(from); i++){
		my_str_insert_c(str, from[i], pos + i);
	}
	return 0;
}

//! Додати стрічку в кінець.
//! За потреби -- збільшує буфер.
//! У випадку помилки повертає різні від'ємні числа, якщо все ОК -- 0.
int my_str_append(my_str_t *str, my_str_t *from) {
	// написати помилки -1, -2
	if (my_str_insert(str, from, str->size_m) != 0){
		return -1;
	}
	return 0;
}

//! Додати С-стрічку в кінець.
//! За потреби -- збільшує буфер.
//! У випадку помилки повертає різні від'ємні числа, якщо все ОК -- 0.
int my_str_append_cstr(my_str_t *str, char *from){
	for (int i = 0; i < strlen(from); i++){
		if (my_str_pushback(str, from[i]) == -2){
			my_str_reserve(str, str->capacity_m * 2);
			my_str_pushback(str, from[i]);
		}
	}
	return 0;
}

//! Скопіювати підстрічку, із beg включно, по end не включно ([beg, end)).
//! Якщо end за межами початкової стрічки -- це не помилка, копіювати всі
//! символи до кінця. beg має бути в її межах -- якщо beg>size, це помилка.
//! За потреби -- збільшує буфер.
//! У випадку помилки повертає різні від'ємні числа, якщо все ОК -- 0.
int my_str_substr(my_str_t *from, my_str_t *to, size_t beg, size_t end){
	if (end > from->size_m){
		end = from->size_m;
	}
	else if (beg > from->size_m){
		return -1;
	}
	for (int i = 0; i < end - beg; i++){
		if (my_str_pushback(to, from->data[beg + i]) == -2){
			my_str_reserve(to, 2 * to->size_m);
			my_str_pushback(to, from->data[beg + i]);
		}
	}
}

//! C-string варіант my_str_substr().
//! Вважати, що в цільовій С-стрічці достатньо місц.
int my_str_substr_cstr(my_str_t *from, char *to, size_t beg, size_t end){
	if (beg > from->size_m || beg > end || beg < 0){
		return -1;
	}
	if (end > from->size_m) {
		end = from->size_m;
	}
	for (int i = 0; i < end - beg; i++){
		to[i] = my_str_getc(from, beg + i);
	}
	return 0;
}

//!===========================================================================
//! Маніпуляції розміром стрічки
//!===========================================================================




//! Робить буфер розміром, рівний необхідному:
//! так, щоб capacity_m == size_t. Єдиний "офіційний"
//! спосіб зменшити фактичний розмір буфера.
//! У випадку помилки повертає різні від'ємні числа, якщо все ОК -- 0.
int my_str_shrink_to_fit(my_str_t *str){
	str->capacity_m = str->size_m;
	return 0;
}

int my_str_resize(my_str_t *str, size_t new_size, char sym){
	if (new_size < str->size_m){
		str->size_m = new_size;
	}
	else if (new_size > str->size_m){
		if (new_size > str->capacity_m){
			my_str_reserve(str, new_size);
		}
		for (int i = 0; i < new_size - str->size_m; i++){
			str->data[str->size_m + 1] = sym;
		}
	}
	return 0;
}

//!===========================================================================
//! Функції пошуку та порівняння
//!===========================================================================

//! Знайти першу підстрічку в стрічці, повернути номер її
//! початку або (size_t)(-1), якщо не знайдено. from -- місце, з якого починати шукати.
//! Якщо більше за розмір -- вважати, що не знайдено.
size_t my_str_find(const my_str_t *str, const my_str_t *tofind, size_t from) {
	if (from > str->size_m) {
		return -1;
	}
	size_t start = -1;
	size_t k = 0;
	for (size_t i = from; i < str->size_m; i++) {
		if (str->data[i] == tofind->data[k]) {
			if (k == 0) {
				start = i;
			}
			if (k == tofind->size_m - 1) {
				return start;
			}
			k++;

		}
		else if (str->data[i] == tofind->data[0]) {
			k = 0;
			start = i;
			if (k == tofind->size_m - 1) {
				return start;
			}
			k++;
		}
		else {
			start = -1;
			k = 0;
		}

	}
	return start;
}

//! Порівняти стрічки, повернути 0, якщо рівні (за вмістом!)
//! -1 (або інше від'ємне значення), якщо перша менша,
//! 1 (або інше додатне значення) -- якщо друга.
//! Поведінка має бути такою ж, як в strcmp.
int my_str_cmp(const my_str_t *str1, const my_str_t *str2){
	if (str1->size_m < str2->size_m) return -1;
	if (str2->size_m < str1->size_m) return 1;
	for (int i = 0; i <str1->size_m; i++) {
		if (*(str1->data + i) > *(str2->data + i)){
			return 1;
		}
		else if (*(str1->data + i) < *(str2->data + i)){
			return  -1;
		}
	}
	return 0;
}

//! Порівняти стрічку із С-стрічкою, повернути 0, якщо рівні (за вмістом!)
//! -1 (або інше від'ємне значення), якщо перша менша,
//! 1 (або інше додатне значення) -- якщо друга.
//! Поведінка має бути такою ж, як в strcmp.
int my_str_cmp_cstr(my_str_t *str1, const char *cstr2){
	if (str1->size_m < strlen(cstr2)){
		return -1;
	}
	else if (str1->size_m > strlen(cstr2)){
		return 1;
	}
	for (int i = 0; i < strlen(cstr2); i++){
		if (cstr2[i] > my_str_getc(str1, i)){
			return -1;
		}
		if (cstr2[i] > my_str_getc(str1, i)) {
			return 1;
		}
	}
}

//! Аналог my_str_read_file, із stdin.
int my_str_read(my_str_t *str) {
	char sym = '\0';
	int start = 0;
	while (sym != '\n') {
		sym = getchar();
		if (str->size_m == str->capacity_m) {
			my_str_reserve(str, str->capacity_m * 2);
		}
		str->data[start] = sym;
		start++;
	}
	return 0;
}


int main() {
	my_str_t solid, solid_2;
	my_str_create(&solid_2, 2);
	my_str_create(&solid, 1);
	my_str_from_cstr(&solid_2, "ABC", strlen("ABC"));
//	my_str_from_cstr(&solid, "CAb", strlen("ABC"));
	my_str_read(&solid_2);
	my_str_get_cstr(&solid_2);
	printf("%s", solid_2.data);


//	my_str_pushback(&solid, 'F');
//	my_str_get_cstr(&solid_2);
//	printf("%s", solid.data);
//	my_str_insert_cstr(&solid, " LUCKY YOU ", 3);
//	my_str_insert(&solid, &solid_2, 13);
//	my_str_get_cstr(&solid);
//	printf("%s %i", solid.data, solid.capacity_m);
//	for (int i = 0; i < solid_2.size_m; i++) {
//		my_str_insert_c(&solid, solid_2.data[i], i);
//	}
//	my_str_insert(&solid, &solid_2, 0);
//	printf("%s %i %i", solid.data, solid_2.size_m, solid_2.capacity_m);
//	my_str_insert(&solid, &solid_2, 0);
//	printf("%i %i", solid.size_m, solid.capacity_m);
}
