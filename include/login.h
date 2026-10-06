#ifndef LOGIN_H
#define LOGIN_H

/* ============================================================
 *  login.h — Authentication system prototypes
 *  Smart Hospital Management System
 * ============================================================ */

#include "utility.h"

/* ── Credential record ──────────────────────────────────────── */
typedef struct {
    char username[MAX_USER];
    char passwordHash[MAX_PASS];   /* XOR-encrypted */
    int  role;                     /* 0 = admin, 1 = staff */
    char lastLogin[20];
} Credential;

/* ── Module interface ───────────────────────────────────────── */
int  loginSystem(void);
void changePassword(void);
void createDefaultAdmin(void);
void readPassword(char *buf, int maxLen);

/* ── File I/O ───────────────────────────────────────────────── */
void loadCredentials(void);
void saveCredentials(void);

/* ── Globals ────────────────────────────────────────────────── */
extern Credential adminCred;
extern int        isLoggedIn;

#endif /* LOGIN_H */
