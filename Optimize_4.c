#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define DIM_HASH_TABLE 100

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
    result = malloc(sizeof(char *) * (*count));
    if (result)
    {
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
typedef ingrediente* puntatore_ingrediente;

typedef struct ricetta
{
    char *name;
    int weight;
    int num_ordini_sospeso;
    puntatore_ingrediente ingredienti; // lista ingredienti ricetta
    struct ricetta *next;            // puntatore al prossima ricetta
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
typedef puntatore_magazzino* hashtable_magazzino;

typedef struct ordine
{
    char* ricetta;
    int quantita;
    int is_done;
    int weight;
    int time;
    struct ordine *next;
} ordine;
typedef ordine *puntatore_ordine;

typedef struct coda_ordini{
    puntatore_ordine testa;
    puntatore_ordine coda;
} coda_ordini;
typedef coda_ordini * puntatore_coda_ordini;

puntatore_coda_ordini ordini_in_sospeso;
puntatore_coda_ordini ordini_completati;
void add_ordine_in_coda(puntatore_coda_ordini ordini ,puntatore_ordine nuovo_ordine ){
    if(!ordini->coda){
        ordini->testa = nuovo_ordine;
        ordini->coda = ordini->testa;
        return;
    }
    ordini->coda->next = nuovo_ordine;
    ordini->coda = ordini->coda->next;
}

hashtable_ricette crea_hashtable_ricette()
{
    hashtable_ricette ht = (hashtable_ricette)calloc(DIM_HASH_TABLE, sizeof(puntatore_ricetta));
    return ht;
}
puntatore_ingrediente crea_ingr(char *ing, int qnt)
{
    puntatore_ingrediente new_ing = (puntatore_ingrediente )malloc(sizeof(struct ingrediente));
    strcpy(new_ing->name, ing);
    new_ing->quantita = qnt;
    new_ing->next = NULL;
    return new_ing;
}
void aggiungi_ingrediente_in_lista(puntatore_ingrediente head, char *ing, int qnt)
{
    if (head == NULL)
    {
        head = crea_ingr(ing, qnt);
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
    puntatore_ricetta pointer_ric = seek_lista_ricette(ht[hash_indice], nome_ric);
    return pointer_ric;
}
void aggiungi_ricetta(puntatore_ricetta curr, puntatore_ricetta new_ric)
{
    if (curr == NULL)
    {
        curr = new_ric;
    }
    else
    {
        while (curr->next != NULL)
        {
            curr = curr->next;
        }
        curr->next = new_ric;
    }
}
void add_ricetta_hashtable(hashtable_ricette ht, puntatore_ricetta new_ric)
{
    size_t hash_indice = hash_string(new_ric->name) ;
    puntatore_ricetta curr_ric = ht[hash_indice];
    aggiungi_ricetta(curr_ric, new_ric);
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
void aggiungi_magazzino(puntatore_magazzino curr, puntatore_magazzino new_mgz)
{
    if (curr == NULL)
    {
        curr = new_mgz;
    }
    else
    {
        while (curr->next != NULL)
        {
            curr = curr->next;
        }
        curr->next = new_mgz;
    }
}
void add_magazzino_hashtable(hashtable_magazzino ht, puntatore_magazzino new_mgz)
{
    size_t hash_indice = hash_string(new_mgz->ingrediente) ;
    puntatore_magazzino curr_mgz = ht[hash_indice];
    aggiungi_magazzino(curr_mgz, new_mgz);
}
puntatore_lotto crea_lotto(int qnt, int scd)
{
    puntatore_lotto new_lotto = (puntatore_lotto)malloc(sizeof(lotto));
    new_lotto->quantita = qnt;
    new_lotto->scadenza = scd;
    return new_lotto;
}
puntatore_magazzino seek_magazzino(hashtable_magazzino ht_mgz, char *ing)
{
    size_t hash_indice = hash_string(ing) ;
    puntatore_magazzino curr_mgz = ht_mgz[hash_indice];
    while (curr_mgz != NULL && strcmp(curr_mgz->ingrediente, ing) != 0)
    {
        curr_mgz = curr_mgz->next;
    }
    return curr_mgz;
}
void aggiungi_lotto_a_mgz(hashtable_magazzino ht_mgz, char *ing, puntatore_lotto new_lotto)
{
    size_t hash_indice = hash_string(ing) ;
    puntatore_magazzino curr_mgz = ht_mgz[hash_indice];
    while (curr_mgz->next != NULL && strcmp(curr_mgz->ingrediente, ing) != 0)
    {
        curr_mgz = curr_mgz->next;
    }
    if (strcmp(curr_mgz->ingrediente, ing) == 0)
    {
        puntatore_lotto curr_lotto = curr_mgz->lotti;
        while (curr_lotto->next->scadenza < new_lotto->scadenza || curr_lotto->next != NULL)
        {
            curr_lotto = curr_lotto->next;
        }
        new_lotto->next = curr_lotto->next;
        curr_lotto->next = new_lotto;
        curr_mgz->quantita_tot = curr_mgz->quantita_tot + new_lotto->quantita;
    }
    if (curr_mgz->next == NULL)
    {
        puntatore_magazzino new_mgz = crea_magazzino(ing, new_lotto);
        aggiungi_magazzino(ht_mgz[hash_indice], new_mgz);
    }
}
int is_ordine_fattibile(puntatore_ordine ord, hashtable_ricette ht_ricette, hashtable_magazzino ht_mgz)
{
    puntatore_ricetta ric_ord = seek_ricetta_hashtable(ht_ricette, ord->ricetta);
    puntatore_ingrediente curr_ing = ric_ord->ingredienti;
    while (curr_ing != NULL)
    {
        puntatore_magazzino curr_mgz = seek_magazzino(ht_mgz, curr_ing->name);
        if (curr_ing->quantita * ord->quantita > curr_mgz->quantita_tot)
        {
            return 0;
        }
        curr_ing = curr_ing->next;
    }
    return 1;
}

struct ordine *camioncino(struct ordine *head_ord, struct ordine *carico, struct ricetta *head_ric, int cap)
{
    struct ordine *curr = head_ord;
    int weight = 0;
    while (curr != NULL)
    {
        // if (curr->is_done == 1)
        // {
        //     if (weight + curr->weight <= cap)
        //     {
        //         weight = weight + curr->weight;
        //         if (carico == NULL)
        //         {
        //             carico = crea_ordine(head_ric, curr->ricetta, curr->quantita, curr->time);
        //         }
        //         else
        //         {
        //             add_ordine(carico, head_ric, curr->ricetta, curr->quantita, curr->time);
        //         }
        //     }
        // }
        // curr = curr->next;
    }
    return carico;
}
struct ordine *free_ord(struct ordine *head_ord, struct ordine *carico)
{
    struct ordine *curr_ord = head_ord;
    struct ordine *curr_car = carico;
    struct ordine *temp = NULL;
    while (curr_car != NULL)
    {
        curr_ord = head_ord;
        while (curr_ord != NULL)
        {
            if (strcmp(curr_ord->ricetta, curr_car->ricetta) == 0 && curr_ord->time == curr_car->time && curr_ord->quantita == curr_car->quantita)
            {
                temp = curr_ord;
                curr_ord = curr_ord->next;
               // head_ord = remove_ordine(head_ord, temp->ricetta, temp->time);
            }
            else
            {
                curr_ord = curr_ord->next;
            }
        }
        curr_car = curr_car->next;
    }
    return head_ord;
}
struct ordine *free_carico(struct ordine *carico)
{
    struct ordine *curr = carico;
    struct ordine *prec = NULL;
    int i = 1;
    while (curr != NULL)
    {
        prec = curr;
        curr = curr->next;
        prec->next = NULL;
        free(prec);
        i++;
    }
    return NULL;
}


puntatore_ordine crea_ordine(char* nome_ricetta, int quantita, hashtable_ricette ht_ricette){
    puntatore_ordine new_ordine = (puntatore_ordine)malloc(sizeof(ordine));
    puntatore_ricetta ricetta = seek_ricetta_hashtable(ht_ricette, nome_ricetta);
    new_ordine->ricetta = ricetta->name;
    new_ordine->quantita = quantita;
    new_ordine->is_done = 0;
    new_ordine->weight = quantita * ricetta->weight;
    new_ordine->time = time;
    new_ordine->next = NULL;
    return new_ordine;
}


void cancella_lista_ingredienti_da_ricetta(puntatore_ricetta ricetta){
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
void cancella_ricetta_hashtable(hashtable_ricette ht, puntatore_ricetta ricetta_da_eliminare){
    cancella_lista_ingredienti_da_ricetta(ricetta_da_eliminare);
    size_t hash_indice = hash_string(ricetta_da_eliminare->name) ;
    if(ht[hash_indice] == ricetta_da_eliminare){
        ht[hash_indice] = ht[hash_indice]->next;
        free(ricetta_da_eliminare);
        return;
    }
    puntatore_ricetta curr_ric = ht[hash_indice];
    puntatore_ricetta next_ric = ht[hash_indice]->next;
    while(next_ric != ricetta_da_eliminare){
        curr_ric = next_ric;
        next_ric = next_ric->next;
    }
    curr_ric->next = next_ric->next;
    free(ricetta_da_eliminare);
}

puntatore_ordine aggiungi_ordine_in_lista_ordinata_per_peso(puntatore_ordine lista_dove_aggiungere, puntatore_ordine da_aggiungere){
    
    if(lista_dove_aggiungere == NULL){
        lista_dove_aggiungere = da_aggiungere;
        return lista_dove_aggiungere;
    }
    puntatore_ordine curr_ord = lista_dove_aggiungere;
    while(curr_ord->next != NULL && curr_ord->weight <= da_aggiungere->weight){
        curr_ord = curr_ord->next;
    }
    curr_ord->next = da_aggiungere;
    return lista_dove_aggiungere;

}
puntatore_ordine seleziona_ordini_per_camincino( int capienza){
    puntatore_ordine lista_ordinata_per_peso = NULL;
    puntatore_ordine testa_ordini_pronti = ordini_completati->testa;
    while(testa_ordini_pronti && capienza >= testa_ordini_pronti->weight){
         lista_ordinata_per_peso = aggiungi_ordine_in_lista_ordinata_per_peso(lista_ordinata_per_peso, testa_ordini_pronti);
          testa_ordini_pronti = testa_ordini_pronti->next;
          ordini_completati->testa = testa_ordini_pronti;
    }
    return lista_ordinata_per_peso;
}

int main()
{
    // DA DOVE PRENDERE L'INPUT
    FILE *in1 = stdin;
    // FILE* in1 = fopen("C:\\Users\\Giacomo\\Desktop\\Prog_API\\Open_rand.txt", "r");
    // FILE* in1 = fopen("C:\\Users\\Giacomo\\Desktop\\Prog_API\\comandi.txt", "r");

    // GESTIONE DEL COMANDO
    int boo;
    int r1;
    char r2;
    
    int time_corriere, capienza;
    char comando[1000];
    char *ric;
    char *ing;
    int qnt;
    int scd;
    char *sub;
    int count = 0;
    char **tokens;
    hashtable_ricette ht_ricette = crea_hashtable_ricette();
    hashtable_magazzino ht_magazzino = crea_hashtable_magazzino();
    puntatore_ordine head_ord = NULL;

    // GESTIONE SINGOLE PARTI DI CODICE
    puntatore_ricetta ptr_ric;

    // EXEC
    r1 = fscanf(in1, "%d %d\n", &time_corriere, &capienza);
    r1--;
    do
    {
        if (time != 0 && time % time_corriere == 0)
        {
            if (!ordini_completati)
            {
                fprintf(stdout, "camioncino vuoto\n");
            }
            else
            {
                puntatore_ordine lista_ordini_da_caricare = seleziona_ordini_per_camincino(capienza);
                //ordine_lista_camincino(lista_ordini_da_caricare);
                // put_ordine(lista_ordini_da_caricare);
                // free_lista(lista_ordini_da_caricare);

                // carico = camioncino(head_ord, carico, head_ric, capienza);
                // head_ord = free_ord(head_ord, carico);
                // carico = weight_sort(carico);
                // put_ordine(carico);
                // carico = free_carico(carico);
            }
        }
        r2 = (long int)fgets(comando, sizeof(comando), in1);
        comando[strlen(comando)] = '\0';
        tokens = str_split(comando, ' ', &count);

        if (comando[0] == 'a' && comando[1] == 'g' && comando[2] == 'g')
        {
            ric = *(tokens + 1);
            ptr_ric = seek_ricetta_hashtable(ht_ricette, ric);
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
                    ing = *(tokens + 2 + (2 * i));
                    sub = *(tokens + 3 + (2 * i));
                    qnt = atoi(sub);
                    aggiungi_ingrediente_in_lista(list_ingredienti, ing, qnt);
                    weight = weight + qnt;
                    i++;
                }
                puntatore_ricetta new_ric = crea_ricetta(ric, weight, 0, list_ingredienti);
                add_ricetta_hashtable(ht_ricette, new_ric);
            }
        }
        if (comando[0] == 'r' && comando[1] == 'i' && comando[2] == 'm')
        {
            ric = *(tokens + 1);
            ptr_ric = seek_ricetta_hashtable(ht_ricette, ric);
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
            }
        }
        if (comando[0] == 'r' && comando[1] == 'i' && comando[2] == 'f')
        {
            int i = 0;
            int weight = 0;
            while (*(tokens + 1 + (3 * i)) != NULL)
            {
                ing = *(tokens + 1 + (3 * i));
                sub = *(tokens + 2 + (3 * i));
                qnt = atoi(sub);
                sub = *(tokens + 3 + (3 * i));
                scd = atoi(sub);
                puntatore_lotto new_lotto = crea_lotto(qnt, scd);
                aggiungi_lotto_a_mgz(ht_magazzino, ing, new_lotto);
                i++;
            }
            fprintf(stdout, "rifornito\n");
            // head_rif = check_ordiniSospesi(head_ord, head_ric, head_rif);
        }
        if (comando[0] == 'o' && comando[1] == 'r' && comando[2] == 'd')
        {
            ric = *(tokens + 1);
            sub = *(tokens + 2);
            qnt = atoi(sub);
            if (seek_ricetta_hashtable(ht_ricette, ric) == NULL)
            {
                fprintf(stdout, "rifiutato\n");
            }
            else
            {
                fprintf(stdout, "accettato\n");
                puntatore_ordine new_ordine = crea_ordine(ric, qnt, ht_ricette);
                int ordine_fattibile = is_ordine_fattibile(head_ord, ht_ricette, ht_magazzino);
                if (ordine_fattibile)
                {
                    printf("Preparazione ordine\n");
                    // head_rif = make_ordine(head_ric, head_rif, head_ord, ric, qnt, time);
                }else{
                    add_ordine_in_coda(ordini_in_sospeso, new_ordine);
                }
            }
        }
    } while (r2 != EOF);
    return 0;
}
