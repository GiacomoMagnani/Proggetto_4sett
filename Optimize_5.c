#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define DIM_HASH_TABLE 100

int debug_var4 = 0;

#define SHOULD_PRINT_DEBUG 0
#define DEBUG(command)      \
    if (SHOULD_PRINT_DEBUG) \
    command

#define SHOULD_PRINT_DEBUG2 0
#define DEBUG2(command)      \
    if (SHOULD_PRINT_DEBUG2) \
    command

#define SHOULD_PRINT_DEBUG3 0
#define DEBUG3(command)      \
    if (SHOULD_PRINT_DEBUG3) \
    command

#define SHOULD_PRINT_DEBUG4 0
#define DEBUG4(command)      \
    if (SHOULD_PRINT_DEBUG4) \
    command

#define SHOULD_PRINT_DEBUG5 0
#define DEBUG5(command)      \
    if (SHOULD_PRINT_DEBUG5) \
    command

int time = 0;

char **str_split(char *a_str, const char a_delim, int *count)
{
    char **result = 0;
    char *tmp = a_str;
    char *last_comma = 0;
    char delim[3];
    delim[0] = a_delim;
    delim[2] = '\n';
    delim[1] = 0;
    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            (*count)++;
            last_comma = tmp;
        }
        tmp++;
    }
    /* Add space for trailing token. */
    *count += last_comma < (a_str + strlen(a_str) - 1);
    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    (*count)++;
    DEBUG(printf("[debug] S.1 %d\n", *count);)
    result = malloc(sizeof(char *) * (*count));
    DEBUG(printf("[debug] S.2\n"));
    if (result)
    {
        DEBUG(printf("[debug] S.3\n"));
        size_t idx = 0;
        char *token = strtok(a_str, delim);
        while (token)
        {
            assert(idx < (*count));
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == (*count) - 1);
        *(result + idx) = 0;
    }
    DEBUG(printf("[debug] S.4\n"));
    // printf("[debug] count %d\n", (*count));
    return result;
}
size_t hash_string(const char *str)
{
    size_t hash = 5381;
    size_t c;
    while ((c = *str++))
    {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash % DIM_HASH_TABLE;
}

typedef struct ingrediente
{
    char name[30];
    int quantita;
    struct ingrediente *next;
} ingrediente;
typedef ingrediente *puntatore_ingrediente;

typedef struct ricetta
{
    char name[30];
    int weight;
    int num_ordini_sospeso;
    puntatore_ingrediente ingredienti; // lista ingredienti ricetta
    struct ricetta *next;              // puntatore al prossima ricetta
} ricetta;
typedef ricetta *puntatore_ricetta;
typedef puntatore_ricetta *hashtable_ricette;

typedef struct lotto
{
    int quantita;
    int scadenza;
    struct lotto *next;
} lotto;
typedef lotto *puntatore_lotto;

typedef struct magazzino
{
    char ingrediente[30];
    int quantita_tot;
    puntatore_lotto lotti;
    struct magazzino *next;
} magazzino;
typedef magazzino *puntatore_magazzino;
typedef puntatore_magazzino *hashtable_magazzino;

void put_hashtable_magazzino (hashtable_magazzino ht_mgz){
    for(int i=0; i<DIM_HASH_TABLE; i++){
        puntatore_magazzino curr_mgz = ht_mgz[i];
        while(curr_mgz != NULL){
            printf("[debug] ingrediente: %s quantita' tot: %d\n", ht_mgz[i]->ingrediente, ht_mgz[i]->quantita_tot);
            puntatore_lotto curr_lotto = curr_mgz->lotti;
            while(curr_lotto != NULL){
                printf("[debug] \tquantita': %d scadenza: %d\n", curr_lotto->quantita, curr_lotto->scadenza);
                curr_lotto = curr_lotto->next;
            }
            printf("\n");
            curr_mgz = curr_mgz->next;
        }
    }
}

typedef struct ordine
{
    char *ricetta;
    int quantita;
    int is_done;
    int weight;
    int time;
    struct ordine *next;
} ordine;
typedef ordine *puntatore_ordine;

typedef struct coda_ordini
{
    puntatore_ordine head;
    puntatore_ordine tail;
} coda_ordini;
typedef coda_ordini *puntatore_coda_ordini;

puntatore_coda_ordini ordini_in_sospeso;
puntatore_coda_ordini ordini_completati;

void stampa_ordini(puntatore_ordine head_ord, char * prefix)
{
    puntatore_ordine curr_ord = head_ord;
    if(curr_ord == NULL){
        fprintf(stdout, "[debug] %s: null\n", prefix);
    }
    while (curr_ord != NULL)
    {
        fprintf(stdout, "[debug] %s: %d %s quantita %d peso %d\n", prefix, curr_ord->time, curr_ord->ricetta, curr_ord->quantita, curr_ord->weight);
        curr_ord = curr_ord->next;
    }
}


void add_ordine_in_coda(puntatore_coda_ordini ordini, puntatore_ordine new_ord)
{
    if (ordini->head == NULL)
    {
        ordini->head = new_ord;
        ordini->tail = ordini->head;
        ordini->tail->next = NULL;
        DEBUG4(printf("[debug] [add_ord_coda] add_from_null %d %s %d\n", ordini->tail->time, ordini->tail->ricetta, ordini->tail->quantita);)
    }
    else {
        ordini->tail->next = new_ord;
        DEBUG4(printf("[debug] [add_ord_coda] previous_tail %d %s %d\n", ordini->tail->time, ordini->tail->ricetta, ordini->tail->quantita);)
        ordini->tail = ordini->tail->next;
        ordini->tail->next = NULL;
    }
}
hashtable_ricette crea_hashtable_ricette()
{
    hashtable_ricette ht = (hashtable_ricette)calloc(DIM_HASH_TABLE, sizeof(puntatore_ricetta));
    return ht;
}
puntatore_ingrediente crea_ingr(char *ing, int qnt)
{
    puntatore_ingrediente new_ing = (puntatore_ingrediente)malloc(sizeof(struct ingrediente));
    strcpy(new_ing->name, ing);
    new_ing->quantita = qnt;
    new_ing->next = NULL;
    return new_ing;
}
puntatore_ingrediente aggiungi_ingrediente_in_lista(puntatore_ingrediente head, char *ing, int qnt)
{
    puntatore_ingrediente new_head = head;
    if (head == NULL)
    {
        head = crea_ingr(ing, qnt);
        new_head = head;
    }
    else
    {
        puntatore_ingrediente curr_ing = head;
        while (curr_ing->next != NULL)
        {
            curr_ing = curr_ing->next;
        }
        puntatore_ingrediente new_ing = crea_ingr(ing, qnt);
        curr_ing->next = new_ing;
    }
    return new_head;
}
puntatore_ricetta crea_ricetta(char *name, int weight, int ord, puntatore_ingrediente ing)
{
    puntatore_ricetta new_ric = (puntatore_ricetta)malloc(sizeof(ricetta));
    strcpy(new_ric->name, name);
    new_ric->weight = weight;
    new_ric->num_ordini_sospeso = ord;
    new_ric->ingredienti = ing;
    new_ric->next = NULL;
    return new_ric;
}
puntatore_ricetta seek_lista_ricette(puntatore_ricetta head, const char *name)
{
    puntatore_ricetta curr = head;
    while (curr != NULL)
    {
        if (strcmp(curr->name, name) == 0)
        {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}
puntatore_ricetta seek_ricetta_hashtable(hashtable_ricette ht, const char *nome_ric)
{
    size_t hash_indice = hash_string(nome_ric);
    DEBUG(printf("[debug] SRH.10 %ld %s\n", hash_indice, nome_ric);)
    puntatore_ricetta pointer_ric = seek_lista_ricette(ht[hash_indice], nome_ric);
    return pointer_ric;
}
puntatore_ricetta aggiungi_ricetta(puntatore_ricetta curr, puntatore_ricetta new_ric)
{
    puntatore_ricetta new_head = curr;
    if (curr == NULL)
    {
        curr = new_ric;
        new_head = new_ric;
    }
    else
    {
        while (curr->next != NULL)
        {
            curr = curr->next;
        }
        curr->next = new_ric;
    }
    return new_head;
}
void add_ricetta_hashtable(hashtable_ricette ht, puntatore_ricetta new_ric)
{
    size_t hash_indice = hash_string(new_ric->name);
    puntatore_ricetta curr_ric = ht[hash_indice];
    ht[hash_indice] = aggiungi_ricetta(curr_ric, new_ric);
}
void free_ing(puntatore_ricetta curr_ric)
{
    puntatore_ingrediente curr_ing = curr_ric->ingredienti;
    puntatore_ingrediente prec_ing = NULL;
    while (curr_ing != NULL)
    {
        prec_ing = curr_ing;
        curr_ing = curr_ing->next;
        prec_ing->next = NULL;
        free(prec_ing);
    }
}
hashtable_magazzino crea_hashtable_magazzino()
{
    hashtable_magazzino ht = (hashtable_magazzino)calloc(DIM_HASH_TABLE, sizeof(puntatore_magazzino));
    return ht;
}
puntatore_magazzino crea_magazzino(char *ing, puntatore_lotto new_lotto)
{
    puntatore_magazzino new_magazzino = (puntatore_magazzino)malloc(sizeof(magazzino));
    strcpy(new_magazzino->ingrediente, ing);
    new_magazzino->quantita_tot = new_lotto->quantita;
    new_magazzino->lotti = new_lotto;
    new_magazzino->next = NULL;
    return new_magazzino;
}
puntatore_magazzino aggiungi_magazzino(puntatore_magazzino head_mgz, puntatore_magazzino new_mgz)
{
    puntatore_magazzino curr = head_mgz;
    DEBUG5(printf("AM.1\n");)
    if (curr == NULL)
    {
        DEBUG5(printf("AM.2\n");)
        curr = new_mgz;
        return curr;
    }
    else
    {
        DEBUG5(printf("AM.3\n");)
        while (curr->next != NULL)
        {
            curr = curr->next;
        }
        DEBUG5(printf("AM.4 %s %d - %d %d\n", new_mgz->ingrediente, new_mgz->quantita_tot, new_mgz->lotti->quantita, new_mgz->lotti->scadenza);)
        curr->next = new_mgz;
        DEBUG5(printf("AM.5 %d %d\n", curr->next->lotti->quantita, curr->next->lotti->scadenza);)
    }
    DEBUG5(printf("AM.6\n");)
    return head_mgz;
}
void add_magazzino_hashtable(hashtable_magazzino ht, puntatore_magazzino new_mgz)
{
    DEBUG5(printf("AMH.1\n");)
    size_t hash_indice = hash_string(new_mgz->ingrediente);
    DEBUG5(printf("AMH.2\n");)
    puntatore_magazzino curr_mgz = ht[hash_indice];
    DEBUG5(printf("AMH.3\n");)
    ht[hash_indice] = aggiungi_magazzino(curr_mgz, new_mgz);
    DEBUG5(printf("AMH.4\n");)
}
puntatore_lotto crea_lotto(int qnt, int scd)
{
    puntatore_lotto new_lotto = (puntatore_lotto)malloc(sizeof(lotto));
    new_lotto->quantita = qnt;
    new_lotto->scadenza = scd;
    new_lotto->next = NULL;
    return new_lotto;
}
puntatore_magazzino seek_magazzino(hashtable_magazzino ht_mgz, char *ing)
{
    size_t hash_indice = hash_string(ing);
    puntatore_magazzino curr_mgz = ht_mgz[hash_indice];
    while (curr_mgz != NULL && strcmp(curr_mgz->ingrediente, ing) != 0)
    {
        curr_mgz = curr_mgz->next;
    }
    return curr_mgz;
}
void aggiungi_lotto_a_mgz(hashtable_magazzino ht_mgz, char *ing, puntatore_lotto new_lotto)
{
    if(strcmp(ing, "dk8BmNQd2ktntRxdmrWZH3E") == 0){
        printf("[debug] [aggiungi_lotto_a_mgz] prima time: %d, lotto qnt: %d scd: %d\n", time, new_lotto->quantita, new_lotto->scadenza);
        put_hashtable_magazzino(ht_mgz);
    }
    DEBUG5(printf("[debug] ALAM.1\n");)
    size_t hash_indice = hash_string(ing);
    puntatore_magazzino curr_mgz = ht_mgz[hash_indice];
    DEBUG(printf("[debug] ALAM.2\n");)
    if (curr_mgz == NULL)
    {
        DEBUG5(printf("[debug] ALAM.3\n");)
        puntatore_magazzino new_mgz = crea_magazzino(ing, new_lotto);
        add_magazzino_hashtable(ht_mgz, new_mgz);
        DEBUG4(printf("[debug] rifornimento: %s %d\n ", new_mgz->ingrediente, new_mgz->quantita_tot);)
    }
    else
    {
        DEBUG5(printf("[debug] ALAM.4\n");)
        while (curr_mgz->next != NULL && strcmp(curr_mgz->ingrediente, ing) != 0)
        {
            curr_mgz = curr_mgz->next;
        }
        DEBUG5(printf("[debug] ALAM.5\n");)
        if (strcmp(curr_mgz->ingrediente, ing) == 0)
        {
            DEBUG5(printf("[debug] ALAM.6\n");)
            puntatore_lotto curr_lotto = curr_mgz->lotti;
            if (new_lotto->scadenza < curr_lotto->scadenza)
            {
                DEBUG5(printf("[debug] ALAM.7\n");)
                curr_mgz->lotti = new_lotto;
                curr_mgz->lotti->next = curr_lotto;
                curr_mgz->quantita_tot = curr_mgz->quantita_tot + new_lotto->quantita;
                DEBUG4(printf("[debug] rifornimento: %s %d\n ", curr_mgz->ingrediente, curr_mgz->quantita_tot);)
                return;
            }
            DEBUG5(printf("[debug] ALAM.8\n");)
            while (curr_lotto->next != NULL && curr_lotto->next->scadenza < new_lotto->scadenza)
            {
                curr_lotto = curr_lotto->next;
            }
            DEBUG5(printf("[debug] ALAM.9\n");)
            if (curr_lotto->next == NULL)
            {
                DEBUG5(printf("[debug] ALAM.10\n");)
                curr_lotto->next = new_lotto;
                curr_mgz->quantita_tot = curr_mgz->quantita_tot + new_lotto->quantita;
            }
            else
            {
                DEBUG5(printf("[debug] ALAM.11\n");)
                new_lotto->next = curr_lotto->next;
                curr_lotto->next = new_lotto;
                curr_mgz->quantita_tot = curr_mgz->quantita_tot + new_lotto->quantita;
            }
            DEBUG4(printf("[debug] rifornimento: %s %d\n ", curr_mgz->ingrediente, curr_mgz->quantita_tot);)
        }
        else
        {
            DEBUG5(printf("[debug] ALAM.12\n");)
            puntatore_magazzino new_mgz = crea_magazzino(ing, new_lotto);
            DEBUG5(printf("[debug] ALAM.13\n");)
            add_magazzino_hashtable(ht_mgz, new_mgz);
            DEBUG5(printf("[debug] rifornimento: %s %d\n ", new_mgz->ingrediente, new_mgz->quantita_tot);)
            DEBUG(printf("[debug] ALAM.14\n");)
        }
    }
    if(strcmp(ing, "dk8BmNQd2ktntRxdmrWZH3E") == 0){
        printf("[debug] [aggiungi_lotto_a_mgz] dopo time: %d, lotto qnt: %d scd: %d\n", time, new_lotto->quantita, new_lotto->scadenza);
        put_hashtable_magazzino(ht_mgz);
    }
}

void rimuovi_lotti_scaduti_per_ingrediente(puntatore_magazzino ingrediente)
{
    puntatore_lotto primo_lotto_magazzino = ingrediente->lotti;

    puntatore_lotto primo_lotto_non_scaduto = primo_lotto_magazzino;
    while(primo_lotto_non_scaduto && primo_lotto_non_scaduto->scadenza <= time)
    {
        primo_lotto_non_scaduto = primo_lotto_non_scaduto->next;
    }
    puntatore_lotto curr = primo_lotto_non_scaduto;
    while(curr != NULL){
        if(curr->scadenza <= time){
            printf("[debug] [rimiovi_lotti_scaduti] lotto scaduto");
        }
        curr = curr->next;
    }

    puntatore_lotto to_delete = primo_lotto_magazzino;
    while(to_delete != primo_lotto_non_scaduto){
        puntatore_lotto temp = to_delete;
        ingrediente->quantita_tot = ingrediente->quantita_tot - temp->quantita;
        to_delete = to_delete->next;
        free(temp);
    }
    ingrediente->lotti = primo_lotto_non_scaduto;
}

puntatore_magazzino remove_magazzino(hashtable_magazzino ht_mgz, puntatore_magazzino to_remove)
{
    size_t hash_indice = hash_string(to_remove->ingrediente);
    puntatore_magazzino head_mgz = ht_mgz[hash_indice];
    puntatore_magazzino curr = head_mgz;
    if (head_mgz == to_remove)
    {
        curr = head_mgz->next;
        head_mgz->next = NULL;
        free(head_mgz);
        return curr;
    }
    puntatore_magazzino prec = NULL;
    while (curr != to_remove)
    {
        prec = curr;
        curr = curr->next;
    }
    prec->next = curr->next;
    curr->next = NULL;
    free(curr);
    return head_mgz;
}

int is_ordine_fattibile(puntatore_ordine ord, hashtable_ricette ht_ricette, hashtable_magazzino ht_mgz)
{
    puntatore_ricetta ric_ord = seek_ricetta_hashtable(ht_ricette, ord->ricetta);
    puntatore_ingrediente curr_ing = ric_ord->ingredienti;
    if(ord->time == 17){
            put_hashtable_magazzino(ht_mgz);
    }
    DEBUG(printf("[debug] [is_ordine_fattibile] 0\n"));
    while (curr_ing != NULL)
    {
        DEBUG(printf("[debug] [is_ordine_fattibile] 1\n"));
        puntatore_magazzino curr_mgz = seek_magazzino(ht_mgz, curr_ing->name);
        if(!curr_mgz){
            DEBUG(printf("[debug] [is_ordine_fattibile] 1.1\n"));
            return 0;
        }
        DEBUG(printf("[debug] [is_ordine_fattibile] 2\n"));
        rimuovi_lotti_scaduti_per_ingrediente(curr_mgz);
        DEBUG(printf("[debug] [is_ordine_fattibile] 3\n"));
        if (!curr_mgz->lotti)
        {
            DEBUG(printf("[debug] [is_ordine_fattibile] 3.1\n"));
            size_t hash_indice = hash_string(curr_mgz->ingrediente);
            ht_mgz[hash_indice] = remove_magazzino(ht_mgz, curr_mgz);
            DEBUG(printf("[debug] [is_ordine_fattibile] 3.2\n"));
            return 0;
        }
        DEBUG(printf("[debug] [is_ordine_fattibile] check-ingrediente %s\n", curr_ing->name));
        if (curr_mgz == NULL || curr_ing->quantita * ord->quantita > curr_mgz->quantita_tot)
        {
            return 0;
        }
        curr_ing = curr_ing->next;
    }
    return 1;
}
puntatore_ordine free_carico(puntatore_ordine carico)
{
    puntatore_ordine curr = carico;
    puntatore_ordine prec = NULL;
    while (curr != NULL)
    {
        prec = curr;
        curr = curr->next;
        prec->next = NULL;
        free(prec);
    }
    return NULL;
}
puntatore_ordine crea_ordine(char *nome_ricetta, int quantita, hashtable_ricette ht_ricette)
{
    puntatore_ordine new_ordine = (puntatore_ordine)malloc(sizeof(ordine));
    puntatore_ricetta ricetta = seek_ricetta_hashtable(ht_ricette, nome_ricetta);
    new_ordine->ricetta = ricetta->name;
    new_ordine->quantita = quantita;
    new_ordine->is_done = 0;
    //printf("[debug] [crea_ordine] %d %d\n", ricetta->weight, new_ordine->quantita);
    new_ordine->weight = quantita * ricetta->weight;
    new_ordine->time = time;
    new_ordine->next = NULL;
    return new_ordine;
}
void cancella_lista_ingredienti_da_ricetta(puntatore_ricetta ricetta)
{
    puntatore_ingrediente curr_ing = ricetta->ingredienti;
    puntatore_ingrediente prec_ing = NULL;
    while (curr_ing != NULL)
    {
        prec_ing = curr_ing;
        curr_ing = curr_ing->next;
        prec_ing->next = NULL;
        free(prec_ing);
    }
}
void cancella_ricetta_hashtable(hashtable_ricette ht, puntatore_ricetta ricetta_da_eliminare)
{
    cancella_lista_ingredienti_da_ricetta(ricetta_da_eliminare);
    size_t hash_indice = hash_string(ricetta_da_eliminare->name);
    if (ht[hash_indice] == ricetta_da_eliminare)
    {
        ht[hash_indice] = ht[hash_indice]->next;
        free(ricetta_da_eliminare);
        return;
    }
    puntatore_ricetta curr_ric = ht[hash_indice];
    puntatore_ricetta next_ric = ht[hash_indice]->next;
    while (next_ric != ricetta_da_eliminare)
    {
        curr_ric = next_ric;
        next_ric = next_ric->next;
    }
    curr_ric->next = next_ric->next;
    free(ricetta_da_eliminare);
}

puntatore_ordine add_ordine_peso(puntatore_ordine carico, puntatore_ordine ord)
{
    //printf("[debug] provo ad inserire ordine con peso %d\n", ord->weight);
   if (carico == NULL)
    {
        carico = ord;
        return carico;
    }
    if(carico->weight < ord->weight){
        ord->next = carico;
        return ord;
    }
    puntatore_ordine curr_ord = carico->next;
    puntatore_ordine prec = carico;
    DEBUG2(printf("[debug] AOP.3\n");)
    while (curr_ord != NULL && curr_ord->weight >= ord->weight)
    {
        prec = curr_ord;
        curr_ord = curr_ord->next;
    }
    if (curr_ord == NULL)
    {
        //printf("[debug] AOP.4.1\n");
        prec->next = ord;
        ord->next = NULL;
    }
    else
    {
        prec->next = ord;
        ord->next = curr_ord;
    }

    return carico;
}

void set_new_ordine_per_ricetta(puntatore_ordine ord, int aggiungi, hashtable_ricette ht_ricette)
{
    puntatore_ricetta ric = seek_ricetta_hashtable(ht_ricette, ord->ricetta);
    ric->num_ordini_sospeso = ric->num_ordini_sospeso + aggiungi;
}
puntatore_ordine seleziona_ordini_camioncino(int capienza, hashtable_ricette ht_ricette)
{
    puntatore_ordine completo = ordini_completati->head;
    while(completo){
        //printf("[debug] [seleziona_ordini_camioncino] ordine: %d %s %d\n", completo->time, completo->ricetta, completo->quantita);
        completo = completo->next;
    }
    puntatore_ordine carico = NULL;
    puntatore_ordine head_ordini_pronti = ordini_completati->head;
    while (head_ordini_pronti && capienza >= head_ordini_pronti->weight)
    {
        capienza-=head_ordini_pronti->weight;
        //printf("[debug] [seleziona_ordini_camioncino] ordine: %d %s %d\n", head_ordini_pronti->time, head_ordini_pronti->ricetta, head_ordini_pronti->quantita);
        puntatore_ordine next_pronto = head_ordini_pronti->next;
        head_ordini_pronti->next = NULL;
        carico = add_ordine_peso(carico, head_ordini_pronti);
           // printf("[debug] carico temporaneo\n");
    //stampa_ordini(carico, "carico temporaneo");
    //printf("[debug] \n\n");
        set_new_ordine_per_ricetta(head_ordini_pronti, -1, ht_ricette);

        head_ordini_pronti = next_pronto;
        ordini_completati->head = head_ordini_pronti;

    }

    if(!ordini_completati->head){
        ordini_completati->tail = NULL;
    }
    //stampa_ordini(ordini_in_sospeso->head, "sospeso ");
    return carico;
}
void put_ordine(puntatore_ordine ord)
{
    puntatore_ordine curr = ord;
    while (curr != NULL)
    {
        fprintf(stdout, "%d %s %d\n", curr->time, curr->ricetta, curr->quantita);
        curr = curr->next;
    }
}
puntatore_lotto remove_lotto(puntatore_lotto head_lotto, puntatore_lotto to_remove)
{
    puntatore_lotto curr = head_lotto;
    if (head_lotto == to_remove)
    {
        curr = head_lotto->next;
        head_lotto->next = NULL;
        free(head_lotto);
        return curr;
    }
    puntatore_lotto prec = NULL;
    while (curr != to_remove)
    {
        prec = curr;
        curr = curr->next;
    }
    prec->next = curr->next;
    curr->next = NULL;
    free(curr);
    return head_lotto;
}
puntatore_lotto remove_lotti_a_zero(puntatore_lotto head_lotto, int residuo)
{
    puntatore_lotto curr_lotto = head_lotto;
    while (residuo > 0)
    {
        if(curr_lotto->scadenza <= time){
            printf("[debug] [remove_lotti_a_zero] lotto scaduto");
            exit(-1);
        }
        if (residuo < curr_lotto->quantita)
        {
            curr_lotto->quantita = curr_lotto->quantita - residuo;
            residuo = 0;
        }
        else
        {
            residuo = residuo - curr_lotto->quantita;
            puntatore_lotto temp = curr_lotto;
            curr_lotto = curr_lotto->next;
            head_lotto = remove_lotto(head_lotto, temp);
        }
    }
    return head_lotto;
}

void make_ordine(puntatore_ordine ord, hashtable_ricette ht_ricette, hashtable_magazzino ht_mgz)
{
    //printf("[debug] [make_ordine] %s %d %d\n", ord->ricetta, ord->quantita, ord->time);
    puntatore_ricetta ric_ord = seek_ricetta_hashtable(ht_ricette, ord->ricetta);
    puntatore_ingrediente curr_ing = ric_ord->ingredienti;
    int residuo;
    while (curr_ing != NULL)
    {
        puntatore_magazzino curr_mgz = seek_magazzino(ht_mgz, curr_ing->name);
        residuo = curr_ing->quantita * ord->quantita;
        curr_mgz->quantita_tot = curr_mgz->quantita_tot - residuo;
        curr_mgz->lotti=remove_lotti_a_zero(curr_mgz->lotti, residuo);
        curr_ing = curr_ing->next;
    }
}
void check_ordiniSospesi(hashtable_ricette ht_ricette, hashtable_magazzino ht_mgz)
{
    puntatore_ordine curr_ord = ordini_in_sospeso->head;
    puntatore_ordine prec_ord = NULL;
    puntatore_ordine temp = NULL;
    while (curr_ord != NULL)
    {
        DEBUG(printf("[debug] [check_ordiniSospesi] curr_ord %d %s\n", curr_ord->time, curr_ord->ricetta);)
        if (is_ordine_fattibile(curr_ord, ht_ricette, ht_mgz) == 1)
        {
            DEBUG(printf("[debug] [check_ordiniSospesi] 1\n");)
            make_ordine(curr_ord, ht_ricette, ht_mgz);
            DEBUG(printf("[debug] [check_ordiniSospesi] 2\n");)
            if (curr_ord == ordini_in_sospeso->head)
            {
                DEBUG(printf("[debug] [check_ordiniSospesi] 3.1\n");)
                temp = curr_ord;
                curr_ord = temp->next;
                ordini_in_sospeso->head = temp->next;
                temp->next = NULL;
            }
            else
            {
                DEBUG(printf("[debug] [check_ordiniSospesi] 3.2\n");)
                temp = curr_ord;
                curr_ord = curr_ord->next;
                prec_ord->next = curr_ord;
                temp->next = NULL;
            }
            DEBUG(printf("[debug] [check_ordiniSospesi] 4\n");)
            add_ordine_in_coda(ordini_completati, temp);
        }
        else
        {
            DEBUG(printf("[debug] [check_ordiniSospesi] 1.2\n");)
            prec_ord = curr_ord;
            curr_ord = curr_ord->next;
        }
        DEBUG(stampa_ordini(ordini_in_sospeso->head, "r sospeso ");)
        DEBUG(stampa_ordini(ordini_completati->head, "r completati ");)
    }
}



int main()
{
    ordini_in_sospeso = (puntatore_coda_ordini)calloc(1, sizeof(coda_ordini));
    ordini_completati = (puntatore_coda_ordini)calloc(1, sizeof(coda_ordini));

    // DA DOVE PRENDERE L'INPUT
    FILE *in1 = stdin;
    //FILE* in1 = fopen("C:\\Users\\Giacomo\\Desktop\\Prog_API\\Open_rand.txt", "r");
    //FILE *in1 = fopen("C:\\Users\\Giacomo\\Desktop\\Prog_API\\comandi.txt", "r");
    //FILE *in1 = fopen("C:\\Users\\Giacomo\\Downloads\\open4.txt", "r");

    // GESTIONE DEL COMANDO
    int r1;
    char r2;

    int time_corriere, capienza;
    char comando[1000];
    int count = 0;
    char **tokens;
    hashtable_ricette ht_ricette = crea_hashtable_ricette();
    hashtable_magazzino ht_magazzino = crea_hashtable_magazzino();

    // EXEC
    DEBUG(printf("[debug] start\n"));
    r1 = fscanf(in1, "%d %d\n", &time_corriere, &capienza);
    r1--;
    DEBUG2(printf("[debug] %d %d\n", time_corriere, capienza));
    do
    {
        /*
        if(time < 25){
            debug_var4 = 1;
        }
        else{
            debug_var4 = 0;
        }
        //*/
        if(time > 100){
            exit(-2);
        }
        if (time != 0 && time % time_corriere == 0)
        {
            if (ordini_completati->head == NULL)
            {
                fprintf(stdout, "camioncino vuoto\n");
            }
            else
            {
                puntatore_ordine carico = seleziona_ordini_camioncino(capienza, ht_ricette);
                put_ordine(carico);
                free_carico(carico);
            }
        }
        r2 = (long int)fgets(comando, sizeof(comando), in1);
        if (!r2)
        {
            break;
        }
        comando[strlen(comando)] = '\0';
        for (int i = 0; i < strlen(comando); i++)
        {
            if (comando[i] == '\n')
            {
                comando[i] = '\0';
            }
        }
        DEBUG2(printf("[debug] %d-%s\n",time, comando));
        count = 0;
        tokens = str_split(comando, ' ', &count);
        DEBUG3(printf("[debug] A.0\n");)
        if (comando[0] == 'a' && comando[1] == 'g' && comando[2] == 'g')
        {
            char* ric;
            ric = *(tokens + 1);
            puntatore_ricetta ptr_ric = seek_ricetta_hashtable(ht_ricette, ric);
            if (ptr_ric != NULL)
            {
                fprintf(stdout, "ignorato\n");
            }
            else
            {
                puntatore_ingrediente list_ingredienti = NULL;
                int i = 0;
                int weight = 0;
                while (*(tokens + 2 + (2 * i)) != NULL)
                {
                    char *ing;
                    char* sub;
                    int qnt;
                    ing = *(tokens + 2 + (2 * i));
                    sub = *(tokens + 3 + (2 * i));
                    qnt = atoi(sub);
                    list_ingredienti = aggiungi_ingrediente_in_lista(list_ingredienti, ing, qnt);
                    weight = weight + qnt;
                    i++;
                }
                puntatore_ricetta new_ric = crea_ricetta(ric, weight, 0, list_ingredienti);

                add_ricetta_hashtable(ht_ricette, new_ric);
                fprintf(stdout, "aggiunta\n");
            }
        }
        if (comando[0] == 'r' && comando[1] == 'i' && comando[2] == 'm')
        {
            char* ric;
            ric = *(tokens + 1);
            puntatore_ricetta ptr_ric = seek_ricetta_hashtable(ht_ricette, ric);
            if (ptr_ric == NULL)
            {
                fprintf(stdout, "non presente\n");
            }
            else if (ptr_ric->num_ordini_sospeso > 0)
            {
                fprintf(stdout, "ordini in sospeso\n");
            }
            else
            {
                cancella_ricetta_hashtable(ht_ricette, ptr_ric);
                fprintf(stdout, "rimossa\n");
            }
        }
        if (comando[0] == 'r' && comando[1] == 'i' && comando[2] == 'f')
        {
            char *ing;
            char *sub;
            int qnt;
            int scd;
            int i = 0;
            DEBUG3(printf("[debug] R.1\n");)
            while (*(tokens + 1 + (3 * i)) != NULL)
            {
                ing = *(tokens + 1 + (3 * i));
                DEBUG(printf("[debug] ing: '%s'\n", ing);)
                sub = *(tokens + 2 + (3 * i));
                DEBUG(printf("[debug] sub: '%s'\n", sub);)
                qnt = atoi(sub);
                DEBUG(printf("[debug] qnt: '%d'\n", qnt);)
                sub = *(tokens + 3 + (3 * i));
                DEBUG(printf("[debug] sub: '%s'\n", sub);)
                scd = atoi(sub);
                DEBUG(printf("[debug] scd: '%d'\n", scd);)
                puntatore_lotto new_lotto = crea_lotto(qnt, scd);
                aggiungi_lotto_a_mgz(ht_magazzino, ing, new_lotto);
                i++;
            }
            DEBUG3(printf("[debug] R.2\n");)
            fprintf(stdout, "rifornito\n");
            check_ordiniSospesi(ht_ricette, ht_magazzino);
            DEBUG(printf("[debug] fine  check_ordiniSospesi\n");)
            DEBUG(stampa_ordini(ordini_in_sospeso->head, "r sospeso ");)
            DEBUG(stampa_ordini(ordini_completati->head, "r completati ");)
        }
        if (comando[0] == 'o' && comando[1] == 'r' && comando[2] == 'd')
        {
            char* ric = *(tokens + 1);
            char* sub = *(tokens + 2);
            int qnt = atoi(sub);
            if (seek_ricetta_hashtable(ht_ricette, ric) == NULL)
            {
                fprintf(stdout, "rifiutato\n");
            }
            else
            {
                puntatore_ordine new_ord = crea_ordine(ric, qnt, ht_ricette);
                int ordine_fattibile = is_ordine_fattibile(new_ord, ht_ricette, ht_magazzino);

                DEBUG(printf("[debug] ordine_creato \n");)
                DEBUG(printf("[debug] is_fattibile: '%d'\n", ordine_fattibile);)
                if (ordine_fattibile == 1)
                {
                    printf("[debug] fattibile\n");
                    make_ordine(new_ord, ht_ricette, ht_magazzino);
                    DEBUG(stampa_ordini(ordini_completati->head, "o1 completati ");)
                    add_ordine_in_coda(ordini_completati, new_ord);
                    DEBUG(stampa_ordini(ordini_completati->head, "o2 completati ");)
                }
                else
                {
                    printf("[debug] non fattibile\n");
                    DEBUG(stampa_ordini(ordini_in_sospeso->head, "o1 sospeso ");)
                    add_ordine_in_coda(ordini_in_sospeso, new_ord);
                    DEBUG(stampa_ordini(ordini_in_sospeso->head, "o2 sospeso ");)
                }
                set_new_ordine_per_ricetta(new_ord, '+', ht_ricette);
                fprintf(stdout, "accettato\n");
                DEBUG4(stampa_ordini(ordini_in_sospeso->head, "o sospeso ");)
                DEBUG4(stampa_ordini(ordini_completati->head, "o completati ");)
            }
        }
        time++;
    } while (r2 != EOF);
    return 0;
}
