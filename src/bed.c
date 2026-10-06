/* ============================================================
 *  bed.c — Bed management module (ICU + General)
 *  Smart Hospital Management System
 * ============================================================ */

#include "../include/bed.h"
#include "../include/billing.h"
#include "../include/patient.h"

/* ── Globals ────────────────────────────────────────────────── */
Bed icuBeds[MAX_ICU_BEDS];
Bed genBeds[MAX_GEN_BEDS];

/* ── Initialize beds on first run ───────────────────────────── */
void initBeds(void) {
    for (int i = 0; i < MAX_ICU_BEDS; i++) {
        icuBeds[i].bedNumber          = 1000 + i + 1;  /* ICU: 1001–1020 */
        icuBeds[i].type               = BED_ICU;
        icuBeds[i].status             = BED_AVAILABLE;
        icuBeds[i].assignedPatientID  = 0;
        strcpy(icuBeds[i].allocatedDate, "");
    }
    for (int i = 0; i < MAX_GEN_BEDS; i++) {
        genBeds[i].bedNumber          = 2000 + i + 1;  /* General: 2001–2100 */
        genBeds[i].type               = BED_GENERAL;
        genBeds[i].status             = BED_AVAILABLE;
        genBeds[i].assignedPatientID  = 0;
        strcpy(genBeds[i].allocatedDate, "");
    }
}

/* ── File I/O ───────────────────────────────────────────────── */
void loadBeds(void) {
    FILE *f = fopen(BED_FILE, "rb");
    if (!f) {
        initBeds();
        saveBeds();
        return;
    }
    fread(icuBeds, sizeof(Bed), MAX_ICU_BEDS, f);
    fread(genBeds, sizeof(Bed), MAX_GEN_BEDS, f);
    fclose(f);
}

void saveBeds(void) {
    FILE *f = fopen(BED_FILE, "wb");
    if (!f) { printf(RED "  Error saving bed data.\n" RESET); return; }
    fwrite(icuBeds, sizeof(Bed), MAX_ICU_BEDS, f);
    fwrite(genBeds, sizeof(Bed), MAX_GEN_BEDS, f);
    fclose(f);
}

/* ── Get first available bed number of given type ───────────── */
int getAvailableBed(BedType type) {
    if (type == BED_ICU) {
        for (int i = 0; i < MAX_ICU_BEDS; i++)
            if (icuBeds[i].status == BED_AVAILABLE)
                return icuBeds[i].bedNumber;
    } else {
        for (int i = 0; i < MAX_GEN_BEDS; i++)
            if (genBeds[i].status == BED_AVAILABLE)
                return genBeds[i].bedNumber;
    }
    return -1; /* none available */
}

/* ── Allocate bed ────────────────────────────────────────────── */
int allocateBed(int patientID, BedType type) {
    char dt[30];
    getCurrentDateTime(dt, sizeof(dt));

    if (type == BED_ICU) {
        for (int i = 0; i < MAX_ICU_BEDS; i++) {
            if (icuBeds[i].status == BED_AVAILABLE) {
                icuBeds[i].status            = BED_OCCUPIED;
                icuBeds[i].assignedPatientID = patientID;
                strncpy(icuBeds[i].allocatedDate, dt, 19);
                saveBeds();
                return icuBeds[i].bedNumber;
            }
        }
    } else {
        for (int i = 0; i < MAX_GEN_BEDS; i++) {
            if (genBeds[i].status == BED_AVAILABLE) {
                genBeds[i].status            = BED_OCCUPIED;
                genBeds[i].assignedPatientID = patientID;
                strncpy(genBeds[i].allocatedDate, dt, 19);
                saveBeds();
                return genBeds[i].bedNumber;
            }
        }
    }
    return -1; /* no bed available */
}

/* ── Release bed ─────────────────────────────────────────────── */
void releaseBed(int bedNumber) {
    if (bedNumber >= 1001 && bedNumber <= 1000 + MAX_ICU_BEDS) {
        int idx = bedNumber - 1001;
        icuBeds[idx].status            = BED_AVAILABLE;
        icuBeds[idx].assignedPatientID = 0;
        strcpy(icuBeds[idx].allocatedDate, "");
    } else if (bedNumber >= 2001 && bedNumber <= 2000 + MAX_GEN_BEDS) {
        int idx = bedNumber - 2001;
        genBeds[idx].status            = BED_AVAILABLE;
        genBeds[idx].assignedPatientID = 0;
        strcpy(genBeds[idx].allocatedDate, "");
    }
    saveBeds();
}

