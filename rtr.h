#ifndef RTR_H

#define TOKENS_BUFSIZE 0xffff
#define CDATA_BUFSIZE 0xffff
#define DOCUMENT_BUFSIZE 0xffff

struct Feed_Channel {
    char *title;
    char *link;
    char *description;
    struct Feed_Item *item_head;
};

struct Feed_Item {
    char *title;
    char *link;
    char *description;
    char *date;
    struct Feed_Item *next;
};

/* defined in rtr_parse.c */
void feed_free(struct Feed_Channel *channel);
int parse_feed(struct XMINL_Handler *x, struct Feed_Channel *channel, int is_atom);

#endif

