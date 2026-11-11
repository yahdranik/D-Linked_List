#ifndef D_LIST_H
#define D_LIST_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

// MACROS ---------------------------------------------------------------------------

#define MY_END_CUSTOM          "\033[0m"
#define MY_RED_AND_BOLD        "\033[1;31m"

#define LIST_DUMP(html, list)  list_dump(html, &list, __FILE__, __FUNCTION__, __LINE__)
#define PRINT_LIST_ERROR(code) fprintf(log, "\nCode: %d\n", code)

#define QUICK_LIST_TESTS "Doubly_Linked_List_Benchmark/QUICK_LIST/quick_list.txt"

// TYPES ----------------------------------------------------------------------------

typedef double list_value_t;
typedef uint32_t list_error_code_t;

struct list_t
{
    list_value_t* data;
    int* prefs;
    int* nexts;

    size_t capacity;
    size_t size;
    size_t first_free;

    list_error_code_t error;
};

// CONSTS --------------------------------------------------------------------------

const size_t INIT_LIST_CAPACITY     = 8;
const list_value_t INIT_LIST_VALUE  = 0xBEBEBE;
const size_t LIST_MAX_CAPACITY      = 4096;

enum list_error_t
{
    LIST_ERROR   = 1,
    LIST_SUCCESS = 0
};

enum list_error_codes
{
    LIST_OKAY,  
    LIST_NOT_MEMORY,
    LIST_NULL_ADRESS,
    LIST_INCORRECT_SIZE,
    LIST_DATA_NULL_ASDRESSES,
    LIST_INCORRECT_CAPACITY,
    LIST_SERVICE_CELL_INCORRECT,
    LIST_OCCUPIED_PTR_INCORRECT,
    LIST_OCCUPIED_CONNECT_INCORRECT,
    LIST_FREE_CONNECT_INCORRECT,
    LIST_FREE_PTR_INCORRECT,
    LIST_INCORRECT_INDEX,
    LIST_LOOP
};

// PROTOTYPES -----------------------------------------------------------------------

list_error_t list_init(list_t* list, size_t size);
list_error_t list_destroy(list_t* list);
list_error_t list_append(list_t* list, size_t index, list_value_t value);
list_error_t list_delete(list_t* list, size_t index, list_value_t value);
list_error_t list_verify_occupied(list_t* list);
list_error_t list_verify(list_t* list);
list_error_t list_verify_free(list_t* list);
list_error_t list_realloc_up(list_t* list, size_t new_size);
int find_index_loc(list_t* list, size_t index);

void list_dump(FILE* html, list_t* list, const char* file_name, const char* func_name, int line);
void benchmark(list_t* list, size_t times);

#endif