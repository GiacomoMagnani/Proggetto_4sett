#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//LIST

#include <stdlib.h>

#define OK 1
#define NOK 0

typedef struct linked_list_node_t
{
    struct linked_list_node_t *previous;
    struct linked_list_node_t *next;
    void *data;
} linked_list_node;

/*
 * Generic Linked List Type: gll_t
 */
typedef linked_list_node *linked_list_node_ptr;
typedef struct
{
    int length;
    linked_list_node_ptr head;
    linked_list_node_ptr tail;
} linked_list;

typedef linked_list *linked_list_ptr;

linked_list_ptr linked_list_initialize()
{
    linked_list_ptr list = (linked_list_ptr)malloc(sizeof(linked_list));
    list->length = 0;
    list->head = list->tail = NULL;
    return list;
}

linked_list_node_ptr linked_list_node_create(void *data)
{
    linked_list_node_ptr node = (linked_list_node_ptr)malloc(sizeof(linked_list_node));
    node->data = data;
    node->previous = node->next = NULL;
    return node;
}

int linked_list_push_front(linked_list_ptr list, void *data)
{
    linked_list_node_ptr newNode = linked_list_node_create(data);

    if (list->length == 0)
    {
        list->tail = newNode;
    }
    else
    {
        list->head->previous = newNode;
        newNode->next = list->head;
    }
    list->head = newNode;
    list->length++;
    return OK;
}

int linked_list_push_back(linked_list_ptr list, void *data)
{
    linked_list_node_ptr newNode = linked_list_node_create(data);

    /* if list is empty */
    if (list->length == 0)
    {
        list->tail = newNode;
        list->head = newNode;
        list->length++;
        return OK;
    }
    list->tail->next = newNode;
    newNode->previous = list->tail;
    list->tail = newNode;
    list->length++;
    return OK;
}

