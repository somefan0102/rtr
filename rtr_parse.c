#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "xminl/xminl.h"
#include "rtr.h"

void feed_free(struct Feed_Channel *channel) {
    struct Feed_Item *item, *next;

    free(channel->title);
    free(channel->link);
    free(channel->description);

    for (item = channel->item_head; item; item = next) {
        next = item->next;
        free(item->title);
        free(item->link);
        free(item->description);
        free(item->date);
        free(item);
    }
}

int parse_feed(struct XMINL_Handler *x, struct Feed_Channel *channel, int is_atom) {
    int is_item = 0;
    int i;
    struct XMINL_Token *tokens = x->token_buf;
    struct Feed_Item *item;

    memset(channel, 0, sizeof(struct Feed_Channel));

    for (i = 0; tokens[i].type && i < x->token_len; i++) {
        char **selected = NULL;

        if (tokens[i].type == XMINL_TOKEN_TAG_START) {
            if (is_item) {
                if (!strcmp(tokens[i].value, "title")) {
                    selected = &item->title;
                } else if (!strcmp(tokens[i].value, "link") && !is_atom) {
                    selected = &item->link;
                } else if (!strcmp(tokens[i].value, is_atom ? "summary" : "description")) {
                    selected = &item->description;
                } else if (!strcmp(tokens[i].value, is_atom ? "updated" : "pubDate")) {
                    selected = &item->date;
                }
            } else {
                if (!strcmp(tokens[i].value, "title")) {
                    selected = &channel->title;
                } else if (!strcmp(tokens[i].value, "link") && !is_atom) {
                    selected = &channel->link;
                } else if (!strcmp(tokens[i].value, is_atom ? "subtitle" : "description")) {
                    selected = &channel->description;
                } else if (!strcmp(tokens[i].value, is_atom ? "entry" : "item")) {
                    is_item = 1;
                    if (channel->item_head) {
                        item = item->next = malloc(sizeof(struct Feed_Item));
                    } else {
                        item = channel->item_head = malloc(sizeof(struct Feed_Item));
                    }
                    if (!item) {
                        feed_free(channel);
                        return 1;
                    }
                    memset(item, 0, sizeof(struct Feed_Item));
                }
            }
        } else if (is_item && tokens->type == XMINL_TOKEN_TAG_END && !strcmp(tokens[i].value, is_atom ? "entry" : "item")) {
            is_item = 0;
        }

        if (selected) {
            if (!(*selected = malloc(sizeof(char) * CDATA_BUFSIZE))) {
                feed_free(channel);
                return 1;
            }
            i++;
            memcpy(*selected, tokens[i].value, strlen(tokens[i].value)+1);
        }
    }

    return 0;
}