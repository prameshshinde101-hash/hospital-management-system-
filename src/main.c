/* ============================================================
 *  main.c — Entry point, main menu, statistics dashboard
 *  Smart Hospital Management System
 * ============================================================ */

#include "../include/utility.h"
#include "../include/patient.h"
#include "../include/doctor.h"
#include "../include/appointment.h"
#include "../include/emergency.h"
#include "../include/ambulance.h"
#include "../include/billing.h"
#include "../include/login.h"
#include "../include/bed.h"
#include "../include/linked_list.h"

/* ── Forward declarations ───────────────────────────────────── */
static void mainMenu(void);
static void reportsMenu(void);
static void settingsMenu(void);
static void statisticsDashboard(void);

/* ── Ensure required directories exist ─────────────────────── */
static void ensureDirectories(void) {
    system("mkdir -p " DATA_DIR);
    system("mkdir -p " LOG_DIR);
    system("mkdir -p " BACKUP_DIR);
}

/* ── Load all data from disk ────────────────────────────────── */
static void loadAllData(void) {
    loadPatients();
    loadDoctors();
    loadAppointments();
    loadBills();
    loadAmbulances();
    loadBeds();
    loadEmergency();
    ll_loadFromFile(&activityLogList);   /* load log into linked list */
}

/* ── Statistics dashboard ───────────────────────────────────── */
static void statisticsDashboard(void) {
    printHeader("HOSPITAL STATISTICS DASHBOARD");

    /* Count active patients */
    int activePatients = 0, discharged = 0;
    for (int i = 0; i < patientCount; i++) {
        if (patients[i].isActive) activePatients++;
        else                      discharged++;
    }

    /* Count available doctors */
    int availDoctors = 0;
    for (int i = 0; i < doctorCount; i++)
        if (doctors[i].isAvailable) availDoctors++;

    /* Count scheduled appointments */
    int scheduledAppts = 0;
    for (int i = 0; i < apptCount; i++)
        if (appointments[i].status == APPT_SCHEDULED) scheduledAppts++;

    /* Bed occupancy */
    int icuOcc = 0, genOcc = 0;
    for (int i = 0; i < MAX_ICU_BEDS; i++)
        if (icuBeds[i].status == BED_OCCUPIED) icuOcc++;
    for (int i = 0; i < MAX_GEN_BEDS; i++)
        if (genBeds[i].status == BED_OCCUPIED) genOcc++;

    /* Revenue */
    float totalRevenue = 0.0f, totalPending = 0.0f;
    for (int i = 0; i < billCount; i++) {
        totalRevenue += bills[i].amountPaid;
        totalPending += (bills[i].totalAmount - bills[i].amountPaid);
    }

    /* Available ambulances */
    int availAmb = 0;
    for (int i = 0; i < ambCount; i++)
        if (ambulances[i].status == AMB_AVAILABLE) availAmb++;

    char dt[30];
    getCurrentDateTime(dt, sizeof(dt));
    printf(CYAN "  Last Updated: %s\n\n" RESET, dt);

    /* ── Row 1: Patients ────────────────────────────────────── */
    printf(CYAN  "  ╔══════════════════╦══════════════════╦══════════════════╗\n");
    printf("  ║" GREEN  "  🏥 PATIENTS      " CYAN "║" BLUE  "  👨‍⚕️ DOCTORS       " CYAN "║" MAGENTA "  📅 APPOINTMENTS  " CYAN "║\n");
    printf("  ║" WHITE  "  Active   : %-5d " CYAN "║" WHITE "  Total    : %-5d " CYAN "║" WHITE "  Scheduled: %-4d " CYAN "║\n",
           activePatients, doctorCount, scheduledAppts);
    printf("  ║" WHITE  "  Total    : %-5d " CYAN "║" WHITE "  Available: %-5d " CYAN "║" WHITE "  Total    : %-4d " CYAN "║\n",
           patientCount, availDoctors, apptCount);
    printf("  ╠══════════════════╬══════════════════╬══════════════════╣\n");

    /* ── Row 2: Beds ────────────────────────────────────────── */
    printf("  ║" RED    "  🛏️  ICU BEDS      " CYAN "║" YELLOW "  🚑 AMBULANCES    " CYAN "║" GREEN  "  🚨 EMERGENCY     " CYAN "║\n");
    printf("  ║" WHITE  "  Occupied : %-5d " CYAN "║" WHITE "  Fleet    : %-5d " CYAN "║" WHITE "  In Queue : %-4d " CYAN "║\n",
           icuOcc, ambCount, emergQueue.size);
    printf("  ║" WHITE  "  Free     : %-5d " CYAN "║" WHITE "  Available: %-5d " CYAN "║" WHITE "  Handled  : %-4d " CYAN "║\n",
           MAX_ICU_BEDS - icuOcc, availAmb, 0);
    printf("  ╠══════════════════╩══════════════════╩══════════════════╣\n");

    /* ── Row 3: Revenue ─────────────────────────────────────── */
    printf("  ║" YELLOW "  💰 FINANCIAL SUMMARY                                 " CYAN "║\n");
    printf("  ║" GREEN  "  Total Collected  : ₹ %10.2f                     " CYAN "║\n", totalRevenue);
    printf("  ║" RED    "  Pending Dues     : ₹ %10.2f                     " CYAN "║\n", totalPending);
    printf("  ║" WHITE  "  Total Bills      : %-5d                            " CYAN "║\n", billCount);
    printf("  ║" WHITE  "  General Beds Free: %-5d / %-5d                    " CYAN "║\n",
           MAX_GEN_BEDS - genOcc, MAX_GEN_BEDS);
    printf("  ╚═══════════════════════════════════════════════════════╝\n" RESET);

    /* ASCII bar chart for bed occupancy */
    printf(YELLOW "\n  BED OCCUPANCY CHART:\n" RESET);
    int icuPct = (MAX_ICU_BEDS > 0) ? (icuOcc * 20) / MAX_ICU_BEDS : 0;
    int genPct = (MAX_GEN_BEDS > 0) ? (genOcc * 20) / MAX_GEN_BEDS : 0;
    printf("  ICU  [");
    for (int i = 0; i < 20; i++) printf(i < icuPct ? RED "█" RESET : "░");
    printf("]  %d/%d\n", icuOcc, MAX_ICU_BEDS);
    printf("  GEN  [");
    for (int i = 0; i < 20; i++) printf(i < genPct ? CYAN "█" RESET : "░");
    printf("]  %d/%d\n", genOcc, MAX_GEN_BEDS);

    pauseScreen();
}