linked_list_node_ptr find_by_name(linked_list_ptr list, int (*predicate)(void *, char *), char *name_to_find)
{
    linked_list_node_ptr current = list->head;
    while (current != NULL)
    {
        if (predicate(current->data, name_to_find))
        {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

int linked_list_remove_by_name(linked_list_ptr list, int (*predicate)(void *, char *), char *name_to_find, void (*free_data)(void *))
{
    linked_list_node_ptr current = find_by_name(list, predicate, name_to_find);
    if (current == NULL)
    {
        return NOK;
    }
    if (current->previous == NULL)
    {
        list->head = current->next;
    }
    else
    {
        current->previous->next = current->next;
    }
    
    if(current->next == NULL){
        list->tail = current->previous;
    }
    list->length--;
    free_data(current->data);
    free(current);
    return OK;
}

linked_list_node_ptr find_at_position(linked_list_ptr list, int position)
{
    if (position < 0 || position >= list->length)
    {
        return NULL;
    }
    linked_list_node_ptr current = list->head;
    for (int i = 0; i < position; i++)
    {
        current = current->next;
    }
    return current;
}

int linked_list_push_at_position(linked_list_ptr list, void *data, int position)
{
   if(position < 0 || position > list->length){
        return NOK;
    }

    linked_list_node_ptr newNode = linked_list_node_create(data);
    if(position == 0){
        linked_list_push_front(list, data);
        return OK;
    }
    if(position == list->length){
        linked_list_push_back(list, data);
        return OK;
    }
    linked_list_node_ptr current = find_at_position(list, position);
    if(current == NULL){
        return NOK;
    }

    newNode->next = current;
    newNode->previous = current->previous;
    current->previous->next = newNode;
    current->previous = newNode;
    list->length++;
    return OK;

}

int linked_list_remove_at_position(linked_list_ptr list, int position, void (*free_data)(void *))
{
    if (position < 0 || position >= list->length)
    {
        return NOK;
    }
    linked_list_node_ptr current = find_at_position(list, position);
    if (current == NULL)
    {
        return NOK;
    }

    if (current->previous == NULL)
    {
        list->head = current->next;
    }
    else
    {
        current->previous->next = current->next;
    }

    if (current->next == NULL)
    {
        list->tail = current->previous;
    }
    else
    {
        current->next->previous = current->previous;
    }
    list->length--;
    free_data(current->data);
    free(current);
    return OK;
}

int linked_list_pop_front(linked_list_ptr list, void (*free_data)(void *))
{
  return linked_list_remove_at_position(list, 0, free_data);
}

int linked_list_remove_node(linked_list_ptr list,linked_list_node_ptr node_to_delete, void (*free_data)(void *)){
    if(node_to_delete == NULL){
        return NOK;
    }
    if(node_to_delete->previous == NULL){
        list->head = node_to_delete->next;
    }
    else{
        node_to_delete->previous->next = node_to_delete->next;
    }

    if(node_to_delete->next == NULL){
        list->tail = node_to_delete->previous;
    }
    else{
        node_to_delete->next->previous = node_to_delete->previous;
    }
    list->length--;
    free_data(node_to_delete->data);
    free(node_to_delete);
    return OK;
}

int linked_list_destroy(linked_list_ptr list, void (*free_data)(void *))
{
    int destroyed_nodes = 0;
    linked_list_node_ptr current = list->head;
    linked_list_node_ptr next;

    while (current != NULL)
    {
        next = current->next;
        free_data(current->data);
        free(current);
        current = next;
        destroyed_nodes++;
    }
    free(list);
    return destroyed_nodes;
}

//END OF LIST


#define DEBUG_ENABLED 0
#define DEBUG(command) \
    if (DEBUG_ENABLED) \
    command

#define MAXIMUM_COMMAND_LENGHT 100
#define MAXIMUM_IDENTIFIER_LENGHT 256
#define HASHTABLE_SIZE 1000

int is_new_line(int c)
{
    return c == 13 || c == 10;
}

size_t hash_string(const char *str)
{
    size_t hash = 5381;
    size_t c;
    while ((c = *str++))
    {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash % HASHTABLE_SIZE;
}

typedef enum
{
    ADD_RECIPE,
    REMOVE_RECIPE,
    REFILL,
    ORDER
} COMMAND;

int is_command(const char *prefix, const char *input_string)
{
    return strncmp(prefix, input_string, strlen(prefix)) == 0;
}

void panic(const char *message)
{
    fprintf(stderr, "PANIC: %s\n", message);
    exit(-1);
}

COMMAND get_command(const char *command)
{
    if (is_command("aggiungi_ricetta", command))
    {
        return ADD_RECIPE;
    }
    else if (is_command("rimuovi_ricetta", command))
    {
        return REMOVE_RECIPE;
    }
    else if (is_command("rifornimento", command))
    {
        return REFILL;
    }
    else if (is_command("ordine", command))
    {
        return ORDER;
    }
    panic("Invalid command");
    return -1;
}

int should_load_truck(int current_time, int truck_loading_interval)
{
    return current_time % truck_loading_interval == 0;
}

typedef struct generic_list_t
{
    void *data;
    struct generic_list_t *next;
} generic_list;

typedef struct recipe_t
{
    char *name;
    int weight;
    int number_pending_orders;
    int last_expired_ingredient_check;
    linked_list_ptr ingredients;
} recipe;

typedef recipe *recipe_ptr;

typedef linked_list_ptr *hashtable;
typedef hashtable hashtable_ricette_t;
typedef hashtable hashtable_ordini_t;
typedef hashtable hashtable_warehouse_t;

typedef struct item_batch
{
    int quantity;
    int expiration_date;
} item_batch;
typedef item_batch *item_batch_ptr;
typedef linked_list_ptr batches_list;

typedef struct warehouse_ingredient
{
    char *name;
    int quantity_total_in_stock;
    batches_list batches;
} warehouse_item;
typedef warehouse_item *warehouse_item_ptr;

typedef struct ingredient_t
{
    char *name;
    int quantity;
    warehouse_item_ptr warehouse_item_info;
} ingredient;
typedef ingredient *ingredient_ptr;

typedef struct order
{
    recipe_ptr recipe;
    int quantity;
    int time;
    int total_weight;
    ingredient_ptr last_missing_ingredient;
} order;
typedef order *order_ptr;

// DEBUG FUNCTIONS
void print_order_list(linked_list_ptr list, char *list_name)
{
    if (!DEBUG_ENABLED)
        return;
    printf("Orders %s: ", list_name);
    linked_list_node_ptr current_node = list->head;
    while (current_node != NULL)
    {
        order_ptr current_order = (order_ptr)current_node->data;
        printf("(name:%s, time: %d, qnt:%d, weight:%d)", current_order->recipe->name, current_order->time, current_order->quantity, current_order->total_weight);
        current_node = current_node->next;
        if (current_node)
            printf(", ");
    }
    printf("\n");
}

void print_warehouse_item(warehouse_item_ptr item)
{
    if (!DEBUG_ENABLED)
        return;

    printf("Item: %s, quantity:%d \n", item->name, item->quantity_total_in_stock);
    printf("Batches:");
    linked_list_node_ptr current_batch = item->batches->head;
    while (current_batch != NULL)
    {
        item_batch_ptr current_item_batch = (item_batch_ptr)current_batch->data;
        printf("(qnt: %d, exp_date: %d)", current_item_batch->quantity, current_item_batch->expiration_date);

        current_batch = current_batch->next;
        if (current_batch)
            printf(", ");
    }
    printf("\n");
}

void print_entire_warehouse(hashtable_warehouse_t hashtable_warehouse)
{
    if (!DEBUG_ENABLED)
        return;
    printf("------WAREHOUSE------\n");
    for (int i = 0; i < HASHTABLE_SIZE; ++i)
    {
        linked_list_ptr current_list = hashtable_warehouse[i];
        if (!current_list || current_list->length <= 0)
        {
            continue;
        }
        linked_list_node_ptr current_item = current_list->head;
        while (current_item != NULL)
        {
            warehouse_item_ptr current_warehouse_item = (warehouse_item_ptr)current_item->data;
            print_warehouse_item(current_warehouse_item);
            current_item = current_item->next;
        }
    }
    printf("------END OF WAREHOUSE------\n");
}

void print_recipe_ingredients(linked_list_ptr ingredients)
{

    linked_list_node_ptr current_ingredient = ingredients->head;
    while (current_ingredient != NULL)
    {
        ingredient_ptr current_ingredient_data = (ingredient_ptr)current_ingredient->data;
        printf("(name: %s, qnt:%d)", current_ingredient_data->name, current_ingredient_data->quantity);

        // print_warehouse_item(current_ingredient_data->warehouse_item_info);
        current_ingredient = current_ingredient->next;
        if (current_ingredient)
            printf(", ");
    }
    printf("\n");
}

void print_recipes(hashtable_ricette_t hashtable_ricette)
{
    if (!DEBUG_ENABLED)
        return;
    printf("------RECIPES------\n");
    for (int i = 0; i < HASHTABLE_SIZE; i++)
    {
        linked_list_ptr current_list = hashtable_ricette[i];
        if (current_list->length <= 0)
        {
            continue;
        }
        linked_list_node_ptr current_recipe = current_list->head;
        while (current_recipe != NULL)
        {
            recipe_ptr current_recipe_data = (recipe_ptr)current_recipe->data;
            printf("[debug] Recipe: %s\n", current_recipe_data->name);
            printf("[debug] Weight: %d\n", current_recipe_data->weight);
            printf("[debug] Pending orders: %d\n", current_recipe_data->number_pending_orders);
            printf("[debug] Ingredients:");
            print_recipe_ingredients(current_recipe_data->ingredients);
            printf("\n");
            current_recipe = current_recipe->next;
        }
    }
}

hashtable create_hashtable()
{
    hashtable ht = (hashtable)calloc(HASHTABLE_SIZE, sizeof(linked_list_ptr));
    for (int i = 0; i < HASHTABLE_SIZE; ++i)
    {
        // ht[i] = linked_list_initialize();
        ht[i] = NULL;
    }
    return ht;
}

void *find_in_hashtable(hashtable hashtable, int (*name_match)(void *, char *), char *name)
{
    size_t hash_indice = hash_string(name);
    // printf("[debug] Finding: %s in hashtable at index %zu\n", name, hash_indice);
    linked_list_ptr current_recipe = hashtable[hash_indice];
    if (!current_recipe)
        return NULL;

    linked_list_node_ptr current_node = current_recipe->head;
    while (current_node != NULL)
    {
        void *current_recipe = (recipe_ptr)current_node->data;
        if (name_match(current_recipe, name))
        {
            return current_recipe;
        }
        current_node = current_node->next;
    }
    return NULL;
}

int recipe_name_match(void *recipe, char *recipe_name)
{
    return strcmp(((recipe_ptr)recipe)->name, recipe_name) == 0;
}

recipe_ptr find_recipe(hashtable_ricette_t hashtable_ricette, char *recipe_name)
{
    // printf("[debug] Finding recipe: %s\n", recipe_name);
    recipe_ptr found_recipe = (recipe_ptr)find_in_hashtable(hashtable_ricette, recipe_name_match, recipe_name);
    return found_recipe;
}

int warehouse_item_name_match(void *warehouse_item, char *name)
{
    return strcmp(((warehouse_item_ptr)warehouse_item)->name, name) == 0;
}

warehouse_item_ptr find_in_warehouse(hashtable_warehouse_t hashtable_warehouse, char *warehouse_item_name)
{
    warehouse_item_ptr found_item = (warehouse_item_ptr)find_in_hashtable(hashtable_warehouse, warehouse_item_name_match, warehouse_item_name);
    return found_item;
}

warehouse_item_ptr find_or_add_default_warehouse_item(hashtable_warehouse_t hashtable_warehouse, char *warehouse_item_name)
{
    warehouse_item_ptr found_item = find_in_warehouse(hashtable_warehouse, warehouse_item_name);
    if (!found_item)
    {
        found_item = (warehouse_item_ptr)malloc(sizeof(warehouse_item));
        found_item->name = strdup(warehouse_item_name);
        found_item->quantity_total_in_stock = 0;
        found_item->batches = linked_list_initialize();
        size_t hash_indice = hash_string(warehouse_item_name);
        if (!hashtable_warehouse[hash_indice])
        {
            hashtable_warehouse[hash_indice] = linked_list_initialize();
        }
        linked_list_push_back(hashtable_warehouse[hash_indice], found_item);
    }
    return found_item;
}

// GLOBALS
hashtable_ricette_t hashtable_ricette;
hashtable_warehouse_t hashtable_warehouse;
int current_time = 0;
int truck_capacity = 0;
linked_list_ptr ready_orders;
linked_list_ptr pending_orders;
int last_refill_time = 0;

void consume_input()
{
    char c = getchar();
    int printed = 0;
    while (!is_new_line(c))
    {
        if (!printed)
        {
            // printf("[debug] Consuming: ");
        }
        // printf("%c", c);
        c = getchar();
        printed = 1;
    }
    if (printed)
    {
        // printf("\n");
    }
}

ingredient_ptr create_new_ingredient(char *name, int quantity)
{
    ingredient_ptr new_ingredient = (ingredient_ptr)malloc(sizeof(ingredient));
    new_ingredient->name = strdup(name);
    new_ingredient->quantity = quantity;
    new_ingredient->warehouse_item_info = find_or_add_default_warehouse_item(hashtable_warehouse, name);
    return new_ingredient;
}

recipe_ptr create_new_recipe(char *name, int weight, linked_list_ptr ingredients)
{
    recipe_ptr new_recipe = (recipe_ptr)malloc(sizeof(recipe));
    new_recipe->name = strdup(name);
    new_recipe->weight = weight;
    new_recipe->number_pending_orders = 0;
    new_recipe->ingredients = ingredients;
    new_recipe->last_expired_ingredient_check = 0;
    return new_recipe;
}

void add_new_recipe(hashtable_ricette_t hashtable_ricette, char *recipe_name, linked_list_ptr ingredients, int total_recipe_quantity)
{
    size_t hash_indice = hash_string(recipe_name);
    linked_list_ptr current_list = hashtable_ricette[hash_indice];
    if (!current_list)
    {
        current_list = linked_list_initialize();
        hashtable_ricette[hash_indice] = current_list;
    }
    recipe_ptr new_recipe = create_new_recipe(recipe_name, total_recipe_quantity, ingredients);
    linked_list_push_back(current_list, new_recipe);
}

void handle_add_recipe_command()
{
    char recipe_name[MAXIMUM_IDENTIFIER_LENGHT];
    if (scanf("%s", recipe_name) != 1)
    {
        panic("Invalid input");
    }
    if (find_recipe(hashtable_ricette, recipe_name) != NULL)
    {
        printf("ignorato\n");
        consume_input();
        return;
    }
    linked_list_ptr ingredients = linked_list_initialize();
    int total_recipe_quantity = 0;
    while (!is_new_line(getchar()))
    {

        char ingredient_name[MAXIMUM_IDENTIFIER_LENGHT];
        int ingredient_quantity;
        if (scanf("%s %d", ingredient_name, &ingredient_quantity) != 2)
        {
            panic("Invalid input 2");
        }
        total_recipe_quantity += ingredient_quantity;
        ingredient_ptr new_ingredient = create_new_ingredient(ingredient_name, ingredient_quantity);
        linked_list_push_back(ingredients, new_ingredient);
        // printf("[debug] ingredient: '%s' quantity '%s' '%d'\n", recipe_name, ingredient_name, ingredient_quantity);
    }
    printf("aggiunta\n");
    add_new_recipe(hashtable_ricette, recipe_name, ingredients, total_recipe_quantity);
    // printf("[debug] Recipe DONE: %s\n", recipe_name);
    print_recipes(hashtable_ricette);
}

void free_ingredient(void *raw_ingredient)
{
    ingredient_ptr ingredient = (ingredient_ptr)raw_ingredient;
    free(ingredient->name);
    free(ingredient);
}
void free_recipe(void *raw_recipe)
{
    recipe_ptr recipe = (recipe_ptr)raw_recipe;
    free(recipe->name);
    linked_list_destroy(recipe->ingredients, free_ingredient);
    free(recipe);
}
void free_batch(void *raw_batch)
{
    item_batch_ptr batch = (item_batch_ptr)raw_batch;
    free(batch);
}

void remove_recipe(hashtable_ricette_t hashtable_ricette, recipe_ptr recipe_to_remove)
{
    size_t hash_indice = hash_string(recipe_to_remove->name);
    linked_list_ptr current_list = hashtable_ricette[hash_indice];
    linked_list_remove_by_name(current_list, recipe_name_match, recipe_to_remove->name, free_recipe);
}

void handle_remove_recipe_command()
{
    char recipe_name[MAXIMUM_IDENTIFIER_LENGHT];
    if (scanf("%s", recipe_name) != 1)
    {
        panic("Invalid input");
    }
    recipe_ptr found_recipe = find_recipe(hashtable_ricette, recipe_name);
    if (!found_recipe)
    {
        printf("non presente\n");
        return;
    }
    if (found_recipe->number_pending_orders > 0)
    {
        printf("ordini in sospeso\n");
        return;
    }
    remove_recipe(hashtable_ricette, found_recipe);
    print_recipes(hashtable_ricette);
    printf("rimossa\n");
}

order_ptr create_new_order(recipe_ptr recipe, int quantity)
{
    order_ptr new_order = (order_ptr)malloc(sizeof(order));
    new_order->recipe = recipe;
    new_order->quantity = quantity;
    new_order->time = current_time;
    new_order->total_weight = quantity * recipe->weight;
    new_order->last_missing_ingredient = recipe->ingredients->head->data;
    return new_order;
}

void delete_all_expired_batches(warehouse_item_ptr item)
{
    DEBUG(printf("[debug] Deleting expired batches for ingredient %s\n", item->name));
    linked_list_ptr batch_list = item->batches;

    while (batch_list->head != NULL)
    {
        item_batch_ptr current_batch_data = (item_batch_ptr)batch_list->head->data;
        if (current_batch_data->expiration_date <= current_time)
        {
            item->quantity_total_in_stock -= current_batch_data->quantity;
            linked_list_pop_front(batch_list, free_batch);
        }
        else
        {
            break;
        }
    }
}

int is_order_processable_now(order_ptr order)
{
    if (order->recipe->last_expired_ingredient_check < last_refill_time)
    {
        linked_list_node_ptr current_ingredient = order->recipe->ingredients->head;
        while (current_ingredient != NULL)
        {
            ingredient_ptr current_ingredient_data = (ingredient_ptr)current_ingredient->data;
            delete_all_expired_batches(current_ingredient_data->warehouse_item_info);
            current_ingredient = current_ingredient->next;
        }
        order->recipe->last_expired_ingredient_check = current_time;
    }

    linked_list_node_ptr current_ingredient = order->recipe->ingredients->head;
    while (current_ingredient != NULL)
    {
        const int recipe_ingredient_quantity = ((ingredient_ptr)current_ingredient->data)->quantity;
        const int order_ingredient_required_quantity = order->quantity * recipe_ingredient_quantity;
        const int warehouse_ingredient_quantity = ((ingredient_ptr)current_ingredient->data)->warehouse_item_info->quantity_total_in_stock;
        if (order_ingredient_required_quantity > warehouse_ingredient_quantity)
        {
            return 0;
        }
        current_ingredient = current_ingredient->next;
    }
    return 1;
}

void use_ingredient(ingredient_ptr ingredient, int quantity)
{
    linked_list_node_ptr current_batch = ingredient->warehouse_item_info->batches->head;
    while (current_batch != NULL && quantity > 0)
    {
        item_batch_ptr current_batch_data = (item_batch_ptr)current_batch->data;
        if (current_batch_data->quantity > quantity)
        {
            current_batch_data->quantity -= quantity;
            ingredient->warehouse_item_info->quantity_total_in_stock -= quantity;
            break;
        }
        else
        {
            quantity -= current_batch_data->quantity;
            ingredient->warehouse_item_info->quantity_total_in_stock -= current_batch_data->quantity;
            linked_list_pop_front(ingredient->warehouse_item_info->batches, free_batch);
        }
        current_batch = ingredient->warehouse_item_info->batches->head;
    }
}

void update_warehouse_on_order_processed(order_ptr order)
{
    linked_list_node_ptr current_ingredient = order->recipe->ingredients->head;
    while (current_ingredient != NULL)
    {
        const int recipe_ingredient_quantity = ((ingredient_ptr)current_ingredient->data)->quantity;
        const int order_ingredient_required_quantity = order->quantity * recipe_ingredient_quantity;
        DEBUG(printf("[debug] to prepare order %s @ time %d I am using ingredient %s for total quantity %d \n",
                     order->recipe->name, order->time, ((ingredient_ptr)current_ingredient->data)->name, order_ingredient_required_quantity));
        use_ingredient(current_ingredient->data, order_ingredient_required_quantity);
        current_ingredient = current_ingredient->next;
    }
}

void add_to_ready_orders(linked_list_ptr ready_orders, order_ptr order)
{

    int insert_position = 0;
    linked_list_node_ptr current_order = ready_orders->head;
    while (current_order != NULL)
    {
        order_ptr current_order_data = (order_ptr)current_order->data;
        if (current_order_data->time > order->time)
        {
            break;
        }
        insert_position++;
        current_order = current_order->next;
    }
    linked_list_push_at_position(ready_orders, order, insert_position);
}

void handle_add_order_command()
{
    char recipe_name[MAXIMUM_IDENTIFIER_LENGHT];
    int order_quantity;
    if (scanf("%s %d", recipe_name, &order_quantity) != 2)
    {
        panic("Invalid input");
    }
    recipe_ptr found_recipe = find_recipe(hashtable_ricette, recipe_name);
    if (!found_recipe)
    {
        printf("rifiutato\n");
        return;
    }
    printf("accettato\n");
    found_recipe->number_pending_orders++;
    order_ptr new_order = create_new_order(found_recipe, order_quantity);
    print_entire_warehouse(hashtable_warehouse);
    if (is_order_processable_now(new_order))
    {

        update_warehouse_on_order_processed(new_order);

        add_to_ready_orders(ready_orders, new_order);

        print_entire_warehouse(hashtable_warehouse);
        print_order_list(ready_orders, "ready");
        print_entire_warehouse(hashtable_warehouse);
        DEBUG(printf("[debug] Order is processable %s quantity %d\n", new_order->recipe->name, new_order->quantity));
    }
    else
    {
        linked_list_push_back(pending_orders, new_order);
        print_order_list(pending_orders, "pending");
    }
    // printf("[debug] Order: %s %d\n", recipe_name, order_quantity);
    // print_recipes(hashtable_ricette);
}

item_batch_ptr create_new_batch(int quantity, int expiration_date)
{
    item_batch_ptr new_batch = (item_batch_ptr)malloc(sizeof(item_batch));
    new_batch->quantity = quantity;
    new_batch->expiration_date = expiration_date;
    return new_batch;
}

void add_new_batch_to_warehouse_item(warehouse_item_ptr item, item_batch_ptr new_batch)
{
    item->quantity_total_in_stock += new_batch->quantity;
    // 1 find the right position to insert so that batches are sorted by expiration date in ascending order
    int insert_position = 0;
    linked_list_node_ptr current_batch = item->batches->head;
    while (current_batch != NULL)
    {
        item_batch_ptr current_batch_data = (item_batch_ptr)current_batch->data;
        if (current_batch_data->expiration_date > new_batch->expiration_date)
        {
            break;
        }
        insert_position++;
        current_batch = current_batch->next;
    }
    linked_list_push_at_position(item->batches, new_batch, insert_position);
}

void free_order(void *raw_order)
{
    order_ptr order = (order_ptr)raw_order;
    free(order);
}

void do_not_free(void *data)
{
    return;
}

void process_pending_orders()
{
    int some_order_processed = 0;
    linked_list_node_ptr current_order = pending_orders->head;
    while (current_order != NULL)
    {
        order_ptr current_order_data = (order_ptr)current_order->data;
        if (is_order_processable_now(current_order_data))
        {
            DEBUG(printf("\n\nNew processable order\n\n"));
            update_warehouse_on_order_processed(current_order_data);
            add_to_ready_orders(ready_orders, current_order_data);
            print_entire_warehouse(hashtable_warehouse);
            DEBUG(printf("[debug] Order is processable %s @ time %d, quantity %d\n", current_order_data->recipe->name, current_order_data->time, current_order_data->quantity));

            linked_list_node_ptr next_order = current_order->next;
            linked_list_remove_node(pending_orders, current_order, do_not_free);
            current_order = next_order;
            some_order_processed = 1;
            DEBUG(printf("\n\nDONE\n\n"));
        }
        else
        {
            current_order = current_order->next;
        }
    }
    if (some_order_processed)
    {
        print_order_list(ready_orders, "ready");
        print_order_list(pending_orders, "pending");
    }
}

void handle_warehouse_refill()
{

    while (!is_new_line(getchar()))
    {
        char ingredient_name[MAXIMUM_IDENTIFIER_LENGHT];
        int quantity;
        int expiration_date;
        if (scanf("%s %d %d", ingredient_name, &quantity, &expiration_date) != 3)
        {
            panic("Invalid input");
        }
        warehouse_item_ptr found_item = find_or_add_default_warehouse_item(hashtable_warehouse, ingredient_name);
        item_batch_ptr new_batch = create_new_batch(quantity, expiration_date);
        add_new_batch_to_warehouse_item(found_item, new_batch);
    }
    printf("rifornito\n");
    print_entire_warehouse(hashtable_warehouse);

    process_pending_orders();
    last_refill_time = current_time;
}

void add_new_order_to_truck_orders(linked_list_ptr truck_orders, order_ptr order)
{
    int insert_position = 0;
    linked_list_node_ptr current_order = truck_orders->head;
    while (current_order != NULL)
    {
        order_ptr current_order_data = (order_ptr)current_order->data;
        if (current_order_data->total_weight < order->total_weight)
        {
            break;
        }
        insert_position++;
        current_order = current_order->next;
    }
    linked_list_push_at_position(truck_orders, order, insert_position);
}

void load_truck()
{
    linked_list_ptr truck_orders = linked_list_initialize();

    int loaded_weight = 0;
    while (loaded_weight < truck_capacity && ready_orders->head != NULL)
    {
        order_ptr current_order = (order_ptr)ready_orders->head->data;
        if (loaded_weight + current_order->total_weight > truck_capacity)
        {
            break;
        }
        loaded_weight += current_order->total_weight;

        linked_list_pop_front(ready_orders, do_not_free);

        add_new_order_to_truck_orders(truck_orders, current_order);
        current_order->recipe->number_pending_orders--;
    }

    {
        linked_list_node_ptr current_order = truck_orders->head;
        while (current_order != NULL)
        {
            order_ptr current_order_data = (order_ptr)current_order->data;
            printf("%d %s %d\n", current_order_data->time, current_order_data->recipe->name, current_order_data->quantity);
            current_order = current_order->next;
        }
    }
    if (truck_orders->length <= 0)
    {
        printf("camincino vuoto\n");
    }
    linked_list_destroy(truck_orders, free_order);

    print_order_list(ready_orders, "ready");
    print_order_list(pending_orders, "pending");
}

int main()
{
    hashtable_ricette = create_hashtable();
    hashtable_warehouse = create_hashtable();
    ready_orders = linked_list_initialize();
    pending_orders = linked_list_initialize();
    int truck_loading_interval = 0;
    char raw_command[MAXIMUM_COMMAND_LENGHT];

    if (scanf("%d %d", &truck_loading_interval, &truck_capacity) != 2)
    {
        panic("Invalid input");
    }
    // consume_input();

    DEBUG(printf("[debug] Truck loading interval: %d\n", truck_loading_interval));
    DEBUG(printf("[debug] Truck capacity: %d\n", truck_capacity));

    while (scanf("%s", raw_command) == 1)
    {
        DEBUG(printf("[debug] Command: %s @ time %d\n", raw_command, current_time));
        switch (get_command(raw_command))
        {
        case ADD_RECIPE:
            handle_add_recipe_command();
            break;
        case REMOVE_RECIPE:
            handle_remove_recipe_command();
            break;
        case REFILL:
            handle_warehouse_refill();
            break;
        case ORDER:
            handle_add_order_command();
            break;
        default:
            panic("Invalid command");
            break;
        }

        ++current_time;
        if (should_load_truck(current_time, truck_loading_interval))
        {
            load_truck();
        }
    }
    return 0;
}