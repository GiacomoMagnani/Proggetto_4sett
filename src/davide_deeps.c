#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define NODE_STRUCT typedef struct ll_n { struct ll_n *p; struct ll_n *n; void *d; } ll_n_t;
#define LIST_STRUCT typedef struct { int l; ll_n_t *h; ll_n_t *t; } ll_t;
#define NODE_PTR ll_n_t *
#define LIST_PTR ll_t *
#define NEW_LIST() ((LIST_PTR)malloc(sizeof(ll_t)))
#define NEW_NODE(data) ((NODE_PTR)malloc(sizeof(ll_n_t)))->d = (data);

NODE_STRUCT
LIST_STRUCT
typedef LIST_PTR linked_list_ptr;
typedef NODE_PTR linked_list_node_ptr;

typedef int (*predicate_fn)(void *, char *);
typedef void (*free_fn)(void *);
typedef int list_position;


#define LIST_INIT() ({ LIST_PTR ll = NEW_LIST(); ll->l = 0; ll->h = ll->t = NULL; ll; })
#define CREATE_NODE(d) ({ NODE_PTR nn = (NODE_PTR)malloc(sizeof(ll_n_t)); nn->d = (d); nn->p = nn->n = NULL; nn; })
#define LL_PUSH_FRONT(ll, d) ({ NODE_PTR nn = CREATE_NODE(d); \
    if (!(ll)->l) (ll)->t = nn; \
    else { (ll)->h->p = nn; nn->n = (ll)->h; } \
    (ll)->h = nn; ++(ll)->l; 1; })
#define LL_PUSH_BACK(ll, d) ({ NODE_PTR nn = CREATE_NODE(d); \
    if (!(ll)->l) (ll)->h = nn; \
    else { (ll)->t->n = nn; nn->p = (ll)->t; } \
    (ll)->t = nn; ++(ll)->l; 1; })
#define LL_FIND(ll, pred, name) ({ NODE_PTR cur = (ll)->h; while (cur) { if (pred(cur->d, name)) break; cur = cur->n; } cur; })
#define LL_REMOVE_NODE(ll, node, free_fn) ({ \
    if ((node)->p) (node)->p->n = (node)->n; else (ll)->h = (node)->n; \
    if ((node)->n) (node)->n->p = (node)->p; else (ll)->t = (node)->p; \
    (ll)->l--; free_fn((node)->d); free(node); 1; })
#define LL_DESTROY(ll, free_fn) ({ NODE_PTR cur = (ll)->h; while (cur) { NODE_PTR nxt = cur->n; free_fn(cur->d); free(cur); cur = nxt; } free(ll); })
#define LL_POP_FRONT(ll, free_fn) ({ NODE_PTR fh = (ll)->h; LL_REMOVE_NODE(ll, fh, free_fn); })
#define LL_IS_POS_INVALID(ll, pos) (pos < 0 || pos >= ll->l)

linked_list_ptr linked_list_initialize() { return LIST_INIT(); }
linked_list_node_ptr linked_list_node_create(void *d) { return CREATE_NODE(d); }
list_position linked_list_push_front(linked_list_ptr l, void *d) { return LL_PUSH_FRONT(l, d); }
list_position linked_list_push_back(linked_list_ptr l, void *d) { return LL_PUSH_BACK(l, d); }
linked_list_node_ptr find_by_name(linked_list_ptr l, predicate_fn pred, char *name) { return LL_FIND(l, pred, name); }
list_position linked_list_remove_by_name(linked_list_ptr l, predicate_fn pred, char *name, free_fn free_data) {
    linked_list_node_ptr node = find_by_name(l, pred, name);
    return node ? LL_REMOVE_NODE(l, node, free_data) : 0;
}
linked_list_node_ptr find_at_position(linked_list_ptr l, list_position pos) {
    if (LL_IS_POS_INVALID(l,pos)) return NULL; NODE_PTR cur = l->h; for (list_position i = 0; i < pos; i++) cur = cur->n; return cur;
}
list_position linked_list_push_at_position(linked_list_ptr l, void *d, list_position pos) {
    if (pos < 0 || pos > l->l) return 0;
    if (pos == 0) return linked_list_push_front(l, d);
    if (pos == l->l) return linked_list_push_back(l, d);
    NODE_PTR cur = find_at_position(l, pos); NODE_PTR nn = CREATE_NODE(d);
    nn->n = cur; nn->p = cur->p; cur->p->n = nn; cur->p = nn; l->l++; return 1;
}
list_position linked_list_remove_at_position(linked_list_ptr l, list_position pos, free_fn free_data) {
    if (pos < 0 || pos >= l->l) return 0; NODE_PTR cur = find_at_position(l, pos);
    return cur ? LL_REMOVE_NODE(l, cur, free_data) : 0;
}
list_position linked_list_pop_front(linked_list_ptr l, free_fn free_data) { return LL_POP_FRONT(l, free_data); }
list_position linked_list_remove_node(linked_list_ptr l, linked_list_node_ptr node, free_fn free_data) {
    return node ? LL_REMOVE_NODE(l, node, free_data) : 0;
}
void linked_list_destroy(linked_list_ptr l, free_fn free_data) { return LL_DESTROY(l, free_data); }


