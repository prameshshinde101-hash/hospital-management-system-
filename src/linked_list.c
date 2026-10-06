/* ============================================================
 *  linked_list.c — Singly Linked List for Activity Log
 *  Smart Hospital Management System
 *
 *  This module satisfies the "Linked List" technical requirement.
 *  It maintains an in-memory singly linked list of log entries,
 *  supporting: insert at tail, display forward, display reverse
 *  (recursive), keyword search, delete head, and free all.
 * ============================================================ */

#include "../include/linked_list.h"
#include <stdlib.h>

/* ── Global instance ────────────────────────────────────────── */
LogList activityLogList = { NULL, NULL, 0 };

/* ── Internal sequence counter ──────────────────────────────── */
static int seqCounter = 1;

/* ─────────────────────────────────────────────────────────────
 *  ll_init — initialise an empty list
 * ───────────────────────────────────────────────────────────── */
void ll_init(LogList *list) {
    list->head  = NULL;
    list->tail  = NULL;
    list->count = 0;
}

/* ─────────────────────────────────────────────────────────────
 *  ll_insert — allocate a new node and append at TAIL
 *              O(1) because we maintain a tail pointer
 * ───────────────────────────────────────────────────────────── */
void ll_insert(LogList *list, const char *entry) {
    LogNode *node = (LogNode *)malloc(sizeof(LogNode));
    if (!node) {
        fprintf(stderr, RED "  [LL] malloc failed — log entry dropped.\n" RESET);
        return;
    }

    strncpy(node->entry, entry, LOG_ENTRY_LEN - 1);
    node->entry[LOG_ENTRY_LEN - 1] = '\0';
    getCurrentDateTime(node->timestamp, sizeof(node->timestamp));
    node->seqNo = seqCounter++;
    node->next  = NULL;

    if (list->tail == NULL) {
        /* List is empty — first node */
        list->head = node;
        list->tail = node;
    } else {
        list->tail->next = node;
        list->tail       = node;
    }
    list->count++;

    /* Keep list bounded to last 200 entries to avoid memory growth */
    if (list->count > 200) ll_deleteHead(list);
}

/* ─────────────────────────────────────────────────────────────
 *  ll_display — traverse forward and print all nodes
 * ───────────────────────────────────────────────────────────── */
void ll_display(LogList *list) {
    if (list->head == NULL) {
        printf(YELLOW "  (Log list is empty)\n" RESET);
        return;
    }

    printf(CYAN "  %-5s %-22s %s\n" RESET, "Seq", "Timestamp", "Activity");
    printLine('-', 72);

    LogNode *cur = list->head;
    int shown = 0;
    while (cur != NULL) {
        printf("  %-5d %-22s %s\n", cur->seqNo, cur->timestamp, cur->entry);
        cur = cur->next;
        shown++;
    }
    printLine('-', 72);
    printf(YELLOW "  %d log entries in linked list.\n" RESET, shown);
}

/* ─────────────────────────────────────────────────────────────
 *  ll_displayReverse — recursive traversal, prints NEWEST first
 * ───────────────────────────────────────────────────────────── */
void ll_displayReverse(LogNode *node) {
    if (node == NULL) return;
    ll_displayReverse(node->next);             /* recurse to end   */
    printf("  %-5d %-22s %s\n",               /* print on unwind  */
           node->seqNo, node->timestamp, node->entry);
}

/* ─────────────────────────────────────────────────────────────
 *  ll_search — linear search for first node containing keyword
 *              Returns pointer to node or NULL if not found
 * ───────────────────────────────────────────────────────────── */
LogNode *ll_search(LogList *list, const char *keyword) {
    LogNode *cur = list->head;
    while (cur != NULL) {
        if (strstr(cur->entry, keyword) != NULL) return cur;
        cur = cur->next;
    }
    return NULL;
}

/* ─────────────────────────────────────────────────────────────
 *  ll_deleteHead — remove and free the oldest (head) node
 * ───────────────────────────────────────────────────────────── */
void ll_deleteHead(LogList *list) {
    if (list->head == NULL) return;
    LogNode *old = list->head;
    list->head   = list->head->next;
    if (list->head == NULL) list->tail = NULL;   /* list now empty */
    free(old);
    list->count--;
}

/* ─────────────────────────────────────────────────────────────
 *  ll_freeAll — release every node (call on program exit)
 * ───────────────────────────────────────────────────────────── */
void ll_freeAll(LogList *list) {
    while (list->head != NULL) ll_deleteHead(list);
}

/* ─────────────────────────────────────────────────────────────
 *  ll_count — return number of nodes
 * ───────────────────────────────────────────────────────────── */
int ll_count(LogList *list) {
    return list->count;
}

