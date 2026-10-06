#ifndef EMERGENCY_H
#define EMERGENCY_H

/* ============================================================
 *  emergency.h — Emergency priority queue structures/prototypes
 *  Smart Hospital Management System
 * ============================================================ */

#include "utility.h"

/* ── Priority levels (lower number = higher priority) ───────── */
typedef enum {
    PRIORITY_CRITICAL = 1,
    PRIORITY_HIGH     = 2,
    PRIORITY_MEDIUM   = 3,
    PRIORITY_LOW      = 4
} EmergencyPriority;

/* ── Emergency patient record ───────────────────────────────── */
typedef struct {
    int               emergID;
    int               patientID;
    char              patientName[MAX_NAME];
    int               age;
    char              condition[MAX_DISEASE];
    EmergencyPriority priority;
    char              arrivalTime[20];
    int               isHandled;
} EmergencyPatient;

/* ── Max-heap priority queue node ───────────────────────────── */
typedef struct {
    EmergencyPatient data[MAX_EMERG_QUEUE];
    int              size;
} EmergencyQueue;

/* ── Module interface ───────────────────────────────────────── */
void  emergencyMenu(void);
void  registerEmergency(void);
void  handleNextEmergency(void);
void  displayEmergencyQueue(void);
void  visualizeQueue(void);
void  generateEmergencyReport(void);

/* ── Priority Queue operations ──────────────────────────────── */
void  pqInsert(EmergencyQueue *pq, EmergencyPatient ep);
EmergencyPatient pqExtractMin(EmergencyQueue *pq);
void  pqHeapifyUp(EmergencyQueue *pq, int i);
void  pqHeapifyDown(EmergencyQueue *pq, int i);
int   pqIsEmpty(EmergencyQueue *pq);

/* ── File I/O ───────────────────────────────────────────────── */
void  loadEmergency(void);
void  saveEmergency(void);

/* ── Globals ────────────────────────────────────────────────── */
extern EmergencyQueue emergQueue;
extern int            emergIDCounter;

#endif /* EMERGENCY_H */
