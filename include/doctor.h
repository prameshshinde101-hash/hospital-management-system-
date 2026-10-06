#ifndef DOCTOR_H
#define DOCTOR_H

/* ============================================================
 *  doctor.h — Doctor data structure and module prototypes
 *  Smart Hospital Management System
 * ============================================================ */

#include "utility.h"

/* ── Doctor record ──────────────────────────────────────────── */
typedef struct {
    int  doctorID;
    char name[MAX_NAME];
    char specialization[MAX_SPEC];
    char phone[MAX_PHONE];
    int  experience;       /* years */
    int  isAvailable;      /* 1 = available */
    int  patientCount;     /* current assigned patients */
} Doctor;

/* ── Module interface ───────────────────────────────────────── */
void doctorMenu(void);
void addDoctor(void);
void updateDoctor(void);
void viewDoctors(void);
void searchDoctor(void);
void assignDoctorToPatient(void);
void sortDoctorsBySpecialization(void);
void generateDoctorReport(void);

/* ── File I/O ───────────────────────────────────────────────── */
void loadDoctors(void);
void saveDoctors(void);

/* ── Helpers ────────────────────────────────────────────────── */
int  doctorExists(int id);
int  getDoctorIndex(int id);
int  getNextDoctorID(void);

/* ── Global doctor array ────────────────────────────────────── */
extern Doctor doctors[MAX_DOCTORS];
extern int    doctorCount;

#endif /* DOCTOR_H */