/* ─────────────────────────────────────────────────────────────
 *  ll_saveToFile — write all entries to the log file (append)
 * ───────────────────────────────────────────────────────────── */
void ll_saveToFile(LogList *list) {
    FILE *f = fopen(LOG_FILE, "a");
    if (!f) return;
    LogNode *cur = list->head;
    while (cur != NULL) {
        fprintf(f, "[%s] %s\n", cur->timestamp, cur->entry);
        cur = cur->next;
    }
    fclose(f);
}

/* ─────────────────────────────────────────────────────────────
 *  ll_loadFromFile — read last N lines of log into linked list
 * ───────────────────────────────────────────────────────────── */
void ll_loadFromFile(LogList *list) {
    FILE *f = fopen(LOG_FILE, "r");
    if (!f) return;

    /* Collect all lines */
    char lines[500][LOG_ENTRY_LEN];
    int  total = 0;
    while (total < 500 && fgets(lines[total], LOG_ENTRY_LEN, f))
        total++;
    fclose(f);

    /* Insert last 100 lines into the linked list */
    int start = (total > 100) ? total - 100 : 0;
    for (int i = start; i < total; i++) {
        lines[i][strcspn(lines[i], "\n")] = '\0';
        if (strlen(lines[i]) > 0)
            ll_insert(list, lines[i]);
    }
}

/* ─────────────────────────────────────────────────────────────
 *  logListMenu — interactive linked-list demo / log viewer
 * ───────────────────────────────────────────────────────────── */
void logListMenu(void) {
    int choice;
    do {
        printHeader("ACTIVITY LOG (LINKED LIST)");
        printf(CYAN "  Nodes in list: %d\n\n" RESET, activityLogList.count);
        printf(WHITE "  1.  Display All Logs (Forward traversal)\n");
        printf("  2.  Display Logs (Reverse / newest first)\n");
        printf("  3.  Search Logs by Keyword\n");
        printf("  4.  Add Manual Log Entry\n");
        printf("  5.  Delete Oldest Log Entry (Remove Head)\n");
        printf("  6.  Show Linked List Node Count\n");
        printf(YELLOW "  0.  Back\n" RESET);
        choice = getIntInput("Select option", 0, 6);

        switch (choice) {
            case 1:
                printHeader("LOG — FORWARD TRAVERSAL");
                ll_display(&activityLogList);
                pauseScreen();
                break;

            case 2:
                printHeader("LOG — REVERSE TRAVERSAL (Recursive)");
                printf(CYAN "  %-5s %-22s %s\n" RESET, "Seq", "Timestamp", "Activity");
                printLine('-', 72);
                ll_displayReverse(activityLogList.head);
                printLine('-', 72);
                printf(YELLOW "  (Printed newest to oldest using recursion)\n" RESET);
                pauseScreen();
                break;

            case 3: {
                printHeader("SEARCH LOG ENTRIES");
                char keyword[80];
                getStrInput("Enter keyword to search", keyword, sizeof(keyword));
                LogNode *result = ll_search(&activityLogList, keyword);
                if (result) {
                    printf(GREEN "\n  ✔  First match found:\n" RESET);
                    printf(CYAN  "     [Seq %d] [%s] %s\n" RESET,
                           result->seqNo, result->timestamp, result->entry);
                    /* Count total matches */
                    int count = 0;
                    LogNode *cur = activityLogList.head;
                    while (cur) {
                        if (strstr(cur->entry, keyword)) {
                            printf("     [Seq %d] %s\n", cur->seqNo, cur->entry);
                            count++;
                        }
                        cur = cur->next;
                    }
                    printf(YELLOW "\n  Total matches: %d\n" RESET, count);
                } else {
                    printf(RED "  ✘  No entries found containing \"%s\".\n" RESET, keyword);
                }
                pauseScreen();
                break;
            }

            case 4: {
                char entry[LOG_ENTRY_LEN];
                getStrInput("Enter log message", entry, LOG_ENTRY_LEN);
                ll_insert(&activityLogList, entry);
                logActivity(entry);  /* also write to file */
                printf(GREEN "  ✔  Entry added to linked list and log file.\n" RESET);
                pauseScreen();
                break;
            }

            case 5:
                if (activityLogList.head == NULL) {
                    printf(YELLOW "  List is already empty.\n" RESET);
                } else {
                    printf(RED "  Removing: %s\n" RESET, activityLogList.head->entry);
                    ll_deleteHead(&activityLogList);
                    printf(GREEN "  ✔  Head node deleted. Remaining: %d\n" RESET,
                           activityLogList.count);
                }
                pauseScreen();
                break;

            case 6:
                printf(CYAN "\n  Total nodes in linked list: " YELLOW "%d\n\n" RESET,
                       ll_count(&activityLogList));
                pauseScreen();
                break;
        }
    } while (choice != 0);
}
