#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "flow_list.h"

struct List *list_init(){
    struct List * l = (struct List *) malloc(sizeof(struct List));

    l->actual = NULL;
    l->last   = NULL;
    l->first  = NULL;
    return l;
}

void list_insert(struct List *l, struct Flow *flow) {
    struct item *new_item = (struct item *)malloc(sizeof(struct item));
    new_item->flow = flow;
    new_item->next = NULL;

    if(l->first == NULL){
        l->first = new_item;
        l->actual = l->first;
        l->last = l->first;
        l->first->prev = NULL;
    }else {
        l->last->next = new_item;
        l->last->next->prev = l->last;
        l->last = l->last->next;
        l->actual = l->last;
    }
}

int list_find_item(struct List *l, struct flow_id *id) {
    struct item *actual = l->first;
    while (actual != NULL) {
        if(is_eq_v4_flow(id, (struct flow_id *) &actual->flow->id)){
            l->actual = actual;
            return 1;
        }
        actual = actual->next;
    }
    return 0;
}

void clear_timers(struct List *l, const int timer){
    time_t actual_time = time(NULL);
    struct item *tmp;
    int duration;
    int cnt = 0;
    while (l->first != NULL) {
        duration = actual_time - get_first_timestamp(l->first->flow);
        if(duration < timer){
            l->actual = l->first;
            break;
        }

        cnt++;
        tmp = l->first;
        l->first = l->first->next;

        if(!tmp->flow->packets.isExported){
            flow_to_str(tmp->flow);
        }

        free(tmp->flow->id);
        free(tmp->flow);
        free(tmp);
    }
}

void dispose_list(struct List *l){
    struct item *tmp;
    while(l->first != NULL){
        tmp = l->first;
        l->first = l->first->next;

        if(!tmp->flow->packets.isExported){
            flow_to_str(tmp->flow);
        }

        free(tmp->flow->id);
        free(tmp->flow);
        free(tmp);
    }
    free(l);
}

void list_remove(struct List *l, struct flow_id *id){
    struct item *tmp;

    if(!is_eq_v4_flow(id,l->actual->flow->id)){
        return;
    }

    if(!l->actual->flow->packets.isExported){
        flow_to_str(l->actual->flow);
    }

    if(is_eq_v4_flow(id,l->first->flow->id)){
        tmp = l->first->next;
        tmp->prev = NULL;

        free(l->first->flow->id);
        free(l->first->flow);
        free(l->first);
        l->first = tmp;
    }else{
        if(l->actual == l->last){
            tmp = l->actual->prev;

            free(l->actual->flow->id);
            free(l->actual->flow);
            free(l->actual);

            tmp->next = NULL;
            l->actual = l->last = tmp;
        }else{
            tmp = l->actual;
            l->actual->prev->next = l->actual->next;
            l->actual = tmp->next;

            free(tmp->flow->id);
            free(tmp->flow);
            free(tmp);
        }
    }
}
