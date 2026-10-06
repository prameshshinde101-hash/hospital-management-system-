/* ============================================================
 *  patient.c — Patient management module
 *  Smart Hospital Management System
 * ============================================================ */

#include "../include/patient.h"
#include "../include/bed.h"

/* ── Globals ────────────────────────────────────────────────── */
Patient patients[MAX_PATIENTS];
int     patientCount = 0;

/* ── File I/O ───────────────────────────────────────────────── */
void loadPatients(void) {
    FILE *f = fopen(PATIENT_FILE, "rb");
    if (!f) return;
    fread(&patientCount, sizeof(int), 1, f);
    fread(patients, sizeof(Patient), patientCount, f);
    fclose(f);
}

void savePatients(void) {
    FILE *f = fopen(PATIENT_FILE, "wb");
    if (!f) { printf(RED "  Error saving patients.\n" RESET); return; }
    fwrite(&patientCount, sizeof(int), 1, f);
    fwrite(patients, sizeof(Patient), patientCount, f);
    fclose(f);
}

/* ── ID helpers ─────────────────────────────────────────────── */
int getNextPatientID(void) {
    int maxID = 1000;
    for (int i = 0; i < patientCount; i++)
        if (patients[i].patientID > maxID)
            maxID = patients[i].patientID;
    return maxID + 1;
}

int patientExists(int id) {
    for (int i = 0; i < patientCount; i++)
        if (patients[i].patientID == id && patients[i].isActive)
            return 1;
    return 0;
}

int getPatientIndex(int id) {
    for (int i = 0; i < patientCount; i++)
        if (patients[i].patientID == id)
            return i;
    return -1;
}

/* ── Display single patient record ──────────────────────────── */
static void displayPatientCard(Patient *p) {
    printf(CYAN "  ┌─────────────────────────────────────────────────┐\n" RESET);
    printf(CYAN "  │" YELLOW " Patient ID  : " WHITE "%-5d" CYAN "                             │\n" RESET, p->patientID);
    printf(CYAN "  │" YELLOW " Name        : " WHITE "%-33s" CYAN " │\n" RESET, p->name);
    printf(CYAN "  │" YELLOW " Age/Gender  : " WHITE "%d / %c" CYAN "                             │\n" RESET, p->age, p->gender);
    printf(CYAN "  │" YELLOW " Blood Group : " WHITE "%-6s" CYAN "                            │\n" RESET, p->bloodGroup);
    printf(CYAN "  │" YELLOW " Disease     : " WHITE "%-33s" CYAN " │\n" RESET, p->disease);
    printf(CYAN "  │" YELLOW " Phone       : " WHITE "%-33s" CYAN " │\n" RESET, p->phone);
    printf(CYAN "  │" YELLOW " Doctor ID   : " WHITE "%-5d" CYAN "                             │\n" RESET, p->assignedDoctorID);
    printf(CYAN "  │" YELLOW " Bed Number  : " WHITE "%-5d" CYAN "                             │\n" RESET, p->bedNumber);
    printf(CYAN "  │" YELLOW " Admitted    : " WHITE "%-20s" CYAN "              │\n" RESET, p->admitDate);
    printf(CYAN "  │" YELLOW " Status      : " RESET);
    if (p->isActive) printf(GREEN "Active      " CYAN "                             │\n" RESET);
    else             printf(RED   "Discharged  " CYAN "                             │\n" RESET);
    printf(CYAN "  └─────────────────────────────────────────────────┘\n\n" RESET);
}

/* ── Add patient ────────────────────────────────────────────── */
void addPatient(void) {
    printHeader("ADD NEW PATIENT");

    if (patientCount >= MAX_PATIENTS) {
        printf(RED "  ✘  Patient capacity full!\n" RESET);
        pauseScreen();
        return;
    }

    Patient p;
    memset(&p, 0, sizeof(Patient));
    p.patientID = getNextPatientID();
    p.isActive  = 1;

    printf(GREEN "  Auto-assigned Patient ID: %d\n\n" RESET, p.patientID);

    getStrInput("Full Name", p.name, MAX_NAME);
    if (strlen(p.name) == 0) { printf(RED "  Name cannot be empty.\n" RESET); pauseScreen(); return; }

    p.age = getIntInput("Age (1-120)", 1, 120);

    char gBuf[4];
    while (1) {
        getStrInput("Gender (M/F/O)", gBuf, 4);
        toUpperStr(gBuf);
        if (gBuf[0]=='M'||gBuf[0]=='F'||gBuf[0]=='O') { p.gender=gBuf[0]; break; }
        printf(RED "  Enter M, F, or O.\n" RESET);
    }

    getStrInput("Blood Group (e.g. A+)", p.bloodGroup, MAX_BLOOD);
    toUpperStr(p.bloodGroup);
    getStrInput("Disease/Diagnosis", p.disease, MAX_DISEASE);
    getStrInput("Phone Number", p.phone, MAX_PHONE);
    getStrInput("Address", p.address, MAX_ADDR);

    p.assignedDoctorID = 0;
    p.bedNumber = 0;
    getCurrentDateTime(p.admitDate, sizeof(p.admitDate));

    patients[patientCount++] = p;
    savePatients();

    printf(GREEN "\n  ✔  Patient added successfully! ID: %d\n" RESET, p.patientID);
    logActivity("Patient added");
    pauseScreen();
}

