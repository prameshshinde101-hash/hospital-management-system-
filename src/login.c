/* ============================================================
 *  login.c — Authentication system
 *  Smart Hospital Management System
 * ============================================================ */

#include "../include/login.h"
#include <termios.h>
#include <unistd.h>

/* ── Globals ────────────────────────────────────────────────── */
Credential adminCred;
int        isLoggedIn = 0;

/* ── File operations ────────────────────────────────────────── */
void loadCredentials(void) {
    FILE *f = fopen(CRED_FILE, "rb");
    if (!f) {
        createDefaultAdmin();
        return;
    }
    fread(&adminCred, sizeof(Credential), 1, f);
    fclose(f);
}

void saveCredentials(void) {
    FILE *f = fopen(CRED_FILE, "wb");
    if (!f) { printf(RED "  Error saving credentials.\n" RESET); return; }
    fwrite(&adminCred, sizeof(Credential), 1, f);
    fclose(f);
}

/* ── Create default admin on first run ──────────────────────── */
void createDefaultAdmin(void) {
    strncpy(adminCred.username, "admin", MAX_USER - 1);
    char pass[MAX_PASS];
    strncpy(pass, "admin123", MAX_PASS - 1);
    encryptStr(pass);
    strncpy(adminCred.passwordHash, pass, MAX_PASS - 1);
    adminCred.role = 0;
    getCurrentDateTime(adminCred.lastLogin, sizeof(adminCred.lastLogin));
    saveCredentials();
    printf(GREEN "  Default admin created. Username: admin | Password: admin123\n" RESET);
}

/* ── Read password without echo ─────────────────────────────── */
void readPassword(char *buf, int maxLen) {
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    int i = 0;
    int ch;
    printf(WHITE "  Password: " RESET);
    while (i < maxLen - 1 && (ch = getchar()) != '\n' && ch != EOF) {
        buf[i++] = (char)ch;
        putchar('*');
        fflush(stdout);
    }
    buf[i] = '\0';
    putchar('\n');

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

/* ── Main login function ─────────────────────────────────────── */
int loginSystem(void) {
    clearScreen();
    printf(CYAN);
    printLine('=', 50);
    printf("|%*s" YELLOW "HOSPITAL MANAGEMENT SYSTEM" CYAN "%*s|\n", 12, "", 12, "");
    printf("|%*s" WHITE "     Admin Login Portal       " CYAN "%*s|\n", 10, "", 10, "");
    printLine('=', 50);
    printf(RESET "\n");

    loadCredentials();

    char username[MAX_USER];
    char password[MAX_PASS];

    int attempts = 0;
    while (attempts < 3) {
        getStrInput("Username", username, MAX_USER);
        readPassword(password, MAX_PASS);

        /* Encrypt the entered password for comparison */
        char encPass[MAX_PASS];
        strncpy(encPass, password, MAX_PASS - 1);
        encPass[MAX_PASS - 1] = '\0';
        encryptStr(encPass);

        if (strcmp(username, adminCred.username) == 0 &&
            strcmp(encPass, adminCred.passwordHash) == 0) {
            printf(GREEN "\n  ✔  Login successful! Welcome, %s.\n" RESET, username);
            getCurrentDateTime(adminCred.lastLogin, sizeof(adminCred.lastLogin));
            saveCredentials();
            logActivity("Admin logged in");
            isLoggedIn = 1;
            usleep(800000);
            return 1;
        }

        attempts++;
        printf(RED "\n  ✘  Invalid credentials. Attempt %d/3.\n\n" RESET, attempts);
    }

    printf(RED "  System locked. Too many failed attempts.\n" RESET);
    logActivity("FAILED LOGIN - 3 attempts");
    return 0;
}

/* ── Change password ────────────────────────────────────────── */
void changePassword(void) {
    printHeader("CHANGE PASSWORD");

    char oldPass[MAX_PASS], newPass[MAX_PASS], confirmPass[MAX_PASS];

    readPassword(oldPass, MAX_PASS);
    char encOld[MAX_PASS];
    strncpy(encOld, oldPass, MAX_PASS - 1);
    encOld[MAX_PASS - 1] = '\0';
    encryptStr(encOld);

    if (strcmp(encOld, adminCred.passwordHash) != 0) {
        printf(RED "\n  ✘  Current password incorrect.\n" RESET);
        pauseScreen();
        return;
    }

    printf(WHITE "  New ");
    readPassword(newPass, MAX_PASS);
    printf(WHITE "  Confirm ");
    readPassword(confirmPass, MAX_PASS);

    if (strcmp(newPass, confirmPass) != 0) {
        printf(RED "\n  ✘  Passwords do not match.\n" RESET);
        pauseScreen();
        return;
    }
    if (strlen(newPass) < 6) {
        printf(RED "\n  ✘  Password must be at least 6 characters.\n" RESET);
        pauseScreen();
        return;
    }

    strncpy(adminCred.passwordHash, newPass, MAX_PASS - 1);
    encryptStr(adminCred.passwordHash);
    saveCredentials();
    printf(GREEN "\n  ✔  Password changed successfully.\n" RESET);
    logActivity("Password changed");
    pauseScreen();
}
