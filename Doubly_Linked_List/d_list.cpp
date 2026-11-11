#include "d_list.h"

// TODO сложные тесты сделать и редми написать 
static int counter_dumps = 0;

int main()
{
    FILE* html = fopen(HTML_FILE, "w");

    fprintf(html,
            "<!DOCTYPE html>\n"
            "<html>\n"
            "<head>\n"
            "<meta charset=\"UTF-8\">\n"
            "<title>List dump</title>\n"
            "<style>body { background-color: #f2dbf8; font-family: monospace; }</style>\n"
            "</head>\n"
            "<body>\n");

    list_t list = {};
    list_init(&list, INIT_LIST_CAPACITY);

    list_testing(html, &list);
    list_destroy(&list);
    fprintf(html, "</body>\n</html>\n");

    fclose(html);
}

void list_testing(FILE* html, list_t* list)
{
    list_append(list, 676767., 0);
    list_append(list, 676767., 1);
    list_append(list, 676767., 2);
    list_append(list, 676767., 0);
    list_append(list, 676767., 1);
    list_append(list, 676767., 2);
    LIST_DUMP(html, list);
    list_delete(list, 1);
    LIST_DUMP(html, list);
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

list_error_t list_append(list_t* list, list_value_t value, size_t index) //NOTE: чтобы добалвлять в конец, index = size. В начало index = 0
                                                                        //  Индексация с нуля
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
    if (list == NULL || index > list->size) 
    {
        list->error |= LIST_INCORRECT_INDEX;
        return -1;
    }

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

void list_logger(FILE* log, list_error_code_t error)
{
    if      (CHECK_LIST_ERROR(error, LIST_NOT_MEMORY))                  fprintf(log, "FATAL: not memory\n");
    else if (CHECK_LIST_ERROR(error, LIST_NULL_ADRESS))                 fprintf(log, "ERROR: adress list is NULL\n");
    else if (CHECK_LIST_ERROR(error, LIST_DATA_NULL_ASDRESSES))         fprintf(log, "ERROR: adress list data is NULL\n");
    else if (CHECK_LIST_ERROR(error, LIST_INCORRECT_INDEX))             fprintf(log, "ERROR: referencing list with incorrect index\n");
    else if (CHECK_LIST_ERROR(error, LIST_INCORRECT_SIZE))              fprintf(log, "ERROR: list size is incorrect\n");
    else if (CHECK_LIST_ERROR(error, LIST_INCORRECT_CAPACITY))          fprintf(log, "ERROR: list capacity is incorrect\n");
    else if (CHECK_LIST_ERROR(error, LIST_SERVICE_CELL_INCORRECT))      fprintf(log, "ERROR: service cell [0] is incorrect\n");
    else if (CHECK_LIST_ERROR(error, LIST_OCCUPIED_PTR_INCORRECT))      fprintf(log, "ERROR: incorrect pointer on occupied cells\n");
    else if (CHECK_LIST_ERROR(error, LIST_OCCUPIED_CONNECT_INCORRECT))  fprintf(log, "ERROR: connect of occupied cells is incorrect\n");
    else if (CHECK_LIST_ERROR(error, LIST_FREE_PTR_INCORRECT))          fprintf(log, "ERROR: incorrect pointer on free cells\n");
    else if (CHECK_LIST_ERROR(error, LIST_FREE_CONNECT_INCORRECT))      fprintf(log, "ERROR: connect of free cells is incorrect\n");
    else if (CHECK_LIST_ERROR(error, LIST_LOOP))                        fprintf(log, "ERROR: list has loops\n");
    else fprintf(log, "OK!\n");
}

void create_list_dot_file(FILE* dot, const list_t* list)
{
    fprintf(dot, "/* Auto generate code */\n\n");
    fprintf(dot, "digraph G {\n");
    fprintf(dot, "rankdir=LR;\n");
    fprintf(dot, "bgcolor=\"#f2dbf8\";\n"
             "splines=ortho;\n"
             "forcelabels=true;\n"
             "nodesep=1.;\n"
             "ranksep=1.;\n"
             "forcelabels=true;\n");
    fprintf(dot, "node [fontsize=12, shape=box, style=\"filled, rounded\", color=\"#23033a\", fontname=\"Georgia\"];\n");
    fprintf(dot, "edge [labelfloat=false, labelangle=25, labeldistance=2.5];\n\n");

    fprintf(dot, "n0 [fillcolor=\"#a094ef\", label=<<table border=\"0\" cellborder=\"0\" cellspacing=\"0\" cellpadding=\"4\">\n" 
                        "<tr>\n"
                        "  <td colspan=\"2\" border=\"1\" sides=\"b\"><b>index:</b> 0</td>\n"
                        "</tr>\n"
                        "<tr>\n"
                        "  <td colspan=\"2\" border=\"1\" sides=\"b\"><b>value:</b> %X</td>\n"
                        "</tr>\n"
                        "<tr>"
                        "  <td border=\"1\" sides=\"r\"><b>next:</b> %d</td>\n"
                        "  <td><b>pref:</b> %d</td>\n"
                        "</tr>\n"
                        "</table>>];\n",
                    (unsigned int)(uintptr_t)list->data[0], list->nexts[0], list->prefs[0]);

    for (size_t i = 1; i < list->capacity; i++)
    {
        const char* color = (list->nexts[i] < 0 || list->prefs[i] < 0) ? "#DDA0DD" : "#c383ff";

        fprintf(dot, "n%zu [fillcolor=\"%s\", label=<<table border=\"0\" cellborder=\"0\" cellspacing=\"0\" cellpadding=\"4\">\n" 
                    "<tr>\n"
                    "  <td colspan=\"2\" border=\"1\" sides=\"b\"><b>index:</b> %zu</td>\n"
                    "</tr>\n"
                    "<tr>\n"
                    "  <td colspan=\"2\" border=\"1\" sides=\"b\"><b>value:</b> %X</td>\n"
                    "</tr>\n"
                    "<tr>"
                    "  <td border=\"1\" sides=\"r\"><b>next:</b> %d</td>\n"
                    "  <td><b>pref:</b> %d</td>\n"
                    "</tr>\n"
                    "</table>>];\n",
                i, color, i, (uint32_t) list->data[i], list->nexts[i], list->prefs[i]);
    }

    fprintf(dot, "\n");

    for (size_t i = 0; i < list->capacity - 1; i++)
    {
        fprintf(dot, "n%zu -> ", i);
    }
    fprintf(dot, "n%zu [style=invis, penwidth = 1];\n", list->capacity - 1);

    int current = (int)list->first_free;
    while (current != 0)
    {
        int next = -list->nexts[current];

        if (next != 0)
        {
            fprintf(dot, "n%d -> n%d [xlabel = \"free\", style = dotted, fontsize=12, constraint=false];\n", current, next);
        }
        current = next;
    }

    current = list->nexts[0];
    while (current != 0)
    {
        int next = list->nexts[current];

        if (next != 0)
        {
            fprintf(dot, "n%d -> n%d [style = filled, fontsize=12, constraint=false];\n", current, next);
        }
        current = next;
    }

    fprintf(dot, "n%zu [label=\"HEAD\", fillcolor=\"#c383ff\"];\n", list->capacity);
    fprintf(dot, "n%zu [label=\"TAIL\", fillcolor=\"#c383ff\"];\n", list->capacity + 1);

    fprintf(dot, "{ rank=same; n%zu; n%d }\n", list->capacity, list->prefs[0]);
    fprintf(dot, "{ rank=same; n%zu; n%d }\n", list->capacity + 1, list->nexts[0]);

    fprintf(dot, "n%zu -> n%d [style = filled, renwidth = 5.0]\n", list->capacity, list->prefs[0]);
    fprintf(dot, "n%zu -> n%d [style = filled, renwidth = 5.0]\n", list->capacity + 1, list->nexts[0]);

    fprintf(dot, "}\n");
}

void list_dump(FILE* html, const list_t* list, const char* file_name, const char* func_name, int line)
{
    FILE* log = fopen(LOG_FILE, "w");
    PRINT_LIST_ERROR((int) list->error);
    list_logger(log, list->error);

    fprintf(log,"Dump was summoned in\n"
                "      file: == %s ==\n"       
                "      function: == %s ==\n"      
                "      line: == %d ==\n", 
            file_name, func_name, line);
    fflush(log);
    fprintf(log,"\nCurrent parametres:\n");

    fprintf(log,"====== struct LIST_T ======\n"
                "       list: [%p]    \n"
                "       data: [%p]    \n"
                "       nexts: [%p]   \n"
                "       prefs: [%p]   \n"
                "       capacity: %zu \n"
                "                     \n"
                "======= DATA =======\n", 
                list, list->data, list->nexts, list->prefs, list->capacity);

// PRINT DATA ------------------------------------------------------------------------

    for (size_t i = 0; i < INIT_LIST_CAPACITY; i += 2)
    {
        fprintf(log,"[%zu]                [%zu]\n"
                    "value: %-10d   value: %-10d\n"
                    "next:  %-3d          next:  %-3d\n"
                    "prev:  %-3d          prev:  %-3d\n",
                    i, i + 1, (int) list->data[i], (int) list->data[i + 1], list->nexts[i], list->nexts[i + 1], list->prefs[i], list->prefs[i + 1]);
    }

// GRAPHIC DUMP ---------------------------------------------------------------------

    FILE* dot = fopen(DOT_FILE, "w");
    create_list_dot_file(dot, list);
    fclose(dot);

    char file_name_listing[1000] = {};
    char script[1000] = {};
    
    snprintf(file_name_listing, 1000, "report/list%d.png",  counter_dumps);
    snprintf(script, 1000, "dot -Tpng Doubly_Linked_List/list.dot -o Doubly_Linked_List/%s", file_name_listing);
    // printf("%s\n", script);
    system(script);
    // system("echo hui");

// HTML --------------------------------------------------------------------------------------

    fflush(log);

    FILE* text_file = fopen(LOG_FILE, "r");
    fprintf(html, "<img src=\"%s\" width=\"100%%\">\n", file_name_listing);
    fprintf(html, "<pre>\n");

    int c = 0;

    while ((c = fgetc(text_file)) != EOF)
    {
        fprintf(html, "%c", c);
    }

    fprintf(html, "</pre>\n");

    fclose(text_file);
    fclose(log);

    counter_dumps++;
}