/* ── Display beds ────────────────────────────────────────────── */
void displayBeds(void) {
    printHeader("BED STATUS");

    int icuFree = 0, icuOcc = 0, genFree = 0, genOcc = 0;

    /* ICU summary */
    printf(RED BOLD "  ── ICU BEDS (1001–%d) ──────────────────\n" RESET, 1000 + MAX_ICU_BEDS);
    for (int i = 0; i < MAX_ICU_BEDS; i++) {
        Bed *b = &icuBeds[i];
        if (b->status == BED_AVAILABLE) { printf(GREEN "  [%4d: FREE]  " RESET, b->bedNumber); icuFree++; }
        else                            { printf(RED   "  [%4d: OCC ]  " RESET, b->bedNumber); icuOcc++; }
        if ((i + 1) % 5 == 0) printf("\n");
    }
    printf("\n");

    /* General summary */
    printf(CYAN BOLD "  ── GENERAL WARD BEDS (2001–%d) ────────\n" RESET, 2000 + MAX_GEN_BEDS);
    for (int i = 0; i < MAX_GEN_BEDS; i++) {
        Bed *b = &genBeds[i];
        if (b->status == BED_AVAILABLE) { printf(GREEN "  [%4d: FREE]  " RESET, b->bedNumber); genFree++; }
        else                            { printf(RED   "  [%4d: OCC ]  " RESET, b->bedNumber); genOcc++; }
        if ((i + 1) % 5 == 0) printf("\n");
    }
    printf("\n");

    printLine('-', 60);
    printf(YELLOW "  ICU    : %d occupied / %d free / %d total\n" RESET, icuOcc,  icuFree,  MAX_ICU_BEDS);
    printf(YELLOW "  General: %d occupied / %d free / %d total\n" RESET, genOcc, genFree, MAX_GEN_BEDS);
    pauseScreen();
}

/* ── Bed allocation UI ───────────────────────────────────────── */
static void allocateBedUI(void) {
    printHeader("ALLOCATE BED");

    int patientID = getIntInput("Enter Patient ID", 1, 99999);

    printf(WHITE "  Bed Type:\n  1. ICU (₹%d/day)\n  2. General Ward (₹%d/day)\n" RESET,
           (int)RATE_ICU_BED_PER_DAY, (int)RATE_GEN_BED_PER_DAY);
    int typeChoice = getIntInput("Select type", 1, 2);
    BedType type = (typeChoice == 1) ? BED_ICU : BED_GENERAL;

    int avail = getAvailableBed(type);
    if (avail == -1) {
        printf(RED "  ✘  No %s beds available!\n" RESET,
               type == BED_ICU ? "ICU" : "General");
        pauseScreen();
        return;
    }

    int bedNo = allocateBed(patientID, type);
    if (bedNo == -1) {
        printf(RED "  ✘  Allocation failed.\n" RESET);
    } else {
        printf(GREEN "\n  ✔  Bed %d allocated to Patient %d.\n" RESET, bedNo, patientID);

        /* Update patient record */
        for (int i = 0; i < patientCount; i++) {
            if (patients[i].patientID == patientID) {
                patients[i].bedNumber = bedNo;
                savePatients();
                break;
            }
        }
        logActivity("Bed allocated");
    }
    pauseScreen();
}

/* ── Release bed UI ─────────────────────────────────────────── */
static void releaseBedUI(void) {
    printHeader("RELEASE BED");
    int bedNo = getIntInput("Enter Bed Number to release", 1001, 2000 + MAX_GEN_BEDS);
    releaseBed(bedNo);
    printf(GREEN "  ✔  Bed %d released and now available.\n" RESET, bedNo);
    logActivity("Bed released");
    pauseScreen();
}

/* ── Bed status summary ─────────────────────────────────────── */
void bedStatus(void) {
    int icuFree = 0, genFree = 0;
    for (int i = 0; i < MAX_ICU_BEDS; i++) if (icuBeds[i].status == BED_AVAILABLE) icuFree++;
    for (int i = 0; i < MAX_GEN_BEDS;  i++) if (genBeds[i].status == BED_AVAILABLE) genFree++;

    printf(CYAN "\n  ┌──────────────────────────────────────┐\n");
    printf("  │         BED AVAILABILITY SUMMARY     │\n");
    printf("  ├──────────────────────────────────────┤\n");
    printf("  │  ICU Beds      : %2d / %2d available   │\n", icuFree, MAX_ICU_BEDS);
    printf("  │  General Beds  : %3d / %3d available  │\n", genFree, MAX_GEN_BEDS);
    printf("  └──────────────────────────────────────┘\n\n" RESET);
}

/* ── Bed menu ────────────────────────────────────────────────── */
void bedMenu(void) {
    int choice;
    do {
        printHeader("BED MANAGEMENT");
        printf(WHITE "  1.  Display All Beds\n");
        printf("  2.  Allocate Bed to Patient\n");
        printf("  3.  Release Bed\n");
        printf("  4.  Quick Bed Status\n");
        printf(YELLOW "  0.  Back to Main Menu\n" RESET);
        choice = getIntInput("Select option", 0, 4);
        switch (choice) {
            case 1: displayBeds();    break;
            case 2: allocateBedUI();  break;
            case 3: releaseBedUI();   break;
            case 4: bedStatus(); pauseScreen(); break;
        }
    } while (choice != 0);
}