int current_time = 0;

#define MAXIMUM_COMMAND_LENGHT 100
#define MAXIMUM_IDENTIFIER_LENGHT 256
#define HASHTABLE_SIZE 1000
#define ISNEWLINE(c) (!(c!= 10 && c!=13))


size_t h(const char*b,int c){
    size_t d=5381,e;
    for(;(e=*b++);d=(d<<5)+d-d*2*c+e);
    return d%HASHTABLE_SIZE;
}
size_t hash_string(const char*b){ return h(b,1);}
size_t dbj_alternative(const char*b){ return h(b,0);}

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
    linked_list_node_ptr last_missing_ingredient_node;
} order;
typedef order *order_ptr;

void *find_in_hashtable(hashtable hashtable, int (*name_match)(void *, char *), char *name)
{
    linked_list_ptr current_recipe = hashtable[hash_string(name)];
    if (!current_recipe)
        return NULL;

    linked_list_node_ptr current_node = current_recipe->h;
    while (current_node != NULL)
    {
        void *current_recipe = (recipe_ptr)current_node->d;
        if (name_match(current_recipe, name))
        {
            return current_recipe;
        }
        current_node = current_node->n;
    }
    return NULL;
}

#define GENERIC_NAME_MATCH(fn_prefix, type) \
    int fn_prefix##_name_match(void *item_ptr, char *name_to_match) { return strcmp(((type)item_ptr)->name, name_to_match) == 0; }


#define RECIPE_NAME_MATCH(item_ptr, name_to_match) \
    GENERIC_NAME_MATCH(recipe_ptr, warehouse_item_ptr)

GENERIC_NAME_MATCH(recipe, warehouse_item_ptr)

recipe_ptr find_recipe(hashtable_ricette_t hashtable_ricette, char *recipe_name)
{
    recipe_ptr found_recipe = (recipe_ptr)find_in_hashtable(hashtable_ricette, recipe_name_match, recipe_name);
    return found_recipe;
}

#define WAREHOUSE_NAME_MATCH(item_ptr, name_to_match) \
    GENERIC_NAME_MATCH(warehouse_item, warehouse_item_ptr)

GENERIC_NAME_MATCH(warehouse_item, warehouse_item_ptr)


warehouse_item_ptr find_or_add_default_warehouse_item(hashtable_warehouse_t hashtable_warehouse, char *warehouse_item_name)
{
    warehouse_item_ptr found_item = find_in_hashtable(hashtable_warehouse, warehouse_item_name_match, warehouse_item_name);
    if (!found_item)
    {
        found_item = (warehouse_item_ptr)calloc(1,sizeof(warehouse_item));
        found_item->name = strdup(warehouse_item_name);
        found_item->batches = linked_list_initialize();
        const size_t h = hash_string(warehouse_item_name);
        hashtable_warehouse[h] = hashtable_warehouse[h] ? hashtable_warehouse[h] : linked_list_initialize();
        linked_list_push_back(hashtable_warehouse[h], found_item);
    }
    return found_item;
}

hashtable_ricette_t hashtable_ricette;
hashtable_warehouse_t hashtable_warehouse;

int truck_capacity = 0;
linked_list_ptr ready_orders;
linked_list_ptr pending_orders;
int last_refill_time = 0;

void consume_input()
{
    char c = getchar();
    while (!ISNEWLINE(c))
        c = getchar();
}

ingredient_ptr create_new_ingredient(char *name, int quantity)
{
    ingredient_ptr new_ingredient = (ingredient_ptr)malloc(sizeof(ingredient));
    if (new_ingredient == NULL) {
        panic("Memory allocation failed");
    }
    new_ingredient->name = strdup(name);
    new_ingredient->quantity = quantity;
    new_ingredient->warehouse_item_info = find_or_add_default_warehouse_item(hashtable_warehouse, name);
    return new_ingredient;
}

