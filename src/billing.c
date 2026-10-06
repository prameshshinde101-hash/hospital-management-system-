/* ============================================================
 *  billing.c — Billing and payment module
 *  Smart Hospital Management System
 * ============================================================ */

#include "../include/billing.h"
#include "../include/patient.h"
#include "../include/doctor.h"
#include "../include/bed.h"

/* ── Globals ────────────────────────────────────────────────── */
Bill bills[MAX_PATIENTS];
int  billCount = 0;

/* ── File I/O ───────────────────────────────────────────────── */
void loadBills(void) {
    FILE *f = fopen(BILL_FILE, "rb");
    if (!f) return;
    fread(&billCount, sizeof(int), 1, f);
    fread(bills, sizeof(Bill), billCount, f);
    fclose(f);
}

void saveBills(void) {
    FILE *f = fopen(BILL_FILE, "wb");
    if (!f) { printf(RED "  Error saving billing data.\n" RESET); return; }
    fwrite(&billCount, sizeof(int), 1, f);
    fwrite(bills, sizeof(Bill), billCount, f);
    fclose(f);
}

/* ── ID helpers ─────────────────────────────────────────────── */
int getNextBillID(void) {
    int maxID = 9000;
    for (int i = 0; i < billCount; i++)
        if (bills[i].billID > maxID) maxID = bills[i].billID;
    return maxID + 1;
}

int getBillIndex(int billID) {
    for (int i = 0; i < billCount; i++)
        if (bills[i].billID == billID) return i;
    return -1;
}

/* ── Status label ────────────────────────────────────────────── */
static const char *billStatusLabel(BillStatus s) {
    switch (s) {
        case BILL_PENDING: return RED    "PENDING " RESET;
        case BILL_PAID:    return GREEN  "PAID    " RESET;
        case BILL_PARTIAL: return YELLOW "PARTIAL " RESET;
        default:           return WHITE  "UNKNOWN " RESET;
    }
}

/* ── Generate bill ───────────────────────────────────────────── */
void generateBill(void) {
    printHeader("GENERATE PATIENT BILL");

    if (billCount >= MAX_PATIENTS) {
        printf(RED "  ✘  Bill storage full!\n" RESET);
        pauseScreen();
        return;
    }

    int patientID = getIntInput("Enter Patient ID", 1, 99999);
    int pIdx = getPatientIndex(patientID);
    if (pIdx == -1) {
        printf(RED "  ✘  Patient not found.\n" RESET);
        pauseScreen();
        return;
    }

    /* Check if bill already exists for this patient */
    for (int i = 0; i < billCount; i++) {
        if (bills[i].patientID == patientID && bills[i].status != BILL_PAID) {
            printf(YELLOW "  ⚠  Unpaid bill already exists for this patient (Bill ID: %d).\n" RESET,
                   bills[i].billID);
            if (!confirmAction("Generate new bill anyway")) {
                pauseScreen();
                return;
            }
        }
    }

    Bill b;
    memset(&b, 0, sizeof(Bill));
    b.billID    = getNextBillID();
    b.patientID = patientID;
    strncpy(b.patientName, patients[pIdx].name, MAX_NAME - 1);
    getCurrentDateTime(b.billDate, sizeof(b.billDate));
    b.status = BILL_PENDING;

    /* Determine bed type for rate */
    int bedNo = patients[pIdx].bedNumber;
    float bedRate = 0.0f;
    if      (bedNo >= 1001 && bedNo <= 1000 + MAX_ICU_BEDS) bedRate = RATE_ICU_BED_PER_DAY;
    else if (bedNo >= 2001 && bedNo <= 2000 + MAX_GEN_BEDS) bedRate = RATE_GEN_BED_PER_DAY;

    /* Consultation fee */
    int dIdx = getDoctorIndex(patients[pIdx].assignedDoctorID);
    if (dIdx >= 0 && doctors[dIdx].experience >= 10)
        b.consultationFee = RATE_CONSULT_SPECIALIST;
    else
        b.consultationFee = RATE_CONSULT_GENERAL;

    /* Days admitted */
    b.daysAdmitted   = getIntInput("Number of days admitted", 0, 365);
    b.bedCharges     = bedRate * b.daysAdmitted;

    /* Other charges */
    b.emergencyCharges = getFloatInput("Emergency charges (0 if none)", 0.0f, 500000.0f);
    b.medicineCharges  = getFloatInput("Medicine charges (₹)",          0.0f, 500000.0f);
    b.labCharges       = getFloatInput("Lab / Diagnostic charges (₹)",  0.0f, 500000.0f);
    b.miscCharges      = getFloatInput("Miscellaneous charges (₹)",     0.0f, 500000.0f);

    getStrInput("Notes (optional)", b.notes, sizeof(b.notes));

    /* Compute total */
    b.totalAmount = b.consultationFee + b.bedCharges + b.emergencyCharges +
                    b.medicineCharges + b.labCharges + b.miscCharges;
    b.amountPaid  = 0.0f;

    bills[billCount++] = b;
    saveBills();

    printFinalBill(b.billID);
    logActivity("Bill generated");
    pauseScreen();
}

