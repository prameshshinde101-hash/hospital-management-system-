#ifndef LINKED_LIST_H
#define LINKED_LIST_H

/* ============================================================
 *  linked_list.h — Singly Linked List for Activity Log
 *
 *  Demonstrates the Linked List data structure required by the
 *  project specification. Used to maintain an in-memory linked
 *  list of recent activity log entries with insert, display,
 *  search, delete, and reverse operations.
 * ============================================================ */

#include "utility.h"

#define LOG_ENTRY_LEN  256

/* ── Node structure ─────────────────────────────────────────── */
typedef struct LogNode {
    char            entry[LOG_ENTRY_LEN];   /* log message      */
    char            timestamp[30];          /* date-time string */
    int             seqNo;                  /* sequence number  */
    struct LogNode *next;                   /* pointer to next  */
} LogNode;

/* ── Linked list head/tail ──────────────────────────────────── */
typedef struct {
    LogNode *head;
    LogNode *tail;
    int      count;
} LogList;

/* ── Operations ─────────────────────────────────────────────── */
void     ll_init(LogList *list);
void     ll_insert(LogList *list, const char *entry);   /* insert at tail */
void     ll_display(LogList *list);                     /* print all      */
void     ll_displayReverse(LogNode *node);              /* recursive rev  */
LogNode *ll_search(LogList *list, const char *keyword); /* first match    */
void     ll_deleteHead(LogList *list);                  /* remove oldest  */
void     ll_freeAll(LogList *list);                     /* free all nodes */
int      ll_count(LogList *list);
void     ll_saveToFile(LogList *list);                  /* persist to log */
void     ll_loadFromFile(LogList *list);                /* load from log  */

/* ── Menu ───────────────────────────────────────────────────── */
void     logListMenu(void);

/* ── Global log list ────────────────────────────────────────── */
extern LogList activityLogList;

#endif /* LINKED_LIST_H */
