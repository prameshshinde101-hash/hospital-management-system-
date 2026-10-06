#ifndef APPOINTMENT_H
#define APPOINTMENT_H

/* ============================================================
 *  appointment.h — Appointment data structure and prototypes
 *  Smart Hospital Management System
 * ============================================================ */

#include "utility.h"

typedef enum { APPT_SCHEDULED = 0, APPT_COMPLETED, APPT_CANCELLED } ApptStatus;

/* ── Appointment record ─────────────────────────────────────── */
typedef struct {
    int        tokenNumber;
    int        patientID;
    int        doctorID;
    char       date[20];
    char       timeSlot[10];
    ApptStatus status;
    char       notes[120];
} Appointment;

/* ── Module interface ───────────────────────────────────────── */
void appointmentMenu(void);
void bookAppointment(void);
void cancelAppointment(void);
void displayAppointments(void);
void generateAppointmentReport(void);

/* ── File I/O ───────────────────────────────────────────────── */
void loadAppointments(void);
void saveAppointments(void);

/* ── Helpers ────────────────────────────────────────────────── */
int  getNextToken(void);

/* ── Globals ────────────────────────────────────────────────── */
extern Appointment appointments[MAX_APPTS];
extern int         apptCount;

#endif /* APPOINTMENT_H */
