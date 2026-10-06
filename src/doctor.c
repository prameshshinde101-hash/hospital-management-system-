/* ============================================================
 *  doctor.c — Doctor management module
 *  Smart Hospital Management System
 * ============================================================ */

#include "../include/doctor.h"
#include "../include/patient.h"

/* ── Globals ────────────────────────────────────────────────── */
Doctor doctors[MAX_DOCTORS];
int    doctorCount = 0;

/* ── File I/O ───────────────────────────────────────────────── */
void loadDoctors(void) {
    FILE *f = fopen(DOCTOR_FILE, "rb");
    if (!f) return;
    fread(&doctorCount, sizeof(int), 1, f);
    fread(doctors, sizeof(Doctor), doctorCount, f);
    fclose(f);
}

void saveDoctors(void) {
    FILE *f = fopen(DOCTOR_FILE, "wb");
    if (!f) { printf(RED "  Error saving doctors.\n" RESET); return; }
    fwrite(&doctorCount, sizeof(int), 1, f);
    fwrite(doctors, sizeof(Doctor), doctorCount, f);
    fclose(f);
}

/* ── ID helpers ─────────────────────────────────────────────── */
int getNextDoctorID(void) {
    int maxID = 200;
    for (int i = 0; i < doctorCount; i++)
        if (doctors[i].doctorID > maxID)
            maxID = doctors[i].doctorID;
    return maxID + 1;
}

int doctorExists(int id) {
    for (int i = 0; i < doctorCount; i++)
        if (doctors[i].doctorID == id)
            return 1;
    return 0;
}

int getDoctorIndex(int id) {
    for (int i = 0; i < doctorCount; i++)
        if (doctors[i].doctorID == id)
            return i;
    return -1;
}

/* ── Display single doctor card ─────────────────────────────── */
static void displayDoctorCard(Doctor *d) {
    printf(MAGENTA "  ┌─────────────────────────────────────────────────┐\n" RESET);
    printf(MAGENTA "  │" YELLOW " Doctor ID      : " WHITE "%-5d" MAGENTA "                         │\n" RESET, d->doctorID);
    printf(MAGENTA "  │" YELLOW " Name           : " WHITE "%-30s" MAGENTA " │\n" RESET, d->name);
    printf(MAGENTA "  │" YELLOW " Specialization : " WHITE "%-30s" MAGENTA " │\n" RESET, d->specialization);
    printf(MAGENTA "  │" YELLOW " Phone          : " WHITE "%-30s" MAGENTA " │\n" RESET, d->phone);
    printf(MAGENTA "  │" YELLOW " Experience     : " WHITE "%d years%-24s" MAGENTA " │\n" RESET, d->experience, "");
    printf(MAGENTA "  │" YELLOW " Patients       : " WHITE "%-5d" MAGENTA "                         │\n" RESET, d->patientCount);
    printf(MAGENTA "  │" YELLOW " Availability   : " RESET);
    if (d->isAvailable) printf(GREEN  "Available   " MAGENTA "                         │\n" RESET);
    else                printf(RED    "Unavailable " MAGENTA "                         │\n" RESET);
    printf(MAGENTA "  └─────────────────────────────────────────────────┘\n\n" RESET);
}

/* ── Add doctor ─────────────────────────────────────────────── */
void addDoctor(void) {
    printHeader("ADD NEW DOCTOR");

    if (doctorCount >= MAX_DOCTORS) {
        printf(RED "  ✘  Doctor capacity full!\n" RESET);
        pauseScreen();
        return;
    }

    Doctor d;
    memset(&d, 0, sizeof(Doctor));
    d.doctorID    = getNextDoctorID();
    d.isAvailable = 1;
    d.patientCount = 0;

    printf(GREEN "  Auto-assigned Doctor ID: %d\n\n" RESET, d.doctorID);

    getStrInput("Full Name", d.name, MAX_NAME);
    if (strlen(d.name) == 0) {
        printf(RED "  Name cannot be empty.\n" RESET);
        pauseScreen();
        return;
    }

    getStrInput("Specialization", d.specialization, MAX_SPEC);
    getStrInput("Phone Number",   d.phone, MAX_PHONE);
    d.experience = getIntInput("Years of Experience (0-60)", 0, 60);

    char avBuf[4];
    getStrInput("Available Now? (Y/N)", avBuf, 4);
    d.isAvailable = (tolower(avBuf[0]) == 'y') ? 1 : 0;

    doctors[doctorCount++] = d;
    saveDoctors();

    printf(GREEN "\n  ✔  Doctor added successfully! ID: %d\n" RESET, d.doctorID);
    logActivity("Doctor added");
    pauseScreen();
}

