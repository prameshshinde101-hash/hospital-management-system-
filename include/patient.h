#ifndef PATIENT_H
#define PATIENT_H

/* ============================================================
 *  patient.h — Patient data structure and module prototypes
 *  Smart Hospital Management System
 * ============================================================ */

#include "utility.h"

/* ── Patient record ─────────────────────────────────────────── */
typedef struct {
    int   patientID;
    char  name[MAX_NAME];
    int   age;
    char  gender;          /* 'M' / 'F' / 'O' */
    char  bloodGroup[MAX_BLOOD];
    char  disease[MAX_DISEASE];
    char  phone[MAX_PHONE];
    char  address[MAX_ADDR];
    int   assignedDoctorID;
    int   bedNumber;       /* 0 = no bed */
    char  admitDate[20];
    int   isActive;        /* 1 = admitted, 0 = discharged */
} Patient;

/* ── Module interface ───────────────────────────────────────── */
void patientMenu(void);
void addPatient(void);
void updatePatient(void);
void deletePatient(void);
void searchPatient(void);
void displayAllPatients(void);
void sortPatientsByName(void);
void sortPatientsByAge(void);
void generatePatientReport(void);

/* ── File I/O ───────────────────────────────────────────────── */
void loadPatients(void);
void savePatients(void);

/* ── Helpers (used by other modules) ───────────────────────── */
int  patientExists(int id);
int  getPatientIndex(int id);
int  getNextPatientID(void);

/* ── Global patient array ───────────────────────────────────── */
extern Patient patients[MAX_PATIENTS];
extern int     patientCount;

#endif /* PATIENT_H */
