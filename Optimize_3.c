#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

struct ingrediente{
    char name[30];
    int quantita;
    struct ingrediente* next;
};
struct ingrediente* crea_ingr (char* ing, int qnt){
    struct ingrediente* new_ing = (struct ingrediente*)malloc(sizeof(struct ingrediente));
    strcpy(new_ing->name, ing);
    new_ing->quantita = qnt;
    new_ing->next = NULL;
    return new_ing;
}
struct ricetta{
    char name[30];
    struct ingrediente* ingr;
    struct ricetta* next;
};
void add_ingr (struct ricetta* head, char* ric, char* ing, int qnt){
    struct ricetta* curr = head;
    struct ingrediente* curr_ing;

    while(curr != NULL && strcmp(curr->name, ric) != 0){
        curr = curr->next;
    }
    if(strcmp(curr->name, ric) == 0){
        if(curr->ingr == NULL){
            struct ingrediente* new_ing = crea_ingr(ing, qnt);
            curr->ingr = new_ing;
        }
        else{
            curr_ing = curr->ingr;
            while(curr_ing->next != NULL){
                curr_ing = curr_ing->next;
            }
            struct ingrediente* new_ing = crea_ingr(ing, qnt);
            curr_ing->next = new_ing;
        }
    }
}
struct ricetta* crea_ricetta (char* ric){
    struct ricetta* new_ric = (struct ricetta*)malloc(sizeof(struct ricetta));
    if(new_ric == NULL){
        return NULL;
    }
    strcpy(new_ric->name, ric);
    new_ric->ingr = NULL;
    new_ric->next = NULL;
    return new_ric;
}
void add_ricetta (struct ricetta* head, char* ric){
    struct ricetta* curr = head;
    while(curr->next != NULL){
        curr = curr->next;
    }
    struct ricetta* new_ric = crea_ricetta(ric);
    curr->next = new_ric;
}
int ricetta_alredy_exist (struct ricetta* head, char* ric){
    struct ricetta* curr = head;
    if(head == NULL){
        return 0;
    }
    while(curr != NULL){
        if(strcmp(curr->name, ric) == 0){
            return 1;
        }
        else{
            curr = curr->next;
        }
    }
    return 0;
}
struct ricetta* remove_ricetta (struct ricetta* head, char* ric){
    struct ricetta* curr = head;
    struct ricetta* prec = NULL;
    struct ingrediente* curr_ing;
    struct ingrediente* prec_ing = NULL;
    while(curr != NULL && strcmp(curr->name, ric) != 0){
        prec = curr;
        curr = curr->next;
    }
    if(strcmp(curr->name, ric) == 0){
        curr_ing = curr->ingr;
        while(curr_ing != NULL){
            prec_ing = curr_ing;
            curr_ing = curr_ing->next;
            free(prec_ing);
        }
        free(prec_ing);
        if(curr == head){
            head = curr->next;
            curr->next = NULL;
        }
        else{
            prec->next = curr->next;
            curr->next = NULL;
        }
        free(curr);
    }
    return head;
}
int peso_ricetta (struct ricetta* head, char* ric){
    struct ricetta* curr = head;
    struct ingrediente* curr_ing;
    int weight = 0;
    while(curr != NULL && strcmp(curr->name, ric) != 0){
        curr = curr->next;
    }
    if(strcmp(curr->name, ric) == 0){
        curr_ing = curr->ingr;
        while(curr_ing != NULL){
            weight = weight + curr_ing->quantita;
            curr_ing = curr_ing->next;
        }
    }
    return weight;
}
struct lotto{
    char ingrediente[30];
    int quantita;
    int scadenza;
    struct lotto* next;
};
struct lotto* crea_lotto (char* ing, int qnt, int scd){
    struct lotto* new_ltt = (struct lotto*)malloc(sizeof(struct lotto));
    strcpy(new_ltt->ingrediente, ing);
    new_ltt->quantita = qnt;
    new_ltt->scadenza = scd;
    new_ltt->next = NULL;
    return new_ltt;
}
void add_lotto (struct lotto* head, char* ing, int qnt, int scd){
    struct lotto* curr = head;
    while(curr->next != NULL){
        curr = curr->next;
    }
    struct lotto* new_ltt = crea_lotto(ing, qnt, scd);
    curr->next = new_ltt;
}
struct lotto* remove_lotto (struct lotto* head, char* ing, int qnt, int scd){
    struct lotto* curr = head;
    struct lotto* prec = NULL;
    while(curr != NULL && (strcmp(curr->ingrediente, ing) != 0 || curr->quantita != qnt || curr->scadenza != scd)){
        prec = curr;
        curr = curr->next;
    }
    if(strcmp(curr->ingrediente, ing) == 0 && curr->quantita == qnt && curr->scadenza == scd){
        if(curr->next == head->next){
            prec = curr;
            curr = curr->next;
            prec->next = NULL;
            free(prec);
            return curr;
        }
        else{
            prec->next = curr->next;
            curr->next = NULL;
            free(curr);
            return head;
        }
    }
    return head;
}
struct lotto* scd_sort (struct lotto* head){
    int bo = 0;
    struct lotto *prec = head;
    struct lotto *curr = head->next;
    struct lotto *temp;
    if(curr == NULL){
        return head;
    }
    if(head->scadenza > head->next->scadenza){
        temp = head;
        head = head->next;
        temp->next = head->next;
        head->next = temp;
        bo = 1;
    }
    while(curr->next != NULL){
        if(curr->scadenza > curr->next->scadenza){
            temp = curr;
            curr = curr->next;

            prec->next = curr;
            temp->next = curr->next;
            curr->next = temp;
            bo = 1;
        }
        prec = curr;
        curr = curr->next;
    }
    if(bo == 1){
        scd_sort(head);
    }
    return head;
}
struct ordine{
    char ricetta[30];
    int quantita;
    int is_done;
    int weight;
    int time;
    struct ordine* next;
};
struct ordine* crea_ordine (struct ricetta* head, char* ric, int qnt, int t){
    struct ordine* new_ord = (struct ordine*)malloc(sizeof(struct ordine));