/* ── Reports menu ───────────────────────────────────────────── */
static void reportsMenu(void) {
    int choice;
    do {
        printHeader("REPORTS & ANALYTICS");
        printf(WHITE "  1.  Patient Report\n");
        printf("  2.  Doctor Report\n");
        printf("  3.  Appointment Report\n");
        printf("  4.  Emergency Report\n");
        printf("  5.  Billing Report\n");
        printf("  6.  Statistics Dashboard\n");
        printf(YELLOW "  0.  Back to Main Menu\n" RESET);
        choice = getIntInput("Select option", 0, 6);
        switch (choice) {
            case 1: generatePatientReport();     break;
            case 2: generateDoctorReport();      break;
            case 3: generateAppointmentReport(); break;
            case 4: generateEmergencyReport();   break;
            case 5: generateBillingReport();     break;
            case 6: statisticsDashboard();       break;
        }
    } while (choice != 0);
}

/* ── Settings / admin menu ──────────────────────────────────── */
static void settingsMenu(void) {
    int choice;
    do {
        printHeader("SETTINGS & ADMINISTRATION");
        printf(WHITE "  1.  Change Password\n");
        printf("  2.  View Activity Log (File)\n");
        printf("  3.  Activity Log Linked List Viewer\n");
        printf("  4.  Backup Data\n");
        printf("  5.  Restore Data\n");
        printf("  6.  Clear Screen\n");
        printf(YELLOW "  0.  Back to Main Menu\n" RESET);
        choice = getIntInput("Select option", 0, 6);
        switch (choice) {
            case 1: changePassword(); break;
            case 2: {
                printHeader("ACTIVITY LOG (FILE)");
                FILE *f = fopen(LOG_FILE, "r");
                if (!f) { printf(YELLOW "  Log file is empty or not found.\n" RESET); }
                else {
                    char line[256];
                    int lines = 0;
                    while (fgets(line, sizeof(line), f) && lines < 50) {
                        printf(WHITE "  %s" RESET, line);
                        lines++;
                    }
                    if (lines == 50) printf(YELLOW "  ... (showing last 50 entries)\n" RESET);
                    fclose(f);
                }
                pauseScreen();
                break;
            }
            case 3: logListMenu();  break;
            case 4: backupData();   break;
            case 5: restoreData();  break;
            case 6: clearScreen();  break;
        }
    } while (choice != 0);
}