/* ── Update doctor ──────────────────────────────────────────── */
void updateDoctor(void) {
    printHeader("UPDATE DOCTOR DETAILS");

    int id = getIntInput("Enter Doctor ID to update", 1, 99999);
    int idx = getDoctorIndex(id);
    if (idx == -1) {
        printf(RED "  ✘  Doctor not found.\n" RESET);
        pauseScreen();
        return;
    }

    Doctor *d = &doctors[idx];
    printf(YELLOW "\n  Updating: Dr. %s (leave blank to keep current)\n\n" RESET, d->name);

    char buf[MAX_NAME];

    getStrInput("New Name", buf, MAX_NAME);
    if (strlen(buf) > 0) strncpy(d->name, buf, MAX_NAME - 1);

    getStrInput("New Specialization", buf, MAX_SPEC);
    if (strlen(buf) > 0) strncpy(d->specialization, buf, MAX_SPEC - 1);

    getStrInput("New Phone", buf, MAX_PHONE);
    if (strlen(buf) > 0) strncpy(d->phone, buf, MAX_PHONE - 1);

    printf(WHITE "  New Experience in years (0 to skip): " RESET);
    char expBuf[10];
    fgets(expBuf, sizeof(expBuf), stdin);
    int newExp;
    if (sscanf(expBuf, "%d", &newExp) == 1 && newExp > 0) d->experience = newExp;

    saveDoctors();
    printf(GREEN "\n  ✔  Doctor record updated successfully.\n" RESET);
    logActivity("Doctor record updated");
    pauseScreen();
}

/* ── View all doctors ───────────────────────────────────────── */
void viewDoctors(void) {
    printHeader("ALL DOCTORS");

    if (doctorCount == 0) {
        printf(YELLOW "  No doctors registered.\n" RESET);
        pauseScreen();
        return;
    }

    printf(CYAN "  %-6s %-22s %-22s %-5s %-9s %-8s\n" RESET,
           "ID", "Name", "Specialization", "Exp", "Patients", "Status");
    printLine('-', 78);

    for (int i = 0; i < doctorCount; i++) {
        Doctor *d = &doctors[i];
        const char *avail = d->isAvailable ? GREEN "Available" RESET : RED "Busy     " RESET;
        printf("  %-6d %-22s %-22s %-5d %-8d %s\n",
               d->doctorID, d->name, d->specialization,
               d->experience, d->patientCount, avail);
    }
    printLine('-', 78);
    printf(YELLOW "  Total Doctors: %d\n" RESET, doctorCount);
    pauseScreen();
}

/* ── Search doctor ──────────────────────────────────────────── */
void searchDoctor(void) {
    printHeader("SEARCH DOCTOR");

    printf("  1. Search by ID\n");
    printf("  2. Search by Name\n");
    printf("  3. Search by Specialization\n");
    int choice = getIntInput("Choice", 1, 3);

    int found = 0;

    if (choice == 1) {
        int id = getIntInput("Enter Doctor ID", 1, 99999);
        for (int i = 0; i < doctorCount; i++) {
            if (doctors[i].doctorID == id) {
                displayDoctorCard(&doctors[i]);
                found = 1;
            }
        }
    } else if (choice == 2) {
        char name[MAX_NAME];
        getStrInput("Enter Name (partial OK)", name, MAX_NAME);
        toUpperStr(name);
        for (int i = 0; i < doctorCount; i++) {
            char dName[MAX_NAME];
            strncpy(dName, doctors[i].name, MAX_NAME - 1);
            toUpperStr(dName);
            if (strstr(dName, name)) {
                displayDoctorCard(&doctors[i]);
                found = 1;
            }
        }
    } else {
        char spec[MAX_SPEC];
        getStrInput("Enter Specialization (partial OK)", spec, MAX_SPEC);
        toUpperStr(spec);
        for (int i = 0; i < doctorCount; i++) {
            char dSpec[MAX_SPEC];
            strncpy(dSpec, doctors[i].specialization, MAX_SPEC - 1);
            toUpperStr(dSpec);
            if (strstr(dSpec, spec)) {
                displayDoctorCard(&doctors[i]);
                found = 1;
            }
        }
    }

    if (!found) printf(RED "  ✘  No doctor found.\n" RESET);
    pauseScreen();
}

