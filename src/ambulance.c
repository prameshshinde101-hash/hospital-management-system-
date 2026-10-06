/* ============================================================
 *  ambulance.c — Ambulance management module
 *  Smart Hospital Management System
 * ============================================================ */

#include "../include/ambulance.h"

/* ── Globals ────────────────────────────────────────────────── */
Ambulance ambulances[MAX_AMBULANCES];
int       ambCount = 0;

/* ── File I/O ───────────────────────────────────────────────── */
void loadAmbulances(void) {
    FILE *f = fopen(AMBULANCE_FILE, "rb");
    if (!f) return;
    fread(&ambCount, sizeof(int), 1, f);
    fread(ambulances, sizeof(Ambulance), ambCount, f);
    fclose(f);
}

void saveAmbulances(void) {
    FILE *f = fopen(AMBULANCE_FILE, "wb");
    if (!f) { printf(RED "  Error saving ambulance data.\n" RESET); return; }
    fwrite(&ambCount, sizeof(int), 1, f);
    fwrite(ambulances, sizeof(Ambulance), ambCount, f);
    fclose(f);
}

/* ── ID helpers ─────────────────────────────────────────────── */
int getNextAmbID(void) {
    int maxID = 500;
    for (int i = 0; i < ambCount; i++)
        if (ambulances[i].ambID > maxID) maxID = ambulances[i].ambID;
    return maxID + 1;
}

int getAvailableAmbulance(void) {
    for (int i = 0; i < ambCount; i++)
        if (ambulances[i].status == AMB_AVAILABLE)
            return i;
    return -1;
}

/* ── Status label ────────────────────────────────────────────── */
static const char *ambStatusLabel(AmbStatus s) {
    switch (s) {
        case AMB_AVAILABLE:   return GREEN  "Available  " RESET;
        case AMB_DISPATCHED:  return RED    "Dispatched " RESET;
        case AMB_MAINTENANCE: return YELLOW "Maintenance" RESET;
        default:              return WHITE  "Unknown    " RESET;
    }
}

/* ── Add ambulance ───────────────────────────────────────────── */
void addAmbulance(void) {
    printHeader("ADD AMBULANCE");

    if (ambCount >= MAX_AMBULANCES) {
        printf(RED "  ✘  Ambulance fleet capacity full!\n" RESET);
        pauseScreen();
        return;
    }

    Ambulance a;
    memset(&a, 0, sizeof(Ambulance));
    a.ambID     = getNextAmbID();
    a.status    = AMB_AVAILABLE;
    a.totalTrips = 0;
    a.assignedPatientID = 0;

    printf(GREEN "  Auto-assigned Ambulance ID: %d\n\n" RESET, a.ambID);

    getStrInput("Vehicle Number (e.g. MH12AB1234)", a.vehicleNumber, sizeof(a.vehicleNumber));
    getStrInput("Driver Name",  a.driverName,  MAX_NAME);
    getStrInput("Driver Phone", a.driverPhone, MAX_PHONE);

    printf(WHITE "  Set initial status:\n  1.Available  2.Maintenance\n" RESET);
    int sc = getIntInput("Choice", 1, 2);
    a.status = (sc == 1) ? AMB_AVAILABLE : AMB_MAINTENANCE;

    ambulances[ambCount++] = a;
    saveAmbulances();

    printf(GREEN "\n  ✔  Ambulance added. ID: %d | Vehicle: %s\n" RESET,
           a.ambID, a.vehicleNumber);
    logActivity("Ambulance added");
    pauseScreen();
}

/* ── View all ambulances ─────────────────────────────────────── */
void viewAmbulances(void) {
    printHeader("AMBULANCE FLEET");

    if (ambCount == 0) {
        printf(YELLOW "  No ambulances registered.\n" RESET);
        pauseScreen();
        return;
    }

    printf(CYAN "  %-5s %-14s %-22s %-14s %-6s %s\n" RESET,
           "ID", "Vehicle No.", "Driver", "Phone", "Trips", "Status");
    printLine('-', 80);

    for (int i = 0; i < ambCount; i++) {
        Ambulance *a = &ambulances[i];
        printf("  %-5d %-14s %-22s %-14s %-6d %s\n",
               a->ambID, a->vehicleNumber, a->driverName,
               a->driverPhone, a->totalTrips, ambStatusLabel(a->status));
    }
    printLine('-', 80);

    int avail = 0, disp = 0, maint = 0;
    for (int i = 0; i < ambCount; i++) {
        if      (ambulances[i].status == AMB_AVAILABLE)   avail++;
        else if (ambulances[i].status == AMB_DISPATCHED)  disp++;
        else                                               maint++;
    }
    printf(YELLOW "  Total: %d  |  " GREEN "Available: %d  " RED "Dispatched: %d  " YELLOW "Maintenance: %d\n" RESET,
           ambCount, avail, disp, maint);
    pauseScreen();
}

