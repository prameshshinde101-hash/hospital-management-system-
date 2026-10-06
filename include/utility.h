#ifndef UTILITY_H
#define UTILITY_H

#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE 1

/* ============================================================
 *  utility.h — Common macros, colours, and helper prototypes
 *  Smart Hospital Management System
 * ============================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

/* ── ANSI colour codes ──────────────────────────────────────── */
#define RESET       "\033[0m"
#define BOLD        "\033[1m"
#define RED         "\033[1;31m"
#define GREEN       "\033[1;32m"
#define YELLOW      "\033[1;33m"
#define BLUE        "\033[1;34m"
#define MAGENTA     "\033[1;35m"
#define CYAN        "\033[1;36m"
#define WHITE       "\033[1;37m"
#define BG_BLUE     "\033[44m"
#define BG_RED      "\033[41m"
#define BG_GREEN    "\033[42m"

/* ── Path constants ─────────────────────────────────────────── */
#define DATA_DIR        "data/"
#define LOG_DIR         "logs/"
#define BACKUP_DIR      "backup/"

#define PATIENT_FILE    DATA_DIR "patients.dat"
#define DOCTOR_FILE     DATA_DIR "doctors.dat"
#define APPT_FILE       DATA_DIR "appointments.dat"
#define BILL_FILE       DATA_DIR "billing.dat"
#define AMBULANCE_FILE  DATA_DIR "ambulances.dat"
#define BED_FILE        DATA_DIR "beds.dat"
#define CRED_FILE       DATA_DIR "credentials.dat"
#define LOG_FILE        LOG_DIR  "activity.log"
#define EMERG_FILE      DATA_DIR "emergency.dat"

/* ── Field-size limits ──────────────────────────────────────── */
#define MAX_NAME        60
#define MAX_ADDR        120
#define MAX_PHONE       15
#define MAX_BLOOD       5
#define MAX_DISEASE     80
#define MAX_SPEC        60
#define MAX_PASS        32
#define MAX_USER        20

/* ── Capacity limits ────────────────────────────────────────── */
#define MAX_PATIENTS    200
#define MAX_DOCTORS     50
#define MAX_APPTS       300
#define MAX_AMBULANCES  20
#define MAX_ICU_BEDS    20
#define MAX_GEN_BEDS    100
#define MAX_EMERG_QUEUE 50

/* ── Function prototypes ────────────────────────────────────── */
void  clearScreen(void);
void  pauseScreen(void);
void  printHeader(const char *title);
void  printLine(char ch, int len);
void  printBoxed(const char *msg, const char *colour);
void  loadingBar(const char *msg, int steps);
void  welcomeScreen(void);
void  getCurrentDateTime(char *buf, int size);
void  logActivity(const char *action);
void  toUpperStr(char *s);
void  trimWhitespace(char *s);
int   confirmAction(const char *prompt);
int   getIntInput(const char *prompt, int lo, int hi);
float getFloatInput(const char *prompt, float lo, float hi);
void  getStrInput(const char *prompt, char *dest, int maxLen);
void  encryptStr(char *s);
void  decryptStr(char *s);
void  backupData(void);
void  restoreData(void);
void  showStatsDashboard(void);

#endif /* UTILITY_H */
