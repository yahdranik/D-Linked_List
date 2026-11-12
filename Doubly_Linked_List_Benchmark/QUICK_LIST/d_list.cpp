#include "d_list.h"

static int counter_dumps = 0;


int main()
{
    FILE* quick_list = fopen(QUICK_LIST_TESTS, "w");
    list_t list = {};
    list_init(&list, INIT_LIST_CAPACITY);

    for (size_t i = 0; i < 5000000; i += 100)
    {
        clock_t start = clock();
        benchmark(&list, 5000);
        clock_t end = clock();
        fprintf(quick_list, "%d\n", (int)(end - start));
    }

    list_destroy(&list);
    fclose(quick_list);
}

list_error_t list_init(list_t* list, size_t size)
{
    if (list == NULL) return LIST_ERROR;
    
    size++;                             // NOTE: эта штука специально, чтобы выделить место под первую служебную ячейку

    if (size < 2)
    {
        list->error |= LIST_INCORRECT_SIZE;
        return LIST_ERROR;
    }

    list->data = (list_value_t*) calloc(size, sizeof(list_value_t));
    if (list->data == NULL) 
    { 
        list->error |= LIST_NOT_MEMORY; 
        list_destroy(list);
        return LIST_ERROR; 
    }

    list->prefs = (int*) calloc(size, sizeof(int));
    if (list->prefs == NULL) 
    { 
        list->error |= LIST_NOT_MEMORY; 
        list_destroy(list);
        return LIST_ERROR; 
    }

    list->nexts = (int*) calloc(size, sizeof(int));
    if (list->nexts == NULL) 
    { 
        list->error |= LIST_NOT_MEMORY; 
        list_destroy(list);
        return LIST_ERROR; 
    }

    list->data[0] = INIT_LIST_VALUE;
    for (int i = 1; i < (int) size; i++)
    {
        list->data[i]  = INIT_LIST_VALUE;
        list->prefs[i] = 0 - (i - 1);
        list->nexts[i] = 0 - (i + 1);
    }

    list->nexts[size - 1] = 0;

    list->capacity = size;
    list->size = 0;
    list->first_free = 1;

    return LIST_SUCCESS;
}

list_error_t list_append(list_t* list, size_t index, list_value_t value) //NOTE: чтобы добалвлять в конец, index = size. В начало index = 0.
                                                                        //  Индексация с нуля.
{
    if (list == NULL) return LIST_ERROR;

    if (index > list->size)
    {
        printf("Incorrect index to append\n");
        return LIST_ERROR;
    }
    if (list->first_free == 0)
    {
        list_error_t res_resize = list_realloc_up(list, list->capacity * 2);
        if (res_resize == LIST_ERROR) return LIST_ERROR;
    }

    size_t cell         = list->first_free;
    size_t next_free    = (size_t) -list->nexts[cell];
    list->first_free = next_free;

    if (next_free != 0)
    {
        list->prefs[next_free] = 0;
    }

    int loc_ind = find_index_loc(list, index);
    if (loc_ind == -1)
    {
        return LIST_ERROR;
    }

    list->data[cell]  = value;
    list->prefs[cell] = list->prefs[loc_ind];
    list->nexts[cell] = loc_ind;
    list->nexts[list->prefs[loc_ind]] = (int) cell;
    list->prefs[loc_ind] = (int) cell;

    list->size += 1;
    return LIST_SUCCESS;
}

int find_index_loc(list_t* list, size_t index) // TODO: добавить коды ошибок
{
    if (index > list->size)
    {
        list->error |= LIST_INCORRECT_INDEX;
        return -1;
    }

    int current = 0;

    for (size_t i = 0; i < index + 1; i++)
    {
        current = list->nexts[current];
    }

    return current;
}

