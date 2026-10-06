/* ============================================================
 *  appointment.c — Appointment management module
 *  Smart Hospital Management System
 * ============================================================ */

#include "../include/appointment.h"
#include "../include/patient.h"
#include "../include/doctor.h"

/* ── Globals ────────────────────────────────────────────────── */
Appointment appointments[MAX_APPTS];
int         apptCount = 0;

/* ── Token counter ──────────────────────────────────────────── */
int getNextToken(void) {
    int maxToken = 1000;
    for (int i = 0; i < apptCount; i++)
        if (appointments[i].tokenNumber > maxToken)
            maxToken = appointments[i].tokenNumber;
    return maxToken + 1;
}

/* ── File I/O ───────────────────────────────────────────────── */
void loadAppointments(void) {
    FILE *f = fopen(APPT_FILE, "rb");
    if (!f) return;
    fread(&apptCount, sizeof(int), 1, f);
    fread(appointments, sizeof(Appointment), apptCount, f);
    fclose(f);
}

void saveAppointments(void) {
    FILE *f = fopen(APPT_FILE, "wb");
    if (!f) { printf(RED "  Error saving appointments.\n" RESET); return; }
    fwrite(&apptCount, sizeof(int), 1, f);
    fwrite(appointments, sizeof(Appointment), apptCount, f);
    fclose(f);
}

/* ── Status label helper ────────────────────────────────────── */
static const char *statusLabel(ApptStatus s) {
    switch (s) {
        case APPT_SCHEDULED:  return GREEN  "Scheduled " RESET;
        case APPT_COMPLETED:  return CYAN   "Completed " RESET;
        case APPT_CANCELLED:  return RED    "Cancelled " RESET;
        default:              return WHITE  "Unknown   " RESET;
    }
}

/* ── Print appointment receipt ──────────────────────────────── */
static void printApptReceipt(Appointment *a) {
    int pIdx = getPatientIndex(a->patientID);
    int dIdx = getDoctorIndex(a->doctorID);

    printf(CYAN "\n  ╔══════════════════════════════════════════════════╗\n");
    printf("  ║        APPOINTMENT CONFIRMATION TOKEN            ║\n");
    printf("  ╠══════════════════════════════════════════════════╣\n");
    printf("  ║" YELLOW "  Token Number : " WHITE "%-5d" CYAN "                            ║\n", a->tokenNumber);
    printf("  ║" YELLOW "  Patient      : " WHITE "%-30s" CYAN "  ║\n",
           pIdx >= 0 ? patients[pIdx].name : "Unknown");
    printf("  ║" YELLOW "  Doctor       : " WHITE "%-30s" CYAN "  ║\n",
           dIdx >= 0 ? doctors[dIdx].name : "Unknown");
    printf("  ║" YELLOW "  Date         : " WHITE "%-30s" CYAN "  ║\n", a->date);
    printf("  ║" YELLOW "  Time Slot    : " WHITE "%-30s" CYAN "  ║\n", a->timeSlot);
    printf("  ║" YELLOW "  Status       : " WHITE "%-30s" CYAN "  ║\n", "Scheduled");
    if (strlen(a->notes) > 0)
        printf("  ║" YELLOW "  Notes        : " WHITE "%-30s" CYAN "  ║\n", a->notes);
    printf("  ╚══════════════════════════════════════════════════╝\n" RESET);
}