/* ── Main menu ──────────────────────────────────────────────── */
static void mainMenu(void) {
    int choice;
    do {
        clearScreen();
        printf(CYAN);
        printLine('-', 70);
        printf("  " YELLOW BOLD "SMART HOSPITAL MANAGEMENT SYSTEM" CYAN
               "  |  " WHITE "Admin Panel\n" CYAN);
        printLine('-', 70);
        printf(RESET);

        /* Quick status bar */
        int active = 0;
        for (int i = 0; i < patientCount; i++) if (patients[i].isActive) active++;
        printf(WHITE "  Patients: " GREEN "%d active" WHITE "  |  Doctors: " BLUE "%d" WHITE
               "  |  Emergency Queue: " RED "%d" WHITE "  |  Appointments: " MAGENTA "%d\n\n" RESET,
               active, doctorCount, emergQueue.size, apptCount);

        printf(CYAN "  ╔══════════════════════════════════════════════════════╗\n" RESET);
        printf(WHITE "      1.  " YELLOW "Patient Management\n" RESET);
        printf(WHITE "      2.  " YELLOW "Doctor Management\n" RESET);
        printf(WHITE "      3.  " YELLOW "Appointment Management\n" RESET);
        printf(WHITE "      4.  " RED    "Emergency Handling\n" RESET);
        printf(WHITE "      5.  " YELLOW "Ambulance Management\n" RESET);
        printf(WHITE "      6.  " YELLOW "Bed Management\n" RESET);
        printf(WHITE "      7.  " YELLOW "Billing System\n" RESET);
        printf(WHITE "      8.  " CYAN   "Reports & Analytics\n" RESET);
        printf(WHITE "      9.  " CYAN   "Statistics Dashboard\n" RESET);
        printf(WHITE "      10. " MAGENTA"Settings & Administration\n" RESET);
        printf(WHITE "      0.  " RED    "Logout & Exit\n" RESET);
        printf(CYAN "  ╚══════════════════════════════════════════════════════╝\n" RESET);

        choice = getIntInput("Select module", 0, 10);
        switch (choice) {
            case 1:  patientMenu();          break;
            case 2:  doctorMenu();           break;
            case 3:  appointmentMenu();      break;
            case 4:  emergencyMenu();        break;
            case 5:  ambulanceMenu();        break;
            case 6:  bedMenu();              break;
            case 7:  billingMenu();          break;
            case 8:  reportsMenu();          break;
            case 9:  statisticsDashboard();  break;
            case 10: settingsMenu();         break;
            case 0:
                if (confirmAction("Confirm logout")) {
                    logActivity("Admin logged out");
                    printf(GREEN "\n  Goodbye! Stay healthy. 👋\n\n" RESET);
                }
                else choice = 1; /* stay in loop */
                break;
        }
    } while (choice != 0);
}

/* ── Program entry point ────────────────────────────────────── */
int main(void) {
    ensureDirectories();
    welcomeScreen();
    loadingBar("Loading all hospital records", 25);

    if (!loginSystem()) {
        printf(RED "\n  Access denied. Exiting.\n\n" RESET);
        return 1;
    }

    loadAllData();
    loadingBar("Preparing dashboard", 15);

    mainMenu();

    return 0;
}