recipe_ptr create_new_recipe(char *name, int weight, linked_list_ptr ingredients)
{
    recipe_ptr new_recipe = calloc(1,sizeof(recipe));
    new_recipe->name = strdup(name);
    new_recipe->weight = weight;
    new_recipe->ingredients = ingredients;
    return new_recipe;
}

void add_new_recipe(hashtable_ricette_t hashtable_ricette, char *recipe_name, linked_list_ptr ingredients, int total_recipe_quantity)
{
    const size_t h = hash_string(recipe_name);
    hashtable_ricette[h] = hashtable_ricette[h] ? hashtable_ricette[h] : linked_list_initialize();
    linked_list_push_back(hashtable_ricette[h], create_new_recipe(recipe_name, total_recipe_quantity, ingredients));
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
    while (!ISNEWLINE(getchar()))
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
    }
    printf("aggiunta\n");
    add_new_recipe(hashtable_ricette, recipe_name, ingredients, total_recipe_quantity);
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
    size_t h = hash_string(recipe_to_remove->name);
    linked_list_ptr current_list = hashtable_ricette[h];
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
    printf("rimossa\n");
}

order_ptr create_new_order(recipe_ptr recipe, int quantity)
{
    order_ptr new_order = (order_ptr)calloc(1,sizeof(order));
    new_order->recipe = recipe;
    new_order->quantity = quantity;
    new_order->time = current_time;
    new_order->total_weight = quantity * recipe->weight;
    new_order->last_missing_ingredient_node = recipe->ingredients->h;
    return new_order;
}

void delete_all_expired_batches(warehouse_item_ptr item)
{
    linked_list_ptr batch_list = item->batches;
    while (batch_list->h)
    {
        item_batch_ptr current_batch_data = (item_batch_ptr)batch_list->h->d;
        if (current_batch_data->expiration_date > current_time)
            return;
        if (current_batch_data->expiration_date <= current_time)
        {
            item->quantity_total_in_stock -= current_batch_data->quantity;
            linked_list_pop_front(batch_list, free_batch);
            continue;
        }
        return;
    }
}



int is_order_processable_now(order_ptr order)
{
    if (order->recipe->last_expired_ingredient_check < current_time)
    {
        linked_list_node_ptr current_ingredient = order->recipe->ingredients->h;
        while (current_ingredient != NULL)
        {
            ingredient_ptr current_ingredient_data = (ingredient_ptr)current_ingredient->d;
            delete_all_expired_batches(current_ingredient_data->warehouse_item_info);
            current_ingredient = current_ingredient->n;
        }
        order->recipe->last_expired_ingredient_check = current_time;
    }

    {
        linked_list_node_ptr current_ingredient = order->last_missing_ingredient_node;
        while (current_ingredient != NULL)
        {
            const int recipe_ingredient_quantity = ((ingredient_ptr)current_ingredient->d)->quantity;
            const int order_ingredient_required_quantity = order->quantity * recipe_ingredient_quantity;
            const int warehouse_ingredient_quantity = ((ingredient_ptr)current_ingredient->d)->warehouse_item_info->quantity_total_in_stock;
            if (order_ingredient_required_quantity > warehouse_ingredient_quantity)
            {
                order->last_missing_ingredient_node = current_ingredient;
                return 0;
            }
            current_ingredient = current_ingredient->n;
        }
    }

    {
        linked_list_node_ptr current_ingredient = order->recipe->ingredients->h;
        while (current_ingredient != NULL && current_ingredient != order->last_missing_ingredient_node)
        {
            const int recipe_ingredient_quantity = ((ingredient_ptr)current_ingredient->d)->quantity;
            const int order_ingredient_required_quantity = order->quantity * recipe_ingredient_quantity;
            const int warehouse_ingredient_quantity = ((ingredient_ptr)current_ingredient->d)->warehouse_item_info->quantity_total_in_stock;
            if (order_ingredient_required_quantity > warehouse_ingredient_quantity)
            {
                order->last_missing_ingredient_node = current_ingredient;
                return 0;
            }
            current_ingredient = current_ingredient->n;
        }
    }
    return 1;
}

