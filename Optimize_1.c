#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int* boo;
int* r1;
char* r2;

void removeSubstring (char *str, char *sub) {
    char *pos;
    int len = strlen(sub);

    if ((pos = strstr(str, sub)) != NULL) {
        memmove(pos, pos + len, strlen(pos + len) + 1);
    }
}
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
struct ricetta* lookfor_ric (struct ricetta* head_ric, char* ric){
    struct ricetta* curr = head_ric;
    while(curr != NULL){
        if(strcmp(curr->name, ric) == 0){
            return curr;
        }
        else{
            curr = curr->next;
        }
    }
    return NULL;
}
struct ingrediente* lookfor_ing (struct ricetta* head_ric, char* ric){
    struct ricetta* curr_ric = lookfor_ric(head_ric, ric);
    struct ingrediente* ing = curr_ric->ingr;
    while(ing->next != NULL){
        ing = ing->next;
    }
    return ing;
}
struct ingrediente* add_ingr (struct ingrediente* prec_ing, char* ing, int qnt){
    struct ingrediente* curr_ing = crea_ingr(ing, qnt);
    prec_ing->next = curr_ing;
    return curr_ing;
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
struct ricetta* add_ricetta (struct ricetta* head, char* ric){
    struct ricetta* curr = head;
    while(curr->next != NULL){
        curr = curr->next;
    }
    struct ricetta* new_ric = crea_ricetta(ric);
    curr->next = new_ric;
    return head;
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
struct lotto* lookfor_ltt (struct lotto* head_rif){
    struct lotto* curr = head_rif;
    while(curr->next != NULL){
        curr = curr->next;
    }
    return curr;
}
struct lotto* crea_lotto (char* ing, int qnt, int scd){
    struct lotto* new_ltt = (struct lotto*)malloc(sizeof(struct lotto));
    strcpy(new_ltt->ingrediente, ing);
    new_ltt->quantita = qnt;
    new_ltt->scadenza = scd;
    new_ltt->next = NULL;
    return new_ltt;
}
struct lotto* add_lotto (struct lotto* prec_ltt, char* ing, int qnt, int scd){
    struct lotto* curr_ltt = NULL;
    curr_ltt = crea_lotto(ing, qnt, scd);
    prec_ltt->next = curr_ltt;
    return curr_ltt;
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
struct ordine* lookfor_ord (struct ordine* head_ord, char* ric, int qnt, int time){
    struct ordine* curr = head_ord;
    while(curr != NULL){
        if(strcmp(curr->ricetta, ric) == 0 && curr->quantita == qnt && curr->time == time){
            return curr;
        }
        else{
            curr = curr->next;
        }
    }
    return NULL;
}
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
struct ordine* add_ordine (struct ordine* head, struct ricetta* head_ric, char* ric, int qnt, int t){
    struct ordine* curr = head;
    while(curr->next != NULL){
        curr = curr->next;
    }
    struct ordine* new_ord = crea_ordine(head_ric, ric, qnt, t);
    curr->next = new_ord;
    return head;
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
int check_ingredienti (struct ricetta* ric, struct lotto* head_rif, struct ordine* ord){
    struct ingrediente *curr_ing;
    struct lotto *curr_rif;
    int sum_qnt;
    int bo = 1;
    if(head_rif == NULL){
        return 0;
    }
    curr_ing = ric->ingr;
    while(curr_ing != NULL){
        curr_rif = head_rif;
        sum_qnt = 0;
        while(curr_rif != NULL){
            if(strcmp(curr_rif->ingrediente, curr_ing->name) == 0){
                sum_qnt = sum_qnt + curr_rif->quantita;
                if(sum_qnt >= curr_ing->quantita * ord->quantita){
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
struct lotto* make_ordine (struct ricetta* ric, struct lotto* head_rif, struct ordine* ord){
    struct lotto *curr_rif = head_rif;
    struct lotto *temp = NULL;
    struct ingrediente *curr_ing;
    int residuo;
    curr_ing = ric->ingr;
    while(curr_ing != NULL){
        curr_rif = head_rif;
        residuo = ord->quantita * curr_ing->quantita;
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
    ord->is_done = 1;
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
            bo = check_ingredienti(head_ric, head_rif, curr);
            if(bo == 1){
                head_rif = make_ordine(head_ric, head_rif, curr);
            }
        }
        curr = curr->next;
    }
    return head_rif;
}
void put_ordine (struct ordine* head){
    struct ordine* curr = head;
    while(curr != NULL){
        fprintf(stdout, "%d %s%d\n", curr->time, curr->ricetta, curr->quantita);
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
                    carico = add_ordine(carico, head_ric, curr->ricetta, curr->quantita, curr->time);
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

int main() {
    r1 = (int*)malloc(sizeof(int));
    r2 = (char*)malloc(sizeof(char));
    int time = 0;
    int time_corriere, capienza;
    char comando[101];
    char copy[101];
    char ric[30];
    struct ricetta* head_ric = NULL;
    struct lotto* head_rif = NULL;
    struct ordine* head_ord = NULL;
    struct ordine* carico = NULL;
    *r1 = fscanf(stdin, "%d %d", &time_corriere, &capienza);
    getchar();
    while (1) {
        if(time != 0 && time%time_corriere == 0){
            boo = (int*)malloc(sizeof(int));
            *boo = thereIsALOneOrder(head_ord, capienza);
            if(*boo == 0){
                fprintf(stdout, "camioncino vuoto\n");
            }
            else {
                carico = camioncino(head_ord, carico, head_ric, capienza);
                head_ord = free_ord(head_ord, carico);
                carico = weight_sort(carico);
                put_ordine(carico);
                carico = free_carico(carico);
            }
            free(boo);
        }
        r2 = fgets(comando, sizeof(comando), stdin);
        strcpy(copy, comando);
        if (strstr(comando, "exit") != NULL) {
            break;
        }
        if (strstr(comando, "aggiungi_ricetta") != NULL) {
            char ing[30];
            int qnt;
            char sub[4];
            char c;
            struct ricetta* ric_s = NULL;
            struct ingrediente* ing_s = NULL;
            removeSubstring(copy, "aggiungi_ricetta ");
            sscanf(copy, "%s", ric);
            removeSubstring(copy, strcat(ric, " "));
            if (ricetta_alredy_exist(head_ric, ric) == 1) {
                fprintf(stdout,"ignorato\n");
            }
            else {
                if (head_ric == NULL){
                    head_ric = crea_ricetta(ric);
                    ric_s = head_ric;
                }
                else {
                    head_ric = add_ricetta(head_ric, ric);
                    ric_s = lookfor_ric(head_ric, ric);
                }
                boo = (int*)malloc(sizeof(int));
                *boo = 0;
                while (*boo == 0){
                    if(ric_s->ingr == NULL){
                        sscanf(copy, "%s", ing);
                        removeSubstring(copy, strcat(ing, " "));
                        sscanf(copy, "%d%c", &qnt, &c);
                        sprintf(sub, "%d", qnt);
                        if(c == '\n'){
                            *boo = 1;
                        }
                        else {
                            removeSubstring(copy, strcat(sub, " "));
                        }
                        ric_s->ingr = crea_ingr(ing, qnt);
                        ing_s = lookfor_ing(head_ric, ric);
                    }
                    else{
                        sscanf(copy, "%s", ing);
                        removeSubstring(copy, strcat(ing, " "));
                        sscanf(copy, "%d%c", &qnt, &c);
                        sprintf(sub, "%d", qnt);
                        if(c == '\n'){
                            *boo = 1;
                        }
                        else {
                            removeSubstring(copy, strcat(sub, " "));
                        }
                        ing_s = add_ingr(ing_s, ing, qnt);
                    }
                }
                free(boo);
                fprintf(stdout,"aggiunta\n");
            }
            time++;
        }
        if (strstr(comando, "rimuovi_ricetta") != NULL){
            removeSubstring(copy, "rimuovi_ricetta ");
            sscanf(copy, "%s", ric);
            if(check_ordineIncorso(head_ord, strcat(ric, " ")) == 1){
                fprintf(stdout,"ordini in sospeso\n");
            }
            else if (ricetta_alredy_exist(head_ric, strcat(ric, " ")) == 1){
                head_ric = remove_ricetta(head_ric, ric);
                fprintf(stdout,"rimossa\n");
            }
            else {
                fprintf(stdout,"non presente\n");
            }
            time++;
        }
        if (strstr(comando, "rifornimento") != NULL){
            char ing[30];
            int qnt;
            int scd;
            char sub[4];
            char c;
            struct lotto* ltt_s;
            boo = (int*)malloc(sizeof(int));
            *boo = 0;
            removeSubstring(copy, "rifornimento ");
            do{
                if (head_rif == NULL){
                    sscanf(copy, "%s", ing);
                    removeSubstring(copy, strcat(ing, " "));
                    sscanf(copy, "%d", &qnt);
                    sprintf(sub, "%d", qnt);
                    removeSubstring(copy, strcat(sub, " "));;
                    sscanf(copy, "%d%c", &scd, &c);
                    sprintf(sub, "%d", scd);
                    if(c == '\n'){
                        *boo = 1;
                    }
                    else {
                        removeSubstring(copy, strcat(sub, " "));
                    }
                    head_rif = crea_lotto(ing, qnt, scd);
                    ltt_s = lookfor_ltt(head_rif);
                }
                else {
                    sscanf(copy, "%s", ing);
                    removeSubstring(copy, strcat(ing, " "));
                    sscanf(copy, "%d", &qnt);
                    sprintf(sub, "%d", qnt);
                    removeSubstring(copy, strcat(sub, " "));;
                    sscanf(copy, "%d%c", &scd, &c);
                    sprintf(sub, "%d", scd);
                    if(c == '\n'){
                        *boo = 1;
                    }
                    else {
                        removeSubstring(copy, strcat(sub, " "));
                    }
                    ltt_s = add_lotto(ltt_s, ing, qnt, scd);
                }
            } while(*boo == 0);
            free(boo);
            fprintf(stdout,"rifornito\n");
            head_rif = scd_sort(head_rif);
            head_rif = check_ordiniSospesi(head_ord, head_ric, head_rif);
            time++;
        }
        if (strstr(comando, "ordine") != NULL){
            int qnt;
            struct ordine* ord_s = NULL;
            struct ricetta* ric_s = NULL;
            boo = (int*)malloc(sizeof(int));
            *boo = 1;
            removeSubstring(copy, "ordine ");
            sscanf(copy, "%s %d", ric, &qnt);
            if(ricetta_alredy_exist(head_ric, strcat(ric, " ")) == 0){
                fprintf(stdout, "rifiutato\n");
            }
            else {
                fprintf(stdout, "accettato\n");

                if (head_ord == NULL){
                    head_ord = crea_ordine(head_ric, ric, qnt, time);
                }
                else {
                    head_ord = add_ordine(head_ord, head_ric, ric, qnt, time);
                }
                ord_s = lookfor_ord(head_ord, ric, qnt, time);
                ric_s = lookfor_ric(head_ric, ric);
                *boo = check_ingredienti(ric_s, head_rif, ord_s);
                if(*boo == 1){
                    head_rif = make_ordine(ric_s, head_rif, ord_s);
                }
            }
            free(boo);
            time++;
        }
        if(head_rif != NULL){
            head_rif = remove_lotti_scd(head_rif, time);
        }
    }
    free(r1);
    free(r2);
    return 0;
}