list_error_t list_realloc_up(list_t* list, size_t new_size)
{
    if (list == NULL) return LIST_ERROR;

    if (new_size <= list->capacity || new_size > LIST_MAX_CAPACITY)
        return LIST_ERROR;

    list_value_t* new_data = (list_value_t*) calloc(new_size, sizeof(list_value_t));
    int* new_nexts = (int*) calloc(new_size, sizeof(int));
    int* new_prefs = (int*) calloc(new_size, sizeof(int));

    if (new_data == NULL || new_nexts == NULL || new_prefs == NULL)
    {
        free(new_data);
        free(new_nexts);
        free(new_prefs);

        list->error |= LIST_NOT_MEMORY;
        return LIST_ERROR;
    }

    for (size_t i = 0; i < list->capacity; i++)
    {
        new_data[i] = list->data[i];
        new_nexts[i] = list->nexts[i];
        new_prefs[i] = list->prefs[i];
    }

    size_t old_capacity = list->capacity;

    for (size_t i = old_capacity; i < new_size; i++)
    {
        new_data[i] = INIT_LIST_VALUE;
        new_prefs[i] = -(int)(i - 1);
        new_nexts[i] = -(int)(i + 1);
    }

    new_prefs[old_capacity] = 0;
    new_nexts[new_size - 1] = 0;

    free(list->data);
    free(list->nexts);
    free(list->prefs);

    list->data = new_data;
    list->nexts = new_nexts;
    list->prefs = new_prefs;

    list->first_free = old_capacity;
    list->capacity = new_size;

    return LIST_SUCCESS;
}

list_error_t list_delete(list_t* list, size_t index)
{
    if (list == NULL) return LIST_ERROR;

    if (index >= list->size)
    {
        printf("Incorrect index to append\n");
        return LIST_ERROR;
    }

    int cell = find_index_loc(list, index);
    if (cell == -1)
    {
        return LIST_ERROR;
    }

    list->data[cell] = INIT_LIST_VALUE;
    int next_elem = list->nexts[cell];
    int pref_elem = list->prefs[cell];
    list->nexts[pref_elem] = next_elem;
    list->prefs[next_elem] = pref_elem;
    
    if (cell < (int) list->first_free)
    {
        list->prefs[cell] = 0;
        list->nexts[cell] = (int) -list->first_free;
        list->prefs[list->first_free] = -cell;
        list->first_free  = (size_t) cell; 
    }

    list->size--;

    return LIST_SUCCESS;
}

list_error_t list_destroy(list_t* list)
{
    if (list == NULL)
    {
        return LIST_ERROR;
    }

    if (list->data != NULL)
    {
        for (size_t i = 0; i < list->size; i++)
        {
            list->data[i] = INIT_LIST_VALUE;
        }

        free(list->data);
        list->data = NULL;
    }

    if (list->prefs != NULL)
    {
        for (size_t i = 0; i < list->capacity; i++)
        {
            list->prefs[i] = 0;
        }
        free(list->prefs);
        list->prefs = NULL;
    }

    if (list->nexts != NULL)
    {
        for (size_t i = 0; i < list->capacity; i++)
        {
            list->nexts[i] = 0;
        }
        free(list->nexts);
        list->nexts = NULL;
    }

    list->capacity = 0;
    list->size = 0;
    list->first_free = 0;

    return LIST_SUCCESS;
}