/* ── Book appointment ───────────────────────────────────────── */
void bookAppointment(void) {
    printHeader("BOOK APPOINTMENT");

    if (apptCount >= MAX_APPTS) {
        printf(RED "  ✘  Appointment schedule full!\n" RESET);
        pauseScreen();
        return;
    }

    Appointment a;
    memset(&a, 0, sizeof(Appointment));
    a.tokenNumber = getNextToken();
    a.status      = APPT_SCHEDULED;

    /* Validate patient */
    a.patientID = getIntInput("Enter Patient ID", 1, 99999);
    if (!patientExists(a.patientID)) {
        printf(RED "  ✘  Patient not found or inactive.\n" RESET);
        pauseScreen();
        return;
    }

    /* Validate doctor */
    a.doctorID = getIntInput("Enter Doctor ID", 1, 99999);
    if (!doctorExists(a.doctorID)) {
        printf(RED "  ✘  Doctor not found.\n" RESET);
        pauseScreen();
        return;
    }

    /* Check doctor availability */
    int dIdx = getDoctorIndex(a.doctorID);
    if (!doctors[dIdx].isAvailable) {
        printf(YELLOW "  ⚠  Doctor is currently unavailable. Proceed anyway? \n" RESET);
        if (!confirmAction("Book appointment")) {
            pauseScreen();
            return;
        }
    }

    getStrInput("Appointment Date (DD-MM-YYYY)", a.date, sizeof(a.date));

    /* Time slot selection */
    printf(WHITE "\n  Select Time Slot:\n" RESET);
    printf("  1. 09:00 AM   2. 10:00 AM   3. 11:00 AM\n");
    printf("  4. 12:00 PM   5. 02:00 PM   6. 03:00 PM\n");
    printf("  7. 04:00 PM   8. 05:00 PM\n");
    const char *slots[] = {"09:00 AM","10:00 AM","11:00 AM","12:00 PM",
                           "02:00 PM","03:00 PM","04:00 PM","05:00 PM"};
    int slotChoice = getIntInput("Select slot", 1, 8);
    strncpy(a.timeSlot, slots[slotChoice - 1], sizeof(a.timeSlot) - 1);

    getStrInput("Notes (optional)", a.notes, sizeof(a.notes));

    appointments[apptCount++] = a;
    saveAppointments();

    printApptReceipt(&a);
    printf(GREEN "\n  ✔  Appointment booked. Token: %d\n" RESET, a.tokenNumber);
    logActivity("Appointment booked");
    pauseScreen();
}

/* ── Cancel appointment ─────────────────────────────────────── */
void cancelAppointment(void) {
    printHeader("CANCEL APPOINTMENT");

    int token = getIntInput("Enter Token Number", 1, 99999);
    int found = -1;
    for (int i = 0; i < apptCount; i++) {
        if (appointments[i].tokenNumber == token) {
            found = i;
            break;
        }
    }

    if (found == -1) {
        printf(RED "  ✘  Token not found.\n" RESET);
        pauseScreen();
        return;
    }

    if (appointments[found].status == APPT_CANCELLED) {
        printf(YELLOW "  ⚠  Appointment already cancelled.\n" RESET);
        pauseScreen();
        return;
    }

    printApptReceipt(&appointments[found]);
    if (!confirmAction("Confirm cancellation")) {
        pauseScreen();
        return;
    }

    appointments[found].status = APPT_CANCELLED;
    saveAppointments();
    printf(GREEN "\n  ✔  Appointment cancelled. Token: %d\n" RESET, token);
    logActivity("Appointment cancelled");
    pauseScreen();
}

/* ── Display appointments ───────────────────────────────────── */
void displayAppointments(void) {
    printHeader("ALL APPOINTMENTS");

    if (apptCount == 0) {
        printf(YELLOW "  No appointments booked.\n" RESET);
        pauseScreen();
        return;
    }

    printf("  Filter: 1.All  2.Scheduled  3.Completed  4.Cancelled\n");
    int filter = getIntInput("Filter", 1, 4);

    printf(CYAN "\n  %-7s %-8s %-8s %-12s %-10s %s\n" RESET,
           "Token", "Pat.ID", "Doc.ID", "Date", "Time", "Status");
    printLine('-', 72);

    int shown = 0;
    for (int i = 0; i < apptCount; i++) {
        Appointment *a = &appointments[i];
        int show = (filter == 1) ||
                   (filter == 2 && a->status == APPT_SCHEDULED) ||
                   (filter == 3 && a->status == APPT_COMPLETED) ||
                   (filter == 4 && a->status == APPT_CANCELLED);
        if (!show) continue;

        /* patient name lookup */
        int pIdx = getPatientIndex(a->patientID);
        char pName[22] = "Unknown";
        if (pIdx >= 0) strncpy(pName, patients[pIdx].name, 21);

        printf("  %-7d %-8d %-8d %-12s %-10s %s\n",
               a->tokenNumber, a->patientID, a->doctorID,
               a->date, a->timeSlot, statusLabel(a->status));
        shown++;
    }
    printLine('-', 72);
    printf(YELLOW "  Showing %d appointment(s).\n" RESET, shown);
    pauseScreen();
}

