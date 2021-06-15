#include "packet_parser.h"

struct item {
    struct Flow *flow;
    struct item *next;
    struct item *prev;
};

struct List {
    struct item *first;
    struct item *actual;
    struct item *last;
};

struct List *list_init();

void list_insert(struct List *l, struct Flow *flow);

int list_find_item(struct List *l, struct flow_id *id);

void list_remove(struct List *l, struct flow_id *id);

void dispose_list(struct List *l);

void clear_timers(struct List *l, const int timer);
