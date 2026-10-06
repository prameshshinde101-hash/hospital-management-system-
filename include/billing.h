#ifndef BILLING_H
#define BILLING_H

/* ============================================================
 *  billing.h — Billing system structures and prototypes
 *  Smart Hospital Management System
 * ============================================================ */

#include "utility.h"

typedef enum { BILL_PENDING = 0, BILL_PAID, BILL_PARTIAL } BillStatus;

/* ── Bill record ────────────────────────────────────────────── */
typedef struct {
    int        billID;
    int        patientID;
    char       patientName[MAX_NAME];
    char       billDate[20];
    float      consultationFee;
    float      bedCharges;       /* per day */
    int        daysAdmitted;
    float      emergencyCharges;
    float      medicineCharges;
    float      labCharges;
    float      miscCharges;
    float      totalAmount;
    float      amountPaid;
    BillStatus status;
    char       notes[120];
} Bill;

/* ── Charge rate table ──────────────────────────────────────── */
#define RATE_CONSULT_GENERAL  500.0f
#define RATE_CONSULT_SPECIALIST 1000.0f
#define RATE_ICU_BED_PER_DAY  3000.0f
#define RATE_GEN_BED_PER_DAY   800.0f
#define RATE_EMERGENCY_BASE   2000.0f

/* ── Module interface ───────────────────────────────────────── */
void billingMenu(void);
void generateBill(void);
void viewBill(void);
void updatePayment(void);
void printFinalBill(int billID);
void generateBillingReport(void);

/* ── File I/O ───────────────────────────────────────────────── */
void loadBills(void);
void saveBills(void);

/* ── Helpers ────────────────────────────────────────────────── */
int  getNextBillID(void);
int  getBillIndex(int billID);

/* ── Globals ────────────────────────────────────────────────── */
extern Bill bills[MAX_PATIENTS];
extern int  billCount;

#endif /* BILLING_H */