/* ── Update patient ─────────────────────────────────────────── */
void updatePatient(void) {
    printHeader("UPDATE PATIENT");

    int id = getIntInput("Enter Patient ID", 1, 99999);
    int idx = getPatientIndex(id);
    if (idx == -1 || !patients[idx].isActive) {
        printf(RED "  ✘  Patient not found.\n" RESET);
        pauseScreen();
        return;
    }

    Patient *p = &patients[idx];
    printf(YELLOW "\n  Updating patient: %s (leave blank to keep current)\n\n" RESET, p->name);

    char buf[MAX_NAME];
    getStrInput("New Name", buf, MAX_NAME);
    if (strlen(buf) > 0) strncpy(p->name, buf, MAX_NAME - 1);

    printf(WHITE "  New Age (0 to skip): " RESET);
    char ageBuf[10];
    fgets(ageBuf, sizeof(ageBuf), stdin);
    int newAge;
    if (sscanf(ageBuf, "%d", &newAge) == 1 && newAge > 0) p->age = newAge;

    getStrInput("New Disease/Diagnosis", buf, MAX_DISEASE);
    if (strlen(buf) > 0) strncpy(p->disease, buf, MAX_DISEASE - 1);

    getStrInput("New Phone", buf, MAX_PHONE);
    if (strlen(buf) > 0) strncpy(p->phone, buf, MAX_PHONE - 1);

    getStrInput("New Address", buf, MAX_ADDR);
    if (strlen(buf) > 0) strncpy(p->address, buf, MAX_ADDR - 1);

    savePatients();
    printf(GREEN "\n  ✔  Patient updated successfully.\n" RESET);
    logActivity("Patient record updated");
    pauseScreen();
}

/* ── Delete (discharge) patient ─────────────────────────────── */
void deletePatient(void) {
    printHeader("DISCHARGE PATIENT");

    int id = getIntInput("Enter Patient ID", 1, 99999);
    int idx = getPatientIndex(id);
    if (idx == -1 || !patients[idx].isActive) {
        printf(RED "  ✘  Patient not found or already discharged.\n" RESET);
        pauseScreen();
        return;
    }

    displayPatientCard(&patients[idx]);
    if (!confirmAction("Confirm discharge")) { pauseScreen(); return; }

    /* Release bed if allocated */
    if (patients[idx].bedNumber > 0) releaseBed(patients[idx].bedNumber);

    patients[idx].isActive = 0;
    savePatients();
    printf(GREEN "\n  ✔  Patient discharged.\n" RESET);
    logActivity("Patient discharged");
    pauseScreen();
}

/* ── Search patient ─────────────────────────────────────────── */
void searchPatient(void) {
    printHeader("SEARCH PATIENT");

    printf("  1. Search by ID\n  2. Search by Name\n");
    int choice = getIntInput("Choice", 1, 2);

    int found = 0;
    if (choice == 1) {
        int id = getIntInput("Enter Patient ID", 1, 99999);
        for (int i = 0; i < patientCount; i++) {
            if (patients[i].patientID == id) {
                displayPatientCard(&patients[i]);
                found = 1;
            }
        }
    } else {
        char name[MAX_NAME];
        getStrInput("Enter Name (partial OK)", name, MAX_NAME);
        toUpperStr(name);
        for (int i = 0; i < patientCount; i++) {
            char pName[MAX_NAME];
            strncpy(pName, patients[i].name, MAX_NAME - 1);
            toUpperStr(pName);
            if (strstr(pName, name)) {
                displayPatientCard(&patients[i]);
                found = 1;
            }
        }
    }

    if (!found) printf(RED "  ✘  No patient found.\n" RESET);
    pauseScreen();
}

