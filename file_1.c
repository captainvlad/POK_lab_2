#include <stdio.h>
#include <malloc.h>
#include <mem.h>

typedef struct {
	size_t capacity_m; // Розмір блока
	size_t size_m;       // Фактичний розмір стрічки
	char *data;       // Вказівник на блок пам'яті
} my_str_t;

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
//!===========================================================================
//! Створення та знищення стрічки.
//!===========================================================================

//! Створити стрічку із буфером вказаного розміру. Пам'ять виділяється динамічно.
//! Варто виділяти buf_size+1 для спрощення роботи my_str_get_cstr().
//! УВАГА! Ця функція -- аналог конструктора інших мов.
//! Всі решта функцій можуть вважати, що їм не буде передано жодної змінної,
//! до якої не застосували рівно один раз цю функцію.
//! Тобто, до того, як викликати будь-яку іншу функцію, до змінної типу
//! my_str_t слід рівно один раз за час її існування викликати  my_str_create().
//! (З біллю в серці згадуються закони Мерфі -- кожне твердження
//! обов'язково зрозуміють неправильно. Розказавши 20-30 раз, вирішив
//! записати. Тепер очікую збільшення кількості скарг на довжину опису...)
//! Після використання -- викличте my_str_free().
//! Зауважте -- за такого підходу, повторний виклик my_str_create до тієї ж
//! змінної, без виклику my_str_free, вестиме до memory leak.
//! Повертає 0, якщо все ОК, від'ємні числа, якщо сталася помилка.
int my_str_create(my_str_t *str, size_t buf_size) {
	char *arr = malloc(buf_size + 1);
	if (arr) {
		int i;
		for (i = 0; i < buf_size; ++i) {
			arr[i] = (char) (i);
		}
		arr[buf_size] = '\0';
	}
	str->data = arr;
	str->capacity_m = (size_t) buf_size + 1;
	str->size_m = (size_t) 0;
	return 0;
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
	str->size_m = strlen(str->data);
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
	str->data[str->capacity_m + 1] = '\0';
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
int my_str_pushback(my_str_t *str, char c){
	if (str == NULL){
		return -1;
	}
	if (str->size_m == str->capacity_m - 1){
		return -2;
	}
	str->data[str->size_m + 1] = c;
	str->size_m++;
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
//	printf("%i %i \n", str->size_m, str->capacity_m);
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
	for (int i = 0; i < sizeof(from); i++){
		my_str_insert_c(str, from->data[i], pos + i);
	}
	my_str_pushback(str, '\0');
	return 0;
}

//! Вставити C-стрічку в заданій позиції, змістивши решту символів праворуч.
//! За потреби -- збільшує буфер.
//! У випадку помилки повертає різні від'ємні числа, якщо все ОК -- 0.
int my_str_insert_cstr(my_str_t *str, char *from, size_t pos){
	my_str_t str_2;
	my_str_create(&str_2, 0);
	my_str_from_cstr(&str_2, from, sizeof(from) + 1);
	my_str_insert(str, &str_2, pos);
}

//! Додати стрічку в кінець.
//! За потреби -- збільшує буфер.
//! У випадку помилки повертає різні від'ємні числа, якщо все ОК -- 0.
int my_str_append(my_str_t *str, my_str_t *from) {
	// написати помилки -1, -2
	my_str_insert(str, from, my_str_size(str));
	return 0;
}
//! Додати С-стрічку в кінець.
//! За потреби -- збільшує буфер.
//! У випадку помилки повертає різні від'ємні числа, якщо все ОК -- 0.
int my_str_append_cstr(my_str_t *str, char *from){
	for (int i = 0; i < sizeof(from); i++){
		if (my_str_pushback(str, from[i]) == -2){
			my_str_reserve(str, str->capacity_m * 2);
			my_str_pushback(str, from[i]);
		}
	}
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
		my_str_pushback(to, from->data[beg + i]);
	}
}

//! C-string варіант my_str_substr().
//! Вважати, що в цільовій С-стрічці достатньо місц.
int my_str_substr_cstr(const my_str_t *from, char *to, size_t beg, size_t end){
	if (beg > from->size_m || beg > end){
		return -1;
	}
	for (int i = 0; i < end - beg; i++){
		to[i] = my_str_getc(from, beg + i);
	}
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

//! Якщо new_size менший за поточний розмір -- просто
//! відкидає зайві символи (зменшуючи size_m). Якщо
//! більший -- збільшує фактичний розмір стрічки,
//! встановлюючи нові символи рівними sym.
//! За потреби, збільшує буфер.
//! Сподіваюся, різниця між розміром буфера та фактичним
//! розміром стрічки зрозуміла?
//! У випадку помилки повертає різні від'ємні числа, якщо все ОК -- 0.
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
		return (size_t) (-1);
	}
	size_t k = 0;
	int i = 0;
	for (size_t n = from; n < str->size_m; n++) {
		if (str->data[n] == tofind->data[k]) {
			size_t start = k;
			if (tofind->size_m == k + 1) {
				return k;
			}
			else {
				k++;
			}

		}
	}
	return (size_t) -1;
}

