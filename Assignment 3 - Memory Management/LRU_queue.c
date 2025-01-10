#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

struct LRUQueue {
    struct LRUNode *head;
    struct LRUNode *tail;
};

struct LRUNode {
    int page_number;
    struct LRUNode *next;
};

bool init_lru_queue(struct LRUQueue *queue) {
    if (!queue) return false;
    
    queue->head = NULL;
    queue->tail = NULL;
    return true;
}

/**
 * Adds a page to the end of the queue
 */
bool add_page_to_queue(int page_number, struct LRUQueue *queue) {
    if (!queue) return false;

    struct LRUNode *new_node = malloc(sizeof(struct LRUNode));
    if (!new_node) return false;

    new_node->page_number = page_number;
    new_node->next = NULL;

    if (!queue->head) {
        queue->head = new_node;
        queue->tail = new_node;
    } else {
        queue->tail->next = new_node;
        queue->tail = new_node;
    }
    return true;
}


bool dequeue_page(struct LRUQueue *queue, int *page_number) {
    if (!queue || !queue->head || !page_number) return false;

    struct LRUNode *temp = queue->head;
    *page_number = temp->page_number;
    queue->head = queue->head->next;
    
    if (!queue->head) {
        queue->tail = NULL;
    }
    
    free(temp);
    return true;
}

/**
 * Moves a page to the back of the queue or adds it if not present
 */
bool move_page_to_back(int page_number, struct LRUQueue *queue) {
    if (!queue) return false;

    // Check if page is already at tail
    if (queue->tail && queue->tail->page_number == page_number) {
        return true;
    }

    // Find page in queue
    struct LRUNode *current = queue->head;
    struct LRUNode *prev = NULL;
    
    while (current && current->page_number != page_number) {
        prev = current;
        current = current->next;
    }

    // If page found, move it to back
    if (current) {
        // Remove from current position
        if (prev) {
            prev->next = current->next;
        } else {
            queue->head = current->next;
        }
        
        // Move to back
        current->next = NULL;
        queue->tail->next = current;
        queue->tail = current;
    } else {
        // Page not found, add it
        return add_page_to_queue(page_number, queue);
    }
    
    return true;
}