/* ── Assign doctor to patient ───────────────────────────────── */
void assignDoctorToPatient(void) {
    printHeader("ASSIGN DOCTOR TO PATIENT");

    int pid = getIntInput("Enter Patient ID", 1, 99999);
    if (!patientExists(pid)) {
        printf(RED "  ✘  Patient not found or inactive.\n" RESET);
        pauseScreen();
        return;
    }

    int did = getIntInput("Enter Doctor ID", 1, 99999);
    int dIdx = getDoctorIndex(did);
    if (dIdx == -1) {
        printf(RED "  ✘  Doctor not found.\n" RESET);
        pauseScreen();
        return;
    }

    int pIdx = getPatientIndex(pid);
    /* Decrement old doctor's count if re-assigning */
    if (patients[pIdx].assignedDoctorID > 0) {
        int oldDIdx = getDoctorIndex(patients[pIdx].assignedDoctorID);
        if (oldDIdx != -1 && doctors[oldDIdx].patientCount > 0)
            doctors[oldDIdx].patientCount--;
    }

    patients[pIdx].assignedDoctorID = did;
    doctors[dIdx].patientCount++;

    savePatients();
    saveDoctors();

    printf(GREEN "\n  ✔  Dr. %s assigned to Patient %s.\n" RESET,
           doctors[dIdx].name, patients[pIdx].name);
    logActivity("Doctor assigned to patient");
    pauseScreen();
}

/* ── Sort doctors by specialization (insertion sort) ────────── */
void sortDoctorsBySpecialization(void) {
    for (int i = 1; i < doctorCount; i++) {
        Doctor key = doctors[i];
        int j = i - 1;
        while (j >= 0 && strcasecmp(doctors[j].specialization, key.specialization) > 0) {
            doctors[j + 1] = doctors[j];
            j--;
        }
        doctors[j + 1] = key;
    }
    saveDoctors();
    printf(GREEN "  ✔  Doctors sorted by specialization.\n" RESET);
}

/* ── Doctor report ──────────────────────────────────────────── */
void generateDoctorReport(void) {
    printHeader("DOCTOR REPORT");
    char dt[30];
    getCurrentDateTime(dt, sizeof(dt));

    FILE *f = fopen("data/doctor_report.txt", "w");
    if (f) {
        fprintf(f, "====== DOCTOR REPORT ======\nGenerated: %s\n\n", dt);
        for (int i = 0; i < doctorCount; i++) {
            Doctor *d = &doctors[i];
            fprintf(f, "ID: %d | %s | %s | Exp: %d yrs | Patients: %d | %s\n",
                    d->doctorID, d->name, d->specialization,
                    d->experience, d->patientCount,
                    d->isAvailable ? "Available" : "Unavailable");
        }
        fprintf(f, "\nTotal Doctors: %d\n", doctorCount);
        fclose(f);
        printf(GREEN "  ✔  Report saved to data/doctor_report.txt\n" RESET);
    }

    viewDoctors();
}

/* ── Toggle availability ────────────────────────────────────── */
static void toggleAvailability(void) {
    printHeader("TOGGLE DOCTOR AVAILABILITY");
    int id = getIntInput("Enter Doctor ID", 1, 99999);
    int idx = getDoctorIndex(id);
    if (idx == -1) { printf(RED "  ✘  Doctor not found.\n" RESET); pauseScreen(); return; }
    doctors[idx].isAvailable = !doctors[idx].isAvailable;
    saveDoctors();
    printf(GREEN "  ✔  Dr. %s is now %s.\n" RESET,
           doctors[idx].name, doctors[idx].isAvailable ? "Available" : "Unavailable");
    pauseScreen();
}

/* ── Doctor menu ────────────────────────────────────────────── */
void doctorMenu(void) {
    int choice;
    do {
        printHeader("DOCTOR MANAGEMENT");
        printf(WHITE "  1.  Add Doctor\n");
        printf("  2.  Update Doctor Details\n");
        printf("  3.  View All Doctors\n");
        printf("  4.  Search Doctor\n");
        printf("  5.  Assign Doctor to Patient\n");
        printf("  6.  Sort Doctors by Specialization\n");
        printf("  7.  Toggle Doctor Availability\n");
        printf("  8.  Generate Doctor Report\n");
        printf(YELLOW "  0.  Back to Main Menu\n" RESET);
        choice = getIntInput("Select option", 0, 8);
        switch (choice) {
            case 1: addDoctor();                   break;
            case 2: updateDoctor();                break;
            case 3: viewDoctors();                 break;
            case 4: searchDoctor();                break;
            case 5: assignDoctorToPatient();       break;
            case 6: sortDoctorsBySpecialization(); viewDoctors(); break;
            case 7: toggleAvailability();          break;
            case 8: generateDoctorReport();        break;
        }
    } while (choice != 0);
}