//! Порівняти стрічки, повернути 0, якщо рівні (за вмістом!)
//! -1 (або інше від'ємне значення), якщо перша менша,
//! 1 (або інше додатне значення) -- якщо друга.
//! Поведінка має бути такою ж, як в strcmp.
int my_str_cmp(const my_str_t *str1, const my_str_t *str2){
    if (str1->size_m < str2->size_m) return -1;
    if (str2->size_m < str1->size_m) return 1;
    for (int i = 0; i <str1->size_m; i++) {
        if (*(str1->data + i) != *(str2->data + i)){
            return -2;
        }
    }
    return 0;
}

//! Порівняти стрічку із С-стрічкою, повернути 0, якщо рівні (за вмістом!)
//! -1 (або інше від'ємне значення), якщо перша менша,
//! 1 (або інше додатне значення) -- якщо друга.
//! Поведінка має бути такою ж, як в strcmp.
int my_str_cmp_cstr(const my_str_t *str1, const char *cstr2){
    my_str_t str2;
    my_str_from_cstr(&str2, cstr2, sizeof(cstr2) + 1);
    int fin = my_str_cmp(str1, &str2);
    return fin;
}

//! Знайти перший символ в стрічці, повернути його номер
//! або (size_t)(-1), якщо не знайдено. from -- місце, з якого починати шукати.
//! Якщо більше за розмір -- вважати, що не знайдено.
size_t my_str_find_c(const my_str_t *str, char tofind, size_t from) {
    // mykyta
    if (from > str->size_m) return -1;
    // не проходить по циклу
    printf("fff");
    for (int i = from; i < str->size_m; i++) {
        if (str->data[i] == tofind) {
            return i;
        }
    }
    return -1;
}

//! Знайти символ в стрічці, для якого передана
//! функція повернула true, повернути його номер
//! або (size_t)(-1), якщо не знайдено:
size_t my_str_find_if(const my_str_t *str, int (*predicat)(int)) {
	for (size_t i=0; i < str->size_m; i++) {
		if (predicat(str->data[i]) == 1) {
			printf("%c\n", str->data[i]);
			return i;
		}
	}
	return (size_t) -1;
}


//!===========================================================================
//! Ввід-вивід
//!===========================================================================

//! Прочитати стрічку із файлу. Читає цілий файл.
//! Не давайте читанню вийти за межі буфера! За потреби --
//! збільшуйте буфер.
//! Рекомендую скористатися fgets().
//! У випадку помилки повертає різні від'ємні числа, якщо все ОК -- 0.
int my_str_read_file(my_str_t *str, FILE *file);

//! Аналог my_str_read_file, із stdin.
int my_str_read(my_str_t *str);

//! Записати стрічку в файл:
//! У випадку помилки повертає різні від'ємні числа, якщо все ОК -- 0.
int my_str_write_file(const my_str_t *str, FILE *file);

//! Записати стрічку на консоль:
//! У випадку помилки повертає різні від'ємні числа, якщо все ОК -- 0.
int my_str_write(const my_str_t *str, FILE *file);

//! На відміну від my_str_read_file(), яка читає до кінця файлу,
//! читає по вказаний delimiter, за потреби
//! збільшує стрічку.
//! У випадку помилки повертає різні від'ємні числа, якщо все ОК -- 0.
int my_str_read_file_delim(my_str_t *str, FILE *file, char delimiter);


int main() {
	my_str_t solid;
	my_str_t solid_2;
	my_str_create(&solid, 0);
	my_str_create(&solid_2, 0);
	char c[] = "the wooden bober";
	my_str_from_cstr(&solid, c, sizeof(c) + 1);
//	my_str_from_cstr(&solid_2, "FUCKK", sizeof(5) + 1);

//	printf("%i %i\n", solid.capacity_m, solid.size_m);
//	printf("Cstring %s\n", c);
//	printf("%s %i %i\n", solid.data, solid.capacity_m, solid.size_m);
	my_str_substr(&solid, &solid_2, 0,2);
//	my_str_append(&solid, &solid_2);
//	my_str_append_cstr(&solid, "AAAAAAAa");
    printf("ddddd\n",  my_str_find_c(c, 98, 1000));
//	printf("%c %i %i\n",solid_2.data[1], solid_2.capacity_m, solid_2.size_m);
	}
