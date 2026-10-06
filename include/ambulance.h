#ifndef AMBULANCE_H
#define AMBULANCE_H

/* ============================================================
 *  ambulance.h — Ambulance management structures/prototypes
 *  Smart Hospital Management System
 * ============================================================ */

#include "utility.h"

typedef enum { AMB_AVAILABLE = 0, AMB_DISPATCHED, AMB_MAINTENANCE } AmbStatus;

/* ── Ambulance record ───────────────────────────────────────── */
typedef struct {
    int       ambID;
    char      vehicleNumber[20];
    char      driverName[MAX_NAME];
    char      driverPhone[MAX_PHONE];
    AmbStatus status;
    int       assignedPatientID;  /* 0 = none */
    char      dispatchTime[20];
    int       totalTrips;
} Ambulance;

/* ── Module interface ───────────────────────────────────────── */
void ambulanceMenu(void);
void addAmbulance(void);
void viewAmbulances(void);
void allocateAmbulance(void);
void releaseAmbulance(void);
void ambulanceStatus(void);

/* ── File I/O ───────────────────────────────────────────────── */
void loadAmbulances(void);
void saveAmbulances(void);

/* ── Helpers ────────────────────────────────────────────────── */
int  getNextAmbID(void);
int  getAvailableAmbulance(void);

/* ── Globals ────────────────────────────────────────────────── */
extern Ambulance ambulances[MAX_AMBULANCES];
extern int       ambCount;

#endif /* AMBULANCE_H */