/* ── Mark appointment as completed ─────────────────────────── */
static void completeAppointment(void) {
    printHeader("COMPLETE APPOINTMENT");
    int token = getIntInput("Enter Token Number", 1, 99999);
    for (int i = 0; i < apptCount; i++) {
        if (appointments[i].tokenNumber == token) {
            if (appointments[i].status != APPT_SCHEDULED) {
                printf(YELLOW "  ⚠  Appointment is not in Scheduled state.\n" RESET);
            } else {
                appointments[i].status = APPT_COMPLETED;
                saveAppointments();
                printf(GREEN "  ✔  Appointment marked completed.\n" RESET);
                logActivity("Appointment completed");
            }
            pauseScreen();
            return;
        }
    }
    printf(RED "  ✘  Token not found.\n" RESET);
    pauseScreen();
}

/* ── Appointment report ─────────────────────────────────────── */
void generateAppointmentReport(void) {
    printHeader("APPOINTMENT REPORT");
    char dt[30];
    getCurrentDateTime(dt, sizeof(dt));

    FILE *f = fopen("data/appointment_report.txt", "w");
    if (f) {
        fprintf(f, "====== APPOINTMENT REPORT ======\nGenerated: %s\n\n", dt);
        int sched = 0, comp = 0, canc = 0;
        for (int i = 0; i < apptCount; i++) {
            Appointment *a = &appointments[i];
            const char *s = (a->status == APPT_SCHEDULED)  ? "Scheduled"  :
                            (a->status == APPT_COMPLETED)  ? "Completed"  : "Cancelled";
            fprintf(f, "Token: %-5d PatID: %-5d DocID: %-5d Date: %-12s Time: %-10s %s\n",
                    a->tokenNumber, a->patientID, a->doctorID,
                    a->date, a->timeSlot, s);
            if (a->status == APPT_SCHEDULED) sched++;
            else if (a->status == APPT_COMPLETED) comp++;
            else canc++;
        }
        fprintf(f, "\nTotal: %d | Scheduled: %d | Completed: %d | Cancelled: %d\n",
                apptCount, sched, comp, canc);
        fclose(f);
        printf(GREEN "  ✔  Report saved to data/appointment_report.txt\n" RESET);
    }

    displayAppointments();
}

/* ── Appointment menu ───────────────────────────────────────── */
void appointmentMenu(void) {
    int choice;
    do {
        printHeader("APPOINTMENT MANAGEMENT");
        printf(WHITE "  1.  Book Appointment\n");
        printf("  2.  Cancel Appointment\n");
        printf("  3.  Mark Appointment Completed\n");
        printf("  4.  Display Appointments\n");
        printf("  5.  Generate Appointment Report\n");
        printf(YELLOW "  0.  Back to Main Menu\n" RESET);
        choice = getIntInput("Select option", 0, 5);
        switch (choice) {
            case 1: bookAppointment();           break;
            case 2: cancelAppointment();         break;
            case 3: completeAppointment();       break;
            case 4: displayAppointments();       break;
            case 5: generateAppointmentReport(); break;
        }
    } while (choice != 0);
}
