#ifndef D_LIST_H
#define D_LIST_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

// MACROS ---------------------------------------------------------------------------
#define MY_END_CUSTOM          "\033[0m"
#define MY_RED_AND_BOLD        "\033[1;31m"

#define LIST_DUMP(html, list)   list_dump(html, &list, __FILE__, __FUNCTION__, __LINE__)
#define PRINT_LIST_ERROR(code) fprintf(log, "\nCode: %d\n", code)
#define CHECK_LIST_ERROR(error, checked) ((error & checked) == checked)

#define LOG_FILE  "Doubly_Linked_List/list_logs.log"
#define HTML_FILE "Doubly_Linked_List/list_dump.html"
#define DOT_FILE  "Doubly_Linked_List/list.dot"
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
    LIST_OKAY                       = 0,  
    LIST_NOT_MEMORY                 = 1<<1,
    LIST_NULL_ADRESS                = 1<<2,
    LIST_INCORRECT_SIZE             = 1<<3,
    LIST_DATA_NULL_ASDRESSES        = 1<<4,
    LIST_INCORRECT_CAPACITY         = 1<<5,
    LIST_SERVICE_CELL_INCORRECT     = 1<<6,
    LIST_OCCUPIED_PTR_INCORRECT     = 1<<7,
    LIST_OCCUPIED_CONNECT_INCORRECT = 1<<8,
    LIST_FREE_CONNECT_INCORRECT     = 1<<9,
    LIST_FREE_PTR_INCORRECT         = 1<<10,
    LIST_INCORRECT_INDEX            = 1<<11,
    LIST_LOOP                       = 1<<12
};

// PROTOTYPES -----------------------------------------------------------------------

list_error_t list_init(list_t* list, size_t size);
list_error_t list_destroy(list_t* list);
list_error_t list_append(list_t* list, list_value_t value, size_t index);
list_error_t list_delete(list_t* list, size_t index);
list_error_t list_verify_occupied(list_t* list);
list_error_t list_verify(list_t* list);
list_error_t list_verify_free(list_t* list);
list_error_t list_realloc_up(list_t* list, size_t new_size);
int find_index_loc(list_t* list, size_t index);

void list_dump(FILE* html, list_t* list, const char* file_name, const char* func_name, int line);
#endif