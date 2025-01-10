#ifndef LRU_QUEUE_H
#define LRU_QUEUE_H

#include <stdbool.h>

struct LRUNode {
    int page_number;
    struct LRUNode *next;
};

struct LRUQueue {
    struct LRUNode *head;
    struct LRUNode *tail;
};

bool init_lru_queue(struct LRUQueue *queue);
bool add_page_to_queue(int page_number, struct LRUQueue *queue);
bool dequeue_page(struct LRUQueue *queue, int *page_number);
bool move_page_to_back(int page_number, struct LRUQueue *queue);

#endif