/* ── Display all patients ────────────────────────────────────── */
void displayAllPatients(void) {
    printHeader("ALL PATIENTS");

    if (patientCount == 0) {
        printf(YELLOW "  No patients registered.\n" RESET);
        pauseScreen();
        return;
    }

    printf(CYAN "  %-6s %-22s %-4s %-2s %-6s %-18s %-6s %s\n" RESET,
           "ID", "Name", "Age", "G", "Blood", "Disease", "Bed", "Status");
    printLine('-', 78);

    int active = 0;
    for (int i = 0; i < patientCount; i++) {
        Patient *p = &patients[i];
        const char *status = p->isActive ? GREEN "Active" RESET : RED "Discharged" RESET;
        printf("  %-6d %-22s %-4d %-2c %-6s %-18s %-6d %s\n",
               p->patientID, p->name, p->age, p->gender,
               p->bloodGroup, p->disease, p->bedNumber, status);
        if (p->isActive) active++;
    }
    printLine('-', 78);
    printf(YELLOW "  Total: %d  |  Active: %d  |  Discharged: %d\n" RESET,
           patientCount, active, patientCount - active);
    pauseScreen();
}

/* ── Merge sort by name ─────────────────────────────────────── */
static void mergeSortName(Patient *arr, int l, int r) {
    if (l >= r) return;
    int m = (l + r) / 2;
    mergeSortName(arr, l, m);
    mergeSortName(arr, m + 1, r);

    int n1 = m - l + 1, n2 = r - m;
    Patient *L = malloc(n1 * sizeof(Patient));
    Patient *R = malloc(n2 * sizeof(Patient));
    if (!L || !R) { free(L); free(R); return; }
    memcpy(L, arr + l, n1 * sizeof(Patient));
    memcpy(R, arr + m + 1, n2 * sizeof(Patient));

    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        if (strcasecmp(L[i].name, R[j].name) <= 0) arr[k++] = L[i++];
        else                                         arr[k++] = R[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
    free(L); free(R);
}

void sortPatientsByName(void) {
    if (patientCount == 0) return;
    mergeSortName(patients, 0, patientCount - 1);
    savePatients();
    printf(GREEN "  ✔  Patients sorted by name.\n" RESET);
}

/* ── Sort by age (bubble sort for demo) ─────────────────────── */
void sortPatientsByAge(void) {
    for (int i = 0; i < patientCount - 1; i++)
        for (int j = 0; j < patientCount - i - 1; j++)
            if (patients[j].age > patients[j+1].age) {
                Patient tmp = patients[j];
                patients[j] = patients[j+1];
                patients[j+1] = tmp;
            }
    savePatients();
    printf(GREEN "  ✔  Patients sorted by age.\n" RESET);
}

/* ── Report ─────────────────────────────────────────────────── */
void generatePatientReport(void) {
    printHeader("PATIENT REPORT");
    char dt[30];
    getCurrentDateTime(dt, sizeof(dt));

    FILE *f = fopen("data/patient_report.txt", "w");
    if (f) {
        fprintf(f, "====== PATIENT REPORT ======\nGenerated: %s\n\n", dt);
        int active = 0;
        for (int i = 0; i < patientCount; i++) {
            Patient *p = &patients[i];
            fprintf(f, "ID: %d | %s | Age: %d | %s | %s | %s\n",
                    p->patientID, p->name, p->age, p->bloodGroup,
                    p->disease, p->isActive ? "Active" : "Discharged");
            if (p->isActive) active++;
        }
        fprintf(f, "\nTotal: %d | Active: %d\n", patientCount, active);
        fclose(f);
        printf(GREEN "  ✔  Report saved to data/patient_report.txt\n" RESET);
    } else {
        printf(RED "  ✘  Could not write report file.\n" RESET);
    }

    displayAllPatients();
}

/* ── Patient menu ───────────────────────────────────────────── */
void patientMenu(void) {
    int choice;
    do {
        printHeader("PATIENT MANAGEMENT");
        printf(WHITE "  1.  Add Patient\n");
        printf("  2.  Update Patient Details\n");
        printf("  3.  Discharge Patient\n");
        printf("  4.  Search Patient\n");
        printf("  5.  Display All Patients\n");
        printf("  6.  Sort Patients by Name\n");
        printf("  7.  Sort Patients by Age\n");
        printf("  8.  Generate Patient Report\n");
        printf(YELLOW "  0.  Back to Main Menu\n" RESET);
        choice = getIntInput("Select option", 0, 8);
        switch (choice) {
            case 1: addPatient();           break;
            case 2: updatePatient();        break;
            case 3: deletePatient();        break;
            case 4: searchPatient();        break;
            case 5: displayAllPatients();   break;
            case 6: sortPatientsByName(); displayAllPatients(); break;
            case 7: sortPatientsByAge();  displayAllPatients(); break;
            case 8: generatePatientReport(); break;
        }
    } while (choice != 0);
}
