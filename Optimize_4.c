#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define DIM_HASH_TABLE 100

char** str_split(char* a_str, const char a_delim, int* count){
    char** result = 0;
    char* tmp = a_str;
    char* last_comma = 0;
    char delim[3];
    delim[0] = a_delim;
    delim[2] = '\n';
    delim[1] = 0;
    /* Count how many elements will be extracted. */
    while (*tmp){
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
    result = malloc(sizeof(char*) * (*count));
    if (result){
        size_t idx  = 0;
        char* token = strtok(a_str, delim);
        while (token){
            assert(idx < (*count));
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == (*count) - 1);
        *(result + idx) = 0;
    }
    //printf("[debug] count %d\n", (*count));
    return result;
}
size_t hash_string (const char* str){
    size_t hash = 5381;
    size_t c;
    while (c = *str++){
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash;
}

struct ingrediente{
    char name[30];
    int quantita;
    struct ingrediente* next;
};
typedef struct ricetta {
    char * name;
    int weight;
    int num_ordini_sospeso;
    struct ingrediente* ingredienti; //lista ingredienti ricetta
    struct ricetta* next; //puntatore al prossima ricetta
} ricetta;
typedef ricetta* puntatore_ricetta;
typedef puntatore_ricetta* hashtable_ricette;

typedef struct lotto{
    int quantita;
    int scadenza;
    struct lotto* next;
} lotto;
typedef lotto* puntatore_lotto;

typedef struct magazzino{
    char ingrediente[30];
    int quantita_tot;
    puntatore_lotto lotti;
    struct magazzino* next;
} magazzino;
typedef magazzino* puntatore_magazzino;
typedef puntatore_magazzino hashtable_magazzino;

typedef struct ordine{
    char ricetta[30];
    int quantita;
    int is_done;
    int weight;
    int time;
    struct ordine* next;
} ordine;
typedef ordine* puntatore_ordine;

hashtable_ricette crea_hashtable_ricette (){
    hashtable_ricette ht = (hashtable_ricette)calloc(DIM_HASH_TABLE, sizeof(puntatore_ricetta));
    return ht;
}
struct ingrediente* crea_ingr (char* ing, int qnt){
    struct ingrediente* new_ing = (struct ingrediente*)malloc(sizeof(struct ingrediente));
    strcpy(new_ing->name, ing);
    new_ing->quantita = qnt;
    new_ing->next = NULL;
    return new_ing;
}
void add_ingr (struct ingrediente* head, char* ing, int qnt){
    if(head == NULL){
        head = crea_ingr(ing, qnt);
    }
    else{
        struct ingrediente* curr_ing = head;
        while(curr_ing->next != NULL){
            curr_ing = curr_ing->next;
        }
        struct ingrediente* new_ing = crea_ingr(ing, qnt);
        curr_ing->next = new_ing;
    }
}
puntatore_ricetta crea_ricetta (char* name, int weight, int ord, struct ingrediente* ing){
    puntatore_ricetta new_ric = (puntatore_ricetta)malloc(sizeof(ricetta));
    strcpy(new_ric->name,  name);
    new_ric->weight = weight;
    new_ric->num_ordini_sospeso = ord;
    new_ric->ingredienti = ing;
    new_ric->next = NULL;
    return new_ric;
}
puntatore_ricetta seek_lista_ricette(puntatore_ricetta head, char* name){
    puntatore_ricetta curr = head;
    while(curr != NULL){
        if(strcmp(curr->name, name) == 0){
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}
puntatore_ricetta seek_ricetta_hashtable(hashtable_ricette ht, const char* nome_ric){
    size_t hash_indice = hash_string(nome_ric) % DIM_HASH_TABLE;
    puntatore_ricetta pointer_ric = seek_lista_ricette(ht[hash_indice], nome_ric);
    return pointer_ric;
}
void aggiungi_ricetta (puntatore_ricetta curr, puntatore_ricetta new_ric){
    if(curr == NULL){
        curr = new_ric;
    }
    else{
        while(curr->next != NULL){
            curr = curr->next;
        }
        curr->next = new_ric;
    }
}
void add_ricetta_hashtable (hashtable_ricette ht, puntatore_ricetta new_ric){
    size_t hash_indice = hash_string(new_ric->name) % DIM_HASH_TABLE;
    puntatore_ricetta curr_ric = ht[hash_indice];
    aggiungi_ricetta(curr_ric, new_ric);
}
void free_ing (puntatore_ricetta curr_ric){
    struct ingrediente* curr_ing = curr_ric->ingredienti;
    struct ingrediente* prec_ing = NULL;
    while(curr_ing != NULL){
        prec_ing = curr_ing;
        curr_ing = curr_ing->next;
        prec_ing->next = NULL;
        free(prec_ing);
    }
}
puntatore_ricetta remove_ricetta (puntatore_ricetta head, char* ric){
    puntatore_ricetta curr = seek_lista_ricette(head, ric);
    if(curr == NULL){
        return head;
    }
    if(curr == head){
        curr = head->next;
        head->next = NULL;
        free_ing(head);
        free(head);
        return curr;
    }
    puntatore_ricetta prec = NULL;
    while(prec->next != curr){
        prec = prec->next;
    }
    prec->next = curr->next;
    free_ing(curr);
    curr->next = NULL;
}
void remove_ricetta_hashtable(hashtable_ricette ht, char* nome_ric){
    size_t hash_indice = hash_string(nome_ric) % DIM_HASH_TABLE;
    puntatore_ricetta curr_ric = ht[hash_indice];
    ht[hash_indice] = remove_ricetta(curr_ric, nome_ric);
}

hashtable_magazzino crea_hashtable_magazzino (){
    hashtable_magazzino ht = (hashtable_magazzino)calloc(DIM_HASH_TABLE, sizeof(puntatore_magazzino));
    return ht;
}
puntatore_magazzino crea_magazzino(char* ing, puntatore_lotto new_lotto){
    puntatore_magazzino new_magazzino = (puntatore_magazzino)malloc(sizeof(magazzino));
    strcpy(new_magazzino->ingrediente, ing);
    new_magazzino->quantita_tot = new_lotto->quantita;
    new_magazzino->lotti = new_lotto;
    new_magazzino->next = NULL;
    return new_magazzino;
}
void aggiungi_magazzino (puntatore_magazzino curr, puntatore_magazzino new_mgz){
    if(curr == NULL){
        curr = new_mgz;
    }
    else{
        while(curr->next != NULL){
            curr = curr->next;
        }
        curr->next = new_mgz;
    }
}
void add_magazzino_hashtable (hashtable_magazzino ht, puntatore_magazzino new_mgz){
    size_t hash_indice = hash_string(new_mgz->ingrediente) % DIM_HASH_TABLE;
    puntatore_magazzino curr_mgz = ht[hash_indice];
    aggiungi_magazzino(curr_mgz, new_mgz);
}
puntatore_lotto crea_lotto (int qnt, int scd){
    puntatore_lotto new_lotto = (puntatore_lotto)malloc(sizeof(lotto));
    new_lotto->quantita = qnt;
    new_lotto->scadenza = scd;
    return new_lotto;
}
puntatore_magazzino seek_magazzino(hashtable_magazzino ht_mgz, char* ing){
    size_t hash_indice = hash_string(new_mgz->ingrediente) % DIM_HASH_TABLE;
    puntatore_magazzino curr_mgz = ht[hash_indice];
    while(curr_mgz != NULL && strcmp(curr_mgz->ingrediente, ing) != 0){
        curr_mgz = curr_mgz->next;
    }
    return curr_mgz;
}
void aggiungi_lotto_a_mgz(hashtable_magazzino ht_mgz, char* ing, puntatore_lotto new_lotto){
    size_t hash_indice = hash_string(ing) % DIM_HASH_TABLE;
    puntatore_magazzino curr_mgz = ht_mgz[hash_indice];
    while(curr_mgz->next != NULL && strcmp(curr_mgz->ingrediente, ing) != 0){
        curr_mgz = curr_mgz->next;
    }
    if(strcmp(curr_mgz->ingrediente, ing) == 0){
        puntatore_lotto curr_lotto = curr_mgz->lotti;
        while(curr_lotto->next->scadenza < new_lotto->scadenza || curr_lotto->next != NULL){
            curr_lotto = curr_lotto->next;
        }
        new_lotto->next = curr_lotto->next;
        curr_lotto->next = new_lotto;
        curr_mgz->quantita_tot = curr_mgz->quantita_tot + new_lotto->quantita;
    }
    if(curr_mgz->next == NULL){
        puntatore_magazzino new_mgz = crea_magazzino(ing, new_lotto);
        aggiungi_magazzino(ht_mgz[hash_indice], new_mgz);
    }
}
int check_ingredienti (puntatore_ordine ord, hashtable_ricette ht_ricette, hashtable_magazzino ht_mgz){
    puntatore_ricetta ric_ord = seek_ricetta_hashtable(ht_ricette, ord->ricetta);
    struct ingrediente* curr_ing = ric_ord->ingredienti;
    while(curr_ing != NULL){
        puntatore_magazzino curr_mgz = seek_magazzino(ht_mgz, curr_ing->name);
        if(curr_ing->quantita * ord->quantita > curr_mgz->quantita_tot){
            return 0;
        }
        curr_ing = curr_ing->next;
    }
    return 1;
}
int thereIsALOneOrder (struct ordine* head, int capienza){
    struct ordine* curr = head;
    if(head == NULL){
        return 0;
    }
    while(curr != NULL && capienza >= curr->weight){
        if(curr->is_done == 1){
            return 1;
        }
        curr = curr->next;
    }
    return 0;
}
struct ordine* camioncino (struct ordine* head_ord, struct ordine* carico, struct ricetta* head_ric, int cap){
    struct ordine* curr = head_ord;
    int weight = 0;
    while(curr != NULL){
        if(curr->is_done == 1){
            if(weight + curr->weight <= cap){;
                weight = weight + curr->weight;
                if(carico == NULL){
                    carico = crea_ordine(head_ric, curr->ricetta, curr->quantita, curr->time);
                }
                else {
                    add_ordine(carico, head_ric, curr->ricetta, curr->quantita, curr->time);
                }
            }
        }
        curr = curr->next;
    }
    return carico;
}
struct ordine* free_ord (struct ordine* head_ord, struct ordine* carico){
    struct ordine* curr_ord = head_ord;
    struct ordine* curr_car = carico;
    struct ordine* temp = NULL;
    while(curr_car != NULL){
        curr_ord = head_ord;
        while(curr_ord != NULL){
            if(strcmp(curr_ord->ricetta, curr_car->ricetta) == 0 && curr_ord->time == curr_car->time && curr_ord->quantita == curr_car->quantita){
                temp = curr_ord;
                curr_ord = curr_ord->next;
                head_ord = remove_ordine(head_ord, temp->ricetta, temp->time);
            }
            else {
                curr_ord = curr_ord->next;
            }

        }
        curr_car = curr_car->next;
    }
    return head_ord;
}
struct ordine* free_carico (struct ordine* carico){
    struct ordine* curr = carico;
    struct ordine* prec = NULL;
    int i=1;
    while(curr != NULL){
        prec = curr;
        curr = curr->next;
        prec->next = NULL;
        free(prec);
        i++;
    }
    return NULL;
}

int main(){
    //DA DOVE PRENDERE L'INPUT
    FILE* in1 = stdin;
    //FILE* in1 = fopen("C:\\Users\\Giacomo\\Desktop\\Prog_API\\Open_rand.txt", "r");
    //FILE* in1 = fopen("C:\\Users\\Giacomo\\Desktop\\Prog_API\\comandi.txt", "r");

    // GESTIONE DEL COMANDO
    int boo;
    int r1;
    char r2;
    int time = 0;
    int time_corriere, capienza;
    char comando[1000];
    char* ric;
    char* ing;
    int qnt;
    int scd;
    char* sub;
    int count = 0;
    char** tokens;
    hashtable_ricette ht_ricette = crea_hashtable_ricette();
    hashtable_magazzino ht_magazzino = crea_hashtable_magazzino();
    puntatore_ordine head_ord = NULL;
    puntatore_ordine tail_ord = NULL;

    // GESTIONE SINGOLE PARTI DI CODICE
    puntatore_ricetta ptr_ric;

    // EXEC
    r1 = fscanf(in1, "%d %d\n", &time_corriere, &capienza);
    r1--;
    do{
        if(time != 0 && time%time_corriere == 0){
            boo = thereIsALOneOrder(head_ord, capienza);
            if(boo == 0){
                fprintf(stdout, "camioncino vuoto\n");
            }
            else {
                carico = camioncino(head_ord, carico, head_ric, capienza);
                head_ord = free_ord(head_ord, carico);
                carico = weight_sort(carico);
                put_ordine(carico);
                carico = free_carico(carico);
            }
        }
        r2 = (long int)fgets(comando, sizeof(comando), in1);
        comando[strlen(comando)] = '\0';
        tokens = str_split(comando, ' ', &count);

        if (comando[0]=='a' && comando[1]=='g' && comando[2]=='g'){
            ric = *(tokens + 1);
            ptr_ric = seek_ricetta_hashtable(ht_ricette, ric);
            if(ptr_ric != NULL){
                fprintf(stdout,"ignorato\n");
            }
            else{
                struct ingrediente* list_ing = NULL;
                int i = 0;
                int weight = 0;
                while(*(tokens + 2 + (2*i)) != NULL){
                    ing = *(tokens + 2 + (2*i));
                    sub = *(tokens + 3 + (2*i));
                    qnt = atoi(sub);
                    add_ingr(list_ing, ing, qnt);
                    weight = weight + qnt;
                    i++;
                }
                puntatore_ricetta new_ric = crea_ricetta(ric, weight, 0, list_ing);
                add_ricetta_hashtable(ht_ricette, new_ric);
            }
        }
        if (comando[0]=='r' && comando[1]=='i' && comando[2]=='m'){
            ric = *(tokens + 1);
            ptr_ric = seek_ricetta_hashtable(ht_ricette, ric);
            if(ptr_ric == NULL){
                fprintf(stdout,"non presente\n");
            }
            else if (ptr_ric->num_ordini_sospeso > 0){
                fprintf(stdout,"ordini in sospeso\n");
            }
            else {
                //remove_ricetta_hashtable(...);
            }
        }
        if (comando[0]=='r' && comando[1]=='i' && comando[2]=='f'){
            int i = 0;
            int weight = 0;
            while(*(tokens + 1 + (3*i)) != NULL){
                ing = *(tokens + 1 + (3*i));
                sub = *(tokens + 2 + (3*i));
                qnt = atoi(sub);
                sub = *(tokens + 3 + (3*i));
                scd = atoi(sub);
                puntatore_lotto new_lotto = crea_lotto(qnt, scd);
                aggiungi_lotto_a_mgz(ht_magazzino, ing, new_lotto);
                i++;
            }
            fprintf(stdout,"rifornito\n");
            //head_rif = check_ordiniSospesi(head_ord, head_ric, head_rif);
        }
        if (comando[0]=='o' && comando[1]=='r' && comando[2]=='d'){
            ric = *(tokens + 1);
            sub = *(tokens + 2);
            qnt = atoi(sub);
            if(seek_ricetta_hashtable(ht_ricette, ric) == NULL){
                fprintf(stdout, "rifiutato\n");
            }
            else {
                fprintf(stdout, "accettato\n");
                if (head_ord == NULL){
                    head_ord = add_ordine(head_ord, ht_ricette, ric, qnt, time);
                    tail_ord = head_ord;
                }
                else {
                    tail_ord = add_ordine(tail_ord, ht_ricette, ric, qnt, time);
                }
                int ordine_fattibile = check_ingredienti(head_ord, head_ric, head_rif, ric, qnt);
                if(ordine_fattibile){
                    printf("Preparazione ordine\n");
                    //head_rif = make_ordine(head_ric, head_rif, head_ord, ric, qnt, time);
                }
            }
        }
    }while (r2 != EOF);
    return 0;
}