/* ── Print final bill ────────────────────────────────────────── */
void printFinalBill(int billID) {
    int idx = getBillIndex(billID);
    if (idx == -1) {
        printf(RED "  ✘  Bill not found.\n" RESET);
        return;
    }

    Bill *b = &bills[idx];

    printf(CYAN "\n");
    printLine('-', 56);
    printf("         SMART HOSPITAL MANAGEMENT SYSTEM\n");
    printf("                    TAX INVOICE\n");
    printLine('-', 56);
    printf(YELLOW "  Bill ID      : %d\n" RESET, b->billID);
    printf(YELLOW "  Patient ID   : %d\n" RESET, b->patientID);
    printf(YELLOW "  Patient Name : %s\n" RESET, b->patientName);
    printf(YELLOW "  Date         : %s\n" RESET, b->billDate);
    printf(YELLOW "  Days Admitted: %d\n" RESET, b->daysAdmitted);
    printf(CYAN);
    printLine('-', 56);
    printf(WHITE "  %-32s %10s\n" RESET, "Description", "Amount (₹)");
    printLine('-', 56);

    printf("  %-32s %10.2f\n", "Consultation Fee",  b->consultationFee);
    printf("  %-32s %10.2f\n", "Bed Charges",       b->bedCharges);
    if (b->emergencyCharges > 0)
        printf("  %-32s %10.2f\n", "Emergency Charges", b->emergencyCharges);
    if (b->medicineCharges > 0)
        printf("  %-32s %10.2f\n", "Medicine Charges",  b->medicineCharges);
    if (b->labCharges > 0)
        printf("  %-32s %10.2f\n", "Lab/Diagnostics",   b->labCharges);
    if (b->miscCharges > 0)
        printf("  %-32s %10.2f\n", "Miscellaneous",     b->miscCharges);

    printf(CYAN);
    printLine('-', 56);
    printf(BOLD WHITE "  %-32s %10.2f\n" RESET, "TOTAL AMOUNT", b->totalAmount);
    printf(GREEN "  %-32s %10.2f\n" RESET,     "Amount Paid",   b->amountPaid);
    printf(RED   "  %-32s %10.2f\n" RESET,     "Balance Due",
           b->totalAmount - b->amountPaid);
    printf(CYAN);
    printLine('-', 56);
    printf("  Status: %s\n", billStatusLabel(b->status));
    printLine('-', 56);
    printf(RESET "\n");
}

/* ── View bill ───────────────────────────────────────────────── */
void viewBill(void) {
    printHeader("VIEW BILL");

    printf("  1. View by Bill ID\n  2. View by Patient ID\n");
    int choice = getIntInput("Choice", 1, 2);

    if (choice == 1) {
        int billID = getIntInput("Enter Bill ID", 1, 99999);
        int idx = getBillIndex(billID);
        if (idx == -1) { printf(RED "  ✘  Bill not found.\n" RESET); pauseScreen(); return; }
        printFinalBill(billID);
    } else {
        int patID = getIntInput("Enter Patient ID", 1, 99999);
        int found = 0;
        for (int i = 0; i < billCount; i++) {
            if (bills[i].patientID == patID) {
                printFinalBill(bills[i].billID);
                found = 1;
            }
        }
        if (!found) printf(RED "  ✘  No bills found for this patient.\n" RESET);
    }
    pauseScreen();
}