/* ── Allocate ambulance ──────────────────────────────────────── */
void allocateAmbulance(void) {
    printHeader("ALLOCATE AMBULANCE");

    int idx = getAvailableAmbulance();
    if (idx == -1) {
        printf(RED "  ✘  No ambulances currently available!\n" RESET);
        pauseScreen();
        return;
    }

    printf(GREEN "  ✔  Found available ambulance:\n" RESET);
    printf(CYAN "  ID: %d | Vehicle: %s | Driver: %s | Phone: %s\n\n" RESET,
           ambulances[idx].ambID, ambulances[idx].vehicleNumber,
           ambulances[idx].driverName, ambulances[idx].driverPhone);

    int patientID = getIntInput("Assign to Patient ID (0 = external)", 0, 99999);

    ambulances[idx].status             = AMB_DISPATCHED;
    ambulances[idx].assignedPatientID  = patientID;
    ambulances[idx].totalTrips++;
    getCurrentDateTime(ambulances[idx].dispatchTime, sizeof(ambulances[idx].dispatchTime));

    saveAmbulances();

    printf(GREEN "\n  ✔  Ambulance %s dispatched at %s.\n" RESET,
           ambulances[idx].vehicleNumber, ambulances[idx].dispatchTime);
    logActivity("Ambulance dispatched");
    pauseScreen();
}

/* ── Release ambulance ───────────────────────────────────────── */
void releaseAmbulance(void) {
    printHeader("RELEASE AMBULANCE");

    int ambID = getIntInput("Enter Ambulance ID", 1, 99999);
    int found = -1;
    for (int i = 0; i < ambCount; i++) {
        if (ambulances[i].ambID == ambID) { found = i; break; }
    }

    if (found == -1) {
        printf(RED "  ✘  Ambulance ID not found.\n" RESET);
        pauseScreen();
        return;
    }

    ambulances[found].status            = AMB_AVAILABLE;
    ambulances[found].assignedPatientID = 0;
    strcpy(ambulances[found].dispatchTime, "");

    saveAmbulances();
    printf(GREEN "  ✔  Ambulance %s is now available.\n" RESET,
           ambulances[found].vehicleNumber);
    logActivity("Ambulance returned to fleet");
    pauseScreen();
}

/* ── Ambulance status dashboard ─────────────────────────────── */
void ambulanceStatus(void) {
    printHeader("AMBULANCE STATUS DASHBOARD");

    int avail = 0, disp = 0, maint = 0;
    for (int i = 0; i < ambCount; i++) {
        if      (ambulances[i].status == AMB_AVAILABLE)   avail++;
        else if (ambulances[i].status == AMB_DISPATCHED)  disp++;
        else                                               maint++;
    }

    printf(CYAN "  ╔═══════════════════════════════════════╗\n");
    printf("  ║      AMBULANCE FLEET SUMMARY          ║\n");
    printf("  ╠═══════════════════════════════════════╣\n");
    printf("  ║  Total Fleet    : %-4d                ║\n", ambCount);
    printf("  ║" GREEN "  Available      : %-4d                " CYAN "║\n", avail);
    printf("  ║" RED   "  Dispatched     : %-4d                " CYAN "║\n", disp);
    printf("  ║" YELLOW "  Under Maint.   : %-4d                " CYAN "║\n", maint);
    printf("  ╚═══════════════════════════════════════╝\n\n" RESET);

    if (disp > 0) {
        printf(RED "  Currently Dispatched:\n" RESET);
        printf(CYAN "  %-5s %-14s %-20s %-12s\n" RESET,
               "ID", "Vehicle", "Driver", "Patient ID");
        printLine('-', 58);
        for (int i = 0; i < ambCount; i++) {
            if (ambulances[i].status == AMB_DISPATCHED) {
                printf("  %-5d %-14s %-20s %-12d\n",
                       ambulances[i].ambID,
                       ambulances[i].vehicleNumber,
                       ambulances[i].driverName,
                       ambulances[i].assignedPatientID);
            }
        }
    }
    pauseScreen();
}

/* ── Ambulance menu ─────────────────────────────────────────── */
void ambulanceMenu(void) {
    int choice;
    do {
        printHeader("AMBULANCE MANAGEMENT");
        printf(WHITE "  1.  Add Ambulance\n");
        printf("  2.  View All Ambulances\n");
        printf("  3.  Dispatch Ambulance\n");
        printf("  4.  Release Ambulance (Return to Fleet)\n");
        printf("  5.  Fleet Status Dashboard\n");
        printf(YELLOW "  0.  Back to Main Menu\n" RESET);
        choice = getIntInput("Select option", 0, 5);
        switch (choice) {
            case 1: addAmbulance();     break;
            case 2: viewAmbulances();   break;
            case 3: allocateAmbulance();break;
            case 4: releaseAmbulance(); break;
            case 5: ambulanceStatus();  break;
        }
    } while (choice != 0);
}
