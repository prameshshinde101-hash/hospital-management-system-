#ifndef BED_H
#define BED_H

/* ============================================================
 *  bed.h — Bed management structures and prototypes
 *  Smart Hospital Management System
 * ============================================================ */

#include "utility.h"

typedef enum { BED_AVAILABLE = 0, BED_OCCUPIED } BedStatus;
typedef enum { BED_ICU = 0, BED_GENERAL } BedType;

/* ── Bed record ─────────────────────────────────────────────── */
typedef struct {
    int       bedNumber;
    BedType   type;
    BedStatus status;
    int       assignedPatientID;   /* 0 = free */
    char      allocatedDate[20];
} Bed;

/* ── Module interface ───────────────────────────────────────── */
void bedMenu(void);
void initBeds(void);
void displayBeds(void);
int  allocateBed(int patientID, BedType type);
void releaseBed(int bedNumber);
void bedStatus(void);
int  getAvailableBed(BedType type);

/* ── File I/O ───────────────────────────────────────────────── */
void loadBeds(void);
void saveBeds(void);

/* ── Globals ────────────────────────────────────────────────── */
extern Bed icuBeds[MAX_ICU_BEDS];
extern Bed genBeds[MAX_GEN_BEDS];

#endif /* BED_H */