/* ── Update payment ──────────────────────────────────────────── */
void updatePayment(void) {
    printHeader("UPDATE PAYMENT");

    int billID = getIntInput("Enter Bill ID", 1, 99999);
    int idx = getBillIndex(billID);
    if (idx == -1) {
        printf(RED "  ✘  Bill not found.\n" RESET);
        pauseScreen();
        return;
    }

    Bill *b = &bills[idx];
    printFinalBill(billID);

    float remaining = b->totalAmount - b->amountPaid;
    printf(YELLOW "  Remaining balance: ₹%.2f\n" RESET, remaining);

    float payment = getFloatInput("Amount being paid (₹)", 0.0f, remaining + 0.01f);
    b->amountPaid += payment;

    if (b->amountPaid >= b->totalAmount - 0.01f) {
        b->status = BILL_PAID;
        printf(GREEN "\n  ✔  Bill FULLY PAID. Thank you!\n" RESET);
    } else {
        b->status = BILL_PARTIAL;
        printf(YELLOW "\n  ✔  Partial payment recorded. Remaining: ₹%.2f\n" RESET,
               b->totalAmount - b->amountPaid);
    }

    saveBills();
    logActivity("Payment updated");
    pauseScreen();
}

/* ── Billing report ──────────────────────────────────────────── */
void generateBillingReport(void) {
    printHeader("BILLING REPORT");

    char dt[30];
    getCurrentDateTime(dt, sizeof(dt));

    float totalRevenue = 0.0f, totalPending = 0.0f;
    int   paid = 0, pending = 0, partial = 0;

    FILE *f = fopen("data/billing_report.txt", "w");
    if (f) {
        fprintf(f, "====== BILLING REPORT ======\nGenerated: %s\n\n", dt);
        fprintf(f, "%-6s %-20s %-12s %-12s %-12s %s\n",
                "BillID","Patient","Total","Paid","Balance","Status");
        fprintf(f, "%-70s\n", "----------------------------------------------------------------------");
    }

    printf(CYAN "  %-6s %-18s %10s %10s %10s %s\n" RESET,
           "BillID","Patient","Total","Paid","Balance","Status");
    printLine('-', 68);

    for (int i = 0; i < billCount; i++) {
        Bill *b = &bills[i];
        float balance = b->totalAmount - b->amountPaid;
        const char *statusStr =
            b->status == BILL_PAID    ? "PAID"    :
            b->status == BILL_PARTIAL ? "PARTIAL" : "PENDING";

        printf("  %-6d %-18s %10.2f %10.2f %10.2f %s\n",
               b->billID, b->patientName,
               b->totalAmount, b->amountPaid, balance,
               billStatusLabel(b->status));

        if (f) fprintf(f, "%-6d %-20s %-12.2f %-12.2f %-12.2f %s\n",
                       b->billID, b->patientName,
                       b->totalAmount, b->amountPaid, balance, statusStr);

        totalRevenue += b->amountPaid;
        totalPending += balance;
        if      (b->status == BILL_PAID)    paid++;
        else if (b->status == BILL_PARTIAL) partial++;
        else                                pending++;
    }

    printLine('-', 68);
    printf(GREEN "  Total Collected : ₹%.2f\n" RESET, totalRevenue);
    printf(RED   "  Total Pending   : ₹%.2f\n" RESET, totalPending);
    printf(YELLOW"  Bills: Paid=%d  Partial=%d  Pending=%d\n" RESET,
           paid, partial, pending);

    if (f) {
        fprintf(f, "\nTotal Collected: %.2f\nTotal Pending: %.2f\n", totalRevenue, totalPending);
        fclose(f);
        printf(GREEN "  ✔  Report saved to data/billing_report.txt\n" RESET);
    }

    pauseScreen();
}

/* ── Billing menu ────────────────────────────────────────────── */
void billingMenu(void) {
    int choice;
    do {
        printHeader("BILLING SYSTEM");
        printf(WHITE "  1.  Generate Bill for Patient\n");
        printf("  2.  View Bill\n");
        printf("  3.  Update Payment\n");
        printf("  4.  Billing Report\n");
        printf(YELLOW "  0.  Back to Main Menu\n" RESET);
        choice = getIntInput("Select option", 0, 4);
        switch (choice) {
            case 1: generateBill();        break;
            case 2: viewBill();            break;
            case 3: updatePayment();       break;
            case 4: generateBillingReport();break;
        }
    } while (choice != 0);
}