    if (new_ord == NULL) {
        return NULL;
    }
    strcpy(new_ord->ricetta, ric);
    new_ord->quantita = qnt;
    new_ord->is_done = 0;
    new_ord->weight = peso_ricetta(head, ric) * qnt;
    new_ord->time = t;
    new_ord->next = NULL;
    return new_ord;
}
void add_ordine (struct ordine* head, struct ricetta* head_ric, char* ric, int qnt, int t){
    struct ordine* curr = head;
    while(curr->next != NULL){
        curr = curr->next;
    }
    struct ordine* new_ord = crea_ordine(head_ric, ric, qnt, t);
    curr->next = new_ord;
}
struct ordine* remove_ordine (struct ordine* head, char* ric, int time){
    struct ordine* curr = head;
    struct ordine* prec = NULL;
    while(curr != NULL && (strcmp(curr->ricetta, ric) != 0 || curr->time != time)){
        prec = curr;
        curr = curr->next;
    }
    if(strcmp(curr->ricetta, ric) == 0){
        if(curr->next == head->next){
            prec = curr;
            curr = curr->next;
            prec->next = NULL;
            free(prec);
            return curr;
        }
        else{
            prec->next = curr->next;
            curr->next = NULL;
            free(curr);
            return head;
        }
    }
    return head;
}
int check_ingredienti (struct ordine* head_ord, struct ricetta* head_ric, struct lotto* head_rif, char* ric, int qnt){
    struct ordine* curr_ord = head_ord;
    struct ricetta *curr_ric = head_ric;
    struct ingrediente *curr_ing;
    struct lotto *curr_rif;
    int sum_qnt;
    int bo = 1;
    if(head_rif == NULL){
        return 0;
    }
    while(curr_ord != NULL && (strcmp(curr_ord->ricetta, ric) != 0 || curr_ord->quantita != qnt)){
        curr_ord = curr_ord->next;
    }
    if(curr_ord == NULL){
        return 0;
    }
    while(curr_ric != NULL && strcmp(curr_ric->name, ric) != 0){
        curr_ric = curr_ric->next;
    }
    if(curr_ric == NULL){
        return 0;
    }
    curr_ing = curr_ric->ingr;
    while(curr_ing != NULL){
        curr_rif = head_rif;
        sum_qnt = 0;
        while(curr_rif != NULL){
            if(strcmp(curr_rif->ingrediente, curr_ing->name) == 0){
                sum_qnt = sum_qnt + curr_rif->quantita;
                if(sum_qnt >= curr_ing->quantita * curr_ord->quantita){
                    bo = 1;
                }
                else {
                    bo = 0;
                }
            }
            curr_rif = curr_rif->next;
        }
        if(bo == 0){
            return 0;
        }
        curr_ing = curr_ing->next;
    }
    return 1;
}
struct lotto* make_ordine (struct ricetta* head_ric, struct lotto* head_rif, struct ordine* head_ord, char* ric, int qnt_ord, int time_ord){
    struct ricetta *curr_ric = head_ric;
    struct lotto *curr_rif = head_rif;
    struct lotto *temp = NULL;
    struct ordine *curr_ord = head_ord;
    struct ingrediente *curr_ing;
    int residuo;
    while(curr_ord != NULL && (strcmp(curr_ord->ricetta, ric) != 0 || curr_ord->time != time_ord)){
        curr_ord = curr_ord->next;
    }
    if(strcmp(curr_ord->ricetta, ric) == 0 && curr_ord->time != time_ord){
        while(curr_ric != NULL && strcmp(curr_ric->name, ric) != 0){
            curr_ric = curr_ric->next;
        }
        curr_ing = curr_ric->ingr;
        while(curr_ing != NULL){
            curr_rif = head_rif;
            residuo = qnt_ord * curr_ing->quantita;
            while(curr_rif != NULL && residuo != 0){
                if(strcmp(curr_ing->name, curr_rif->ingrediente) == 0){
                    if(curr_rif->quantita - residuo > 0){
                        curr_rif->quantita = curr_rif->quantita - residuo;
                        residuo = 0;
                    }
                    else {
                        residuo = (curr_rif->quantita - residuo) * (-1);
                        temp = curr_rif;
                        curr_rif = curr_rif->next;
                        head_rif = remove_lotto(head_rif, temp->ingrediente, temp->quantita, temp->scadenza);
                    }
                }
                else {
                    curr_rif = curr_rif->next;
                }
            }
            curr_ing = curr_ing->next;
        }
    }
    curr_ord->is_done = 1;
    return head_rif;
}
struct ordine* weight_sort (struct ordine* head){
    int bo = 0;
    struct ordine *prec = head;
    struct ordine *curr = head->next;
    struct ordine *temp;
    if(curr == NULL){
        return head;
    }
    if(head->weight < head->next->weight){
        temp = head;
        head = head->next;

        temp->next = head->next;
        head->next = temp;
        bo = 1;
    }
    while(curr->next != NULL){
        if(curr->weight < curr->next->weight){
            temp = curr;
            curr = curr->next;

            prec->next = curr;
            temp->next = curr->next;
            curr->next = temp;
            bo = 1;
        }
        prec = curr;
        curr = curr->next;
    }
    if(bo == 1){
        weight_sort(head);
    }
    return head;
}
int check_ordineIncorso (struct ordine* head, char* ric){
    struct ordine* curr = head;
    if(curr == NULL){
        return 0;
    }
    while(curr->next != NULL && strcmp(curr->ricetta, ric) != 0){
        curr = curr->next;
    }
    if (strcmp(curr->ricetta, ric) == 0){
        return 1;
    }
    else{
        return 0;
    }
}
struct lotto* check_ordiniSospesi (struct ordine* head_ord, struct ricetta* head_ric, struct lotto* head_rif){
    struct ordine *curr = head_ord;
    int bo = 0;
    while(curr != NULL){
        if(curr->is_done == 0){
            bo = check_ingredienti(head_ord, head_ric, head_rif, curr->ricetta, curr->quantita);
            if(bo == 1){
                head_rif = make_ordine(head_ric, head_rif, head_ord, curr->ricetta, curr->quantita, curr->time);
            }
        }
        curr = curr->next;
    }
    return head_rif;
}
void put_ordine (struct ordine* head){
    struct ordine* curr = head;
    while(curr != NULL){
        fprintf(stdout, "%d %s %d\n", curr->time, curr->ricetta, curr->quantita);
        curr = curr->next;
    }
}
struct lotto* remove_lotti_scd (struct lotto* head, int time){
    struct lotto *curr = head;
    struct lotto *temp = NULL;
    while(curr != NULL){
        if(time >= curr->scadenza){
            temp = curr;
            curr = curr->next;
            head = remove_lotto(head, temp->ingrediente, temp->quantita, temp->scadenza);
        }
        else{
            curr = curr->next;
        }
    }
    return head;
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

int main(){
    FILE* in1 = stdin;
    //FILE* in1 = fopen("C:\\Users\\Giacomo\\Desktop\\Prog_API\\Open_rand.txt", "r");
    //FILE* in1 = fopen("C:\\Users\\Giacomo\\Desktop\\Prog_API\\comandi.txt", "r");
    int boo;
    int r1;
    char r2;
    int time = 0;
    int time_corriere, capienza;
    char comando[1000];
    struct ricetta* head_ric = NULL;
    struct lotto* head_rif = NULL;
    struct ordine* head_ord = NULL;
    struct ordine* carico = NULL;
    char* ric;
    char* ing;
    int qnt;
    char* sub;
    int scd;
    int count = 0;
    char** tokens;
    int i;
    //printf("[debug] start\n");
    r1 = fscanf(in1, "%d %d\n", &time_corriere, &capienza);
    //printf("[debug] %d %d\n", time_corriere, capienza);
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
        //printf("[debug] %s\n", comando);
        tokens = str_split(comando, ' ', &count);
        /*
        for(i = 0 ; i < count; i++){
            printf("[debug] token cmd %d: '%s'\n", i, *(tokens + i));
            //free(*(tokens + i));
        }
        //*/
        count = 0;
        if (strstr(comando, "exit") != NULL) {
            break;
        }
        if (comando[0]=='a' && comando[1]=='g' && comando[2]=='g'){
            ric = *(tokens + 1);
            if (ricetta_alredy_exist(head_ric, ric) == 1) {
                fprintf(stdout,"ignorato\n");
            }
            else {
                if (head_ric == NULL){
                    head_ric = crea_ricetta(ric);
                }
                else {
                    add_ricetta(head_ric, ric);
                }
                boo = 0;
                i = 0;
                do{
                    //printf("[debug] token %d: '%s'\n", 2+(2*i), *(tokens + 2 + (2*i)));
                    ing = *(tokens + 2 + (2*i));
                    //printf("[debug] ingrediente '%s'\n", ing);
                    //printf("[debug] token %d: '%s'\n", 3+(2*i), *(tokens + 3 + (2*i)));
                    sub = *(tokens + 3 + (2*i));
                    //printf("[debug] sub '%s'\n", sub);
                    qnt = atoi(sub);
                    //printf("[debug] quantita '%d'\n", qnt);
                    i++;
                    if(*(tokens + 2 + (2*i)) ==  NULL){
                        boo = 1;
                    }
                    add_ingr(head_ric, ric, ing, qnt);
                } while(boo == 0);
                i = 0;
                fprintf(stdout,"aggiunta\n");
            }
        }
        if (comando[0]=='r' && comando[1]=='i' && comando[2]=='m'){
            ric = *(tokens + 1);
            if(check_ordineIncorso(head_ord, strcat(ric, "\n")) == 1){
                fprintf(stdout,"ordini in sospeso\n");
            }
            else if (ricetta_alredy_exist(head_ric, strcat(ric, "\n")) == 1){
                head_ric = remove_ricetta(head_ric, ric);
                fprintf(stdout,"rimossa\n");
            }
            else {
                fprintf(stdout,"non presente\n");
            }
        }
        if (comando[0]=='r' && comando[1]=='i' && comando[2]=='f'){
            boo = 0;
            i = 0;
            do{
                //printf("[debug] token %d: '%s'\n", 1+(3*i), *(tokens + 1 + (3*i)));
                ing = *(tokens + 1 + (3*i));
                //printf("[debug] ingrediente '%s'\n", ing);
                //printf("[debug] token %d: '%s'\n", 2+(3*i), *(tokens + 2 + (3*i)));
                sub = *(tokens + 2 + (3*i));
                qnt = atoi(sub);
                //printf("[debug] quantita '%d'\n", qnt);
                //printf("[debug] token %d: '%s'\n", 3+(3*i), *(tokens + 3 + (3*i)));
                sub = *(tokens + 3 + (3*i));
                scd = atoi(sub);
                //printf("[debug] scadenza '%d'\n", scd);
                i++;
                if(*(tokens + 1 + (3*i)) ==  NULL){
                    boo = 1;
                }
                if (head_rif == NULL){
                    head_rif = crea_lotto(ing, qnt, scd);
                }
                else {
                    add_lotto(head_rif, ing, qnt, scd);
                }
            } while(boo == 0);
            fprintf(stdout,"rifornito\n");
            head_rif = scd_sort(head_rif);
            head_rif = check_ordiniSospesi(head_ord, head_ric, head_rif);
        }
        if (comando[0]=='o' && comando[1]=='r' && comando[2]=='d'){
            boo = 1;
            ric = *(tokens + 1);
            sub = *(tokens + 2);
            qnt = atoi(sub);
            if(ricetta_alredy_exist(head_ric, ric) == 0){
                fprintf(stdout, "rifiutato\n");
            }
            else {
                fprintf(stdout, "accettato\n");
                if (head_ord == NULL){
                    head_ord = crea_ordine(head_ric, ric, qnt, time);
                }
                else {
                    add_ordine(head_ord, head_ric, ric, qnt, time);
                }
                boo = check_ingredienti(head_ord, head_ric, head_rif, ric, qnt);
                if(boo == 1){
                    head_rif = make_ordine(head_ric, head_rif, head_ord, ric, qnt, time);
                }
            }
        }
        time++;
        if(head_rif != NULL){
            head_rif = remove_lotti_scd(head_rif, time);
        }
    }while (r2 != EOF);
    //printf("[debug] END \n");
    return 0;
}
