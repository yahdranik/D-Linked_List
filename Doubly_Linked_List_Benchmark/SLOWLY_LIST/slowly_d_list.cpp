#include "slowly_d_list.h"

int main()
{
    FILE* slowly_list = fopen(SLOWLY_LIST_TESTS, "w");
    list_t list = {};
    list_init(&list, INIT_LIST_CAPACITY);
    
    for (size_t i = 0; i < 5000000; i += 100)
    {
        clock_t start = clock();
        benchmark(&list, 5000);
        clock_t end = clock();
        fprintf(slowly_list, "%d\n", (int)(end - start));
    }

    list_destroy(&list);
    fclose(slowly_list);
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
            list_delete(list, idx);
        }
    }
}

list_error_t list_init(list_t* list, size_t size)
{
    if (list == NULL) return LIST_ERROR;

    size++;

    if (size < 2)
    {
        list->error |= LIST_INCORRECT_SIZE;
        return LIST_ERROR;
    }

    list->data = (list_elem_t*) calloc(size, sizeof(list_elem_t));
    if (list->data == NULL)
    {
        list->error |= LIST_NOT_MEMORY;
        list_destroy(list);
        return LIST_ERROR;
    }

    list->data[0].value = INIT_LIST_VALUE;

    for (size_t i = 1; i < size; i++)
    {
        list->data[i].value = INIT_LIST_VALUE;
        list->data[i].prev  = 0 - (int)(i - 1);
        list->data[i].next  = 0 - (int)(i + 1);
    }

    list->data[size - 1].next = 0;

    list->capacity   = size;
    list->size       = 0;
    list->first_free = 1;

    return LIST_SUCCESS;
}

list_error_t list_append(list_t* list, size_t index,  list_value_t value)
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

    int cell         = list->first_free;
    int next_free    = -list->data[cell].next;
    list->first_free = next_free;

    if (next_free != 0)
    {
        list->data[next_free].prev = 0;
    }

    int loc_ind = find_index_loc(list, index);
    if (loc_ind == -1)
    {
        return LIST_ERROR;
    }

    list->data[cell].value = value;
    list->data[cell].prev  = list->data[loc_ind].prev;
    list->data[cell].next  = loc_ind;
    list->data[list->data[loc_ind].prev].next = cell;
    list->data[loc_ind].prev = cell;

    list->size += 1;
    return LIST_SUCCESS;
}

int find_index_loc(list_t* list, size_t index)
{
    if (index > list->size)
    {
        list->error |= LIST_INCORRECT_INDEX;
        return -1;
    }

    int current = 0;

    for (size_t i = 0; i < index + 1; i++)
    {
        current = list->data[current].next;
    }

    return current;
}

list_error_t list_realloc_up(list_t* list, size_t new_size)
{
    if (list == NULL) return LIST_ERROR;

    if (new_size <= list->capacity || new_size > LIST_MAX_CAPACITY)
        return LIST_ERROR;

    list_elem_t* new_data = (list_elem_t*) calloc(new_size, sizeof(list_elem_t));

    if (new_data == NULL)
    {
        list->error |= LIST_NOT_MEMORY;
        return LIST_ERROR;
    }

    for (size_t i = 0; i < list->capacity; i++)
    {
        new_data[i] = list->data[i];
    }

    size_t old_capacity = list->capacity;

    for (size_t i = old_capacity; i < new_size; i++)
    {
        new_data[i].value = INIT_LIST_VALUE;
        new_data[i].prev  = -(int)(i - 1);
        new_data[i].next  = -(int)(i + 1);
    }

    new_data[old_capacity].prev = 0;
    new_data[new_size - 1].next = 0;

    free(list->data);

    list->data       = new_data;
    list->first_free = old_capacity;
    list->capacity   = new_size;

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
    list->data[cell].value = INIT_LIST_VALUE;
    int next_elem = list->data[cell].next;
    int pref_elem = list->data[cell].prev;
    list->data[pref_elem].next = next_elem;
    list->data[next_elem].prev = pref_elem;

    if (cell < list->first_free)
    {
        list->data[cell].prev = 0;
        list->data[cell].next = -list->first_free;
        list->data[list->first_free].prev = -cell;
        list->first_free = cell;
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
            list->data[i].value = INIT_LIST_VALUE;
        }

        free(list->data);
        list->data = NULL;
    }

    list->capacity   = 0;
    list->size       = 0;
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

    if (list->data == NULL)
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
        if (list->data[0].next != 0 || list->data[0].prev != 0)
            list->error |= LIST_SERVICE_CELL_INCORRECT;
    }
    else
    {
        if (list->data[0].next <= 0 || list->data[0].prev <= 0 ||
            list->data[0].prev >= list->capacity ||
            list->data[0].next >= list->capacity)
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
    int current = list->data[0].next;
    int pref    = 0;

    size_t counter = 0;
    while (current != 0)
    {
        if (current < 0 || current >= list->capacity)
        {
            list->error |= LIST_OCCUPIED_PTR_INCORRECT;
            return LIST_ERROR;
        }

        if (list->data[current].next < 0 || list->data[current].next >= list->capacity ||
            list->data[current].prev < 0 || list->data[current].prev >= list->capacity)
        {
            list->error |= LIST_OCCUPIED_PTR_INCORRECT;
            return LIST_ERROR;
        }

        if (pref != list->data[current].prev || counter >= list->size)
        {
            list->error |= LIST_OCCUPIED_CONNECT_INCORRECT;
            return LIST_ERROR;
        }

        pref    = current;
        current = list->data[current].next;
        counter++;
    }

    if (counter != list->size || pref != list->data[0].prev)
    {
        list->error |= LIST_OCCUPIED_CONNECT_INCORRECT;
        return LIST_ERROR;
    }

    return LIST_SUCCESS;
}

list_error_t list_verify_free(list_t* list)
{
    int current    = list->first_free;
    int pref       = 0;
    size_t counter = 0;
    while (current != 0)
    {
        if (current > 0 || -current >= list->capacity)
        {
            list->error |= LIST_FREE_PTR_INCORRECT;
            return LIST_ERROR;
        }

        if (list->data[-current].next > 0 || -list->data[-current].next <= list->capacity + 1 ||
            list->data[-current].prev > 0 || -list->data[-current].prev <= list->capacity + 1)
        {
            list->error |= LIST_FREE_PTR_INCORRECT;
            return LIST_ERROR;
        }

        if (pref != list->data[-current].prev || counter >= list->capacity - list->size - 1)
        {
            list->error |= LIST_FREE_CONNECT_INCORRECT;
            return LIST_ERROR;
        }

        pref    = current;
        current = list->data[-current].next;
        counter++;
    }

    if (counter != list->capacity - list->size - 1)
    {
        list->error |= LIST_FREE_CONNECT_INCORRECT;
        return LIST_ERROR;
    }
    return LIST_SUCCESS;
}