list_error_t list_verify(list_t* list)
{
    if (list == NULL) 
    {
        printf(MY_RED_AND_BOLD "ERROR: List adress is NULL\n" MY_END_CUSTOM);
        return LIST_ERROR;
    }

// LIST STRUCT PARAMETRES ---------------------------------------------------------------
    if (list->data == NULL || list->nexts == NULL || list->prefs == NULL)
    {
        list->error |= LIST_DATA_NULL_ASDRESSES;
        return LIST_ERROR;
    }

    if (list->capacity < 1 || list->capacity > LIST_MAX_CAPACITY)
    {
        list->error |= LIST_INCORRECT_CAPACITY;
        return LIST_ERROR;
    }

    if (list->size >= list->capacity)
    {
        list->error |= LIST_INCORRECT_SIZE;
        return LIST_ERROR;
    }

// SERVICE CELL --------------------------------------------------------------------------
    if (list->size == 0)
    {
        if (list->nexts[0] != 0 || list->prefs[0] != 0)
        list->error |= LIST_SERVICE_CELL_INCORRECT;
    }

    else
    {
        if (list->nexts[0] <= 0 || list->prefs[0] <= 0 || 
            list->prefs[0] >= (int) list->capacity || list->nexts[0] >= (int) list->capacity)
        list->error |= LIST_SERVICE_CELL_INCORRECT;
    }

// OCCUIED CELLS -------------------------------------------------------------------------
    list_error_t res_verify_occupied = list_verify_occupied(list);
    if (res_verify_occupied == LIST_ERROR) return LIST_ERROR;

// FREE CELLS ----------------------------------------------------------------------------
    list_error_t res_verify_free = list_verify_free(list);
    if (res_verify_free == LIST_ERROR) return LIST_ERROR;
    
    if (list->error == LIST_OKAY) return LIST_SUCCESS;
    else                          return LIST_ERROR;
}

list_error_t list_verify_occupied(list_t* list)
{
    int current = list->nexts[0];
    int pref    = 0;

    size_t counter = 0;
    while(current != 0)
    {
        if (current < 0 || current >= (int) list->capacity)
        {
            list->error |= LIST_OCCUPIED_PTR_INCORRECT;
            return LIST_ERROR;
        }

        if (list->nexts[current] < 0 || list->nexts[current] >= (int) list->capacity ||
            list->prefs[current] < 0 || list->prefs[current] >= (int) list->capacity)
        {
            list->error |= LIST_OCCUPIED_PTR_INCORRECT;
            return LIST_ERROR;
        }

        if (pref != list->prefs[current] || counter >= list->size)
        {
            list->error |= LIST_OCCUPIED_CONNECT_INCORRECT;
            return LIST_ERROR;
        }

        pref    = current;
        current = list->nexts[current];
        counter++; 
    }

    if (counter != list->size || pref != list->prefs[0])
    {
        list->error |= LIST_OCCUPIED_CONNECT_INCORRECT;
        return LIST_ERROR;
    }

    return LIST_SUCCESS;
}

void benchmark(list_t* list, size_t times)
{
    if (list == NULL) return;

    for (size_t i = 0; i < times; i++)
    {
        if (rand() % 2 == 0)
        {
            size_t idx = (size_t) rand() % (list->size + 1);
            list_append(list, idx, 676767.0);
        }

        else if (list->size > 0)
        {
            size_t idx = (size_t) rand() % list->size;
            list_delete(list, idx, 0);
        }
    }
}

list_error_t list_verify_free(list_t* list)
{
    int current    = (int) list->first_free;
    int pref       = 0;
    size_t counter = 0;
    while(current != 0)
    {
        if (current > 0 || -current >= (int) list->capacity)
        {
            list->error |= LIST_FREE_PTR_INCORRECT;
            return LIST_ERROR;
        }

        if (list->nexts[-current] > 0 || -list->nexts[-current] <= (int) list->capacity + 1 ||
            list->prefs[-current] > 0 || -list->prefs[-current] <= (int) list->capacity + 1)
        {
            list->error |= LIST_FREE_PTR_INCORRECT;
            return LIST_ERROR;
        }

        if (pref != list->prefs[-current] || counter >= list->capacity - list->size - 1)
        {
            list->error |= LIST_FREE_CONNECT_INCORRECT;
            return LIST_ERROR;
        }

        pref    = current;
        current = list->nexts[-current];
        counter++; 
    }

    if (counter != list->capacity - list->size - 1)
    {
        list->error |= LIST_FREE_CONNECT_INCORRECT;
        return LIST_ERROR;
    }
    return LIST_SUCCESS;
}