void use_ingredient(ingredient_ptr ingredient, int quantity)
{
    linked_list_node_ptr current_batch = ingredient->warehouse_item_info->batches->h;
    while (current_batch != NULL && quantity > 0)
    {
        item_batch_ptr current_batch_data = (item_batch_ptr)current_batch->d;
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
        current_batch = ingredient->warehouse_item_info->batches->h;
    }
}

void update_warehouse_on_order_processed(order_ptr order)
{
    linked_list_node_ptr current_ingredient = order->recipe->ingredients->h;
    while (current_ingredient != NULL)
    {
        const int recipe_ingredient_quantity = ((ingredient_ptr)current_ingredient->d)->quantity;
        const int order_ingredient_required_quantity = order->quantity * recipe_ingredient_quantity;
        use_ingredient(current_ingredient->d, order_ingredient_required_quantity);
        current_ingredient = current_ingredient->n;
    }
}

void add_to_ready_orders(linked_list_ptr ready_orders, order_ptr order)
{

    int insert_position = 0;
    linked_list_node_ptr current_order = ready_orders->h;
    while (current_order != NULL)
    {
        order_ptr current_order_data = (order_ptr)current_order->d;
        if (current_order_data->time > order->time)
        {
            break;
        }
        insert_position++;
        current_order = current_order->n;
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
    if (is_order_processable_now(new_order))
    {
        update_warehouse_on_order_processed(new_order);

        add_to_ready_orders(ready_orders, new_order);

        
    }
    else
    {
        linked_list_push_back(pending_orders, new_order);
    }
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
    linked_list_node_ptr current_batch = item->batches->h;
    while (current_batch != NULL)
    {
        item_batch_ptr current_batch_data = (item_batch_ptr)current_batch->d;
        if (current_batch_data->expiration_date > new_batch->expiration_date)
        {
            break;
        }
        insert_position++;
        current_batch = current_batch->n;
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
    linked_list_node_ptr current_order = pending_orders->h;
    while (current_order != NULL)
    {
        order_ptr current_order_data = (order_ptr)current_order->d;
        if (is_order_processable_now(current_order_data))
        {
            update_warehouse_on_order_processed(current_order_data);
            add_to_ready_orders(ready_orders, current_order_data);

            linked_list_node_ptr next_order = current_order->n;
            linked_list_remove_node(pending_orders, current_order, do_not_free);
            current_order = next_order;
        }
        else
        {
            current_order = current_order->n;
        }
    }
   
}

void handle_warehouse_refill()
{

    while (!ISNEWLINE(getchar()))
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

    last_refill_time = current_time;
    process_pending_orders();
}

void add_new_order_to_truck_orders(linked_list_ptr truck_orders, order_ptr order)
{
    int insert_position = 0;
    linked_list_node_ptr current_order = truck_orders->h;
    while (current_order != NULL)
    {
        order_ptr current_order_data = (order_ptr)current_order->d;
        if (current_order_data->total_weight < order->total_weight)
        {
            break;
        }
        insert_position++;
        current_order = current_order->n;
    }
    linked_list_push_at_position(truck_orders, order, insert_position);
}

void load_truck()
{
    linked_list_ptr truck_orders = linked_list_initialize();

    int loaded_weight = 0;
    while (loaded_weight < truck_capacity && ready_orders->h != NULL)
    {
        order_ptr current_order = (order_ptr)ready_orders->h->d;
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
        linked_list_node_ptr current_order = truck_orders->h;
        while (current_order != NULL)
        {
            order_ptr current_order_data = (order_ptr)current_order->d;
            printf("%d %s %d\n", current_order_data->time, current_order_data->recipe->name, current_order_data->quantity);
            current_order = current_order->n;
        }
    }
    if (truck_orders->l <= 0)
    {
        printf("camioncino vuoto\n");
    }
    linked_list_destroy(truck_orders, free_order);

}

int main()
{
    hashtable_ricette = calloc(HASHTABLE_SIZE, sizeof(linked_list_ptr));
    hashtable_warehouse = calloc(HASHTABLE_SIZE, sizeof(linked_list_ptr));
    ready_orders = linked_list_initialize();
    pending_orders = linked_list_initialize();
    int truck_loading_interval = 0;
    char raw_command[MAXIMUM_COMMAND_LENGHT];

    if (scanf("%d %d", &truck_loading_interval, &truck_capacity) != 2)
    {
        panic("Invalid input");
    }


    while (scanf("%s", raw_command) == 1)
    {
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
        if ((current_time % truck_loading_interval)==0)
        {
            load_truck();
        }
    }
    return 0;
}