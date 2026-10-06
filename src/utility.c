/* ============================================================
 *  utility.c — Common helper functions
 *  Smart Hospital Management System
 * ============================================================ */

#include "../include/utility.h"
#include <unistd.h>
#include <termios.h>

/* ── Screen helpers ─────────────────────────────────────────── */
void clearScreen(void) {
    printf("\033[2J\033[1;1H");
}

void pauseScreen(void) {
    printf(YELLOW "\n  Press Enter to continue..." RESET);
    while (getchar() != '\n');
    getchar();
}

void printLine(char ch, int len) {
    for (int i = 0; i < len; i++) putchar(ch);
    putchar('\n');
}

void printHeader(const char *title) {
    int width = 70;
    clearScreen();
    printf(CYAN);
    printLine('=', width);
    int pad = (width - (int)strlen(title) - 2) / 2;
    printf("|%*s%s%*s|\n", pad, "", title, width - pad - (int)strlen(title) - 2, "");
    printLine('=', width);
    printf(RESET);
}

void printBoxed(const char *msg, const char *colour) {
    int len = (int)strlen(msg) + 4;
    printf("%s", colour);
    printLine('-', len);
    printf("| %s |\n", msg);
    printLine('-', len);
    printf(RESET);
}

/* ── Loading bar animation ──────────────────────────────────── */
void loadingBar(const char *msg, int steps) {
    printf(GREEN "\n  %s\n  [", msg);
    fflush(stdout);
    for (int i = 0; i < steps; i++) {
        printf("█");
        fflush(stdout);
        usleep(40000);
    }
    printf("] Done!\n" RESET);
    usleep(300000);
}

/* ── Welcome screen ─────────────────────────────────────────── */
void welcomeScreen(void) {
    clearScreen();
    printf(CYAN);
    printLine('*', 70);
    printf(BOLD);
    printf("*%68s*\n", "");
    printf("*%*s%-*s*\n", 10, "", 58, "   ██╗  ██╗ ██████╗ ███████╗██████╗ ██╗████████╗ █████╗ ██╗");
    printf("*%*s%-*s*\n", 10, "", 58, "   ██║  ██║██╔═══██╗██╔════╝██╔══██╗██║╚══██╔══╝██╔══██╗██║");
    printf("*%*s%-*s*\n", 10, "", 58, "   ███████║██║   ██║███████╗██████╔╝██║   ██║   ███████║██║");
    printf("*%*s%-*s*\n", 10, "", 58, "   ██╔══██║██║   ██║╚════██║██╔═══╝ ██║   ██║   ██╔══██║██║");
    printf("*%*s%-*s*\n", 10, "", 58, "   ██║  ██║╚██████╔╝███████║██║     ██║   ██║   ██║  ██║███████╗");
    printf(RESET CYAN);
    printf("*%68s*\n", "");
    printf("*%*s" YELLOW "SMART HOSPITAL MANAGEMENT SYSTEM" CYAN "%*s*\n", 19, "", 19, "");
    printf("*%*s" WHITE "Version 1.0  |  Built in C  |  2025" CYAN "%*s*\n", 17, "", 17, "");
    printf("*%68s*\n", "");
    printLine('*', 70);
    printf(RESET);
    loadingBar("Initializing system modules", 30);
}

/* ── Date/time ──────────────────────────────────────────────── */
void getCurrentDateTime(char *buf, int size) {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    strftime(buf, size, "%d-%m-%Y %H:%M:%S", tm);
}

/* ── Activity logger ────────────────────────────────────────── */
void logActivity(const char *action) {
    FILE *f = fopen(LOG_FILE, "a");
    if (!f) return;
    char dt[30];
    getCurrentDateTime(dt, sizeof(dt));
    fprintf(f, "[%s] %s\n", dt, action);
    fclose(f);
}

/* ── String utilities ───────────────────────────────────────── */
void toUpperStr(char *s) {
    for (; *s; s++) *s = (char)toupper((unsigned char)*s);
}

void trimWhitespace(char *s) {
    char *end;
    while (isspace((unsigned char)*s)) s++;
    if (*s == '\0') return;
    end = s + strlen(s) - 1;
    while (end > s && isspace((unsigned char)*end)) end--;
    *(end + 1) = '\0';
}

/* ── XOR encrypt/decrypt (simple obfuscation) ───────────────── */
void encryptStr(char *s) {
    unsigned char key = 0x5A;
    for (; *s; s++) *s ^= key;
}

void decryptStr(char *s) {
    encryptStr(s); /* XOR is symmetric */
}

/* ── Confirm action (Y/N) ───────────────────────────────────── */
int confirmAction(const char *prompt) {
    char c;
    printf(YELLOW "  %s (y/n): " RESET, prompt);
    scanf(" %c", &c);
    while (getchar() != '\n');
    return (tolower(c) == 'y');
}

/* ── Safe integer input with range check ────────────────────── */
int getIntInput(const char *prompt, int lo, int hi) {
    int val;
    char buf[64];
    while (1) {
        printf(WHITE "  %s: " RESET, prompt);
        if (fgets(buf, sizeof(buf), stdin)) {
            if (sscanf(buf, "%d", &val) == 1 && val >= lo && val <= hi)
                return val;
        }
        printf(RED "  Invalid! Enter a number between %d and %d.\n" RESET, lo, hi);
    }
}

/* ── Safe float input ───────────────────────────────────────── */
float getFloatInput(const char *prompt, float lo, float hi) {
    float val;
    char buf[64];
    while (1) {
        printf(WHITE "  %s: " RESET, prompt);
        if (fgets(buf, sizeof(buf), stdin)) {
            if (sscanf(buf, "%f", &val) == 1 && val >= lo && val <= hi)
                return val;
        }
        printf(RED "  Invalid! Enter a value between %.2f and %.2f.\n" RESET, lo, hi);
    }
}

/* ── Safe string input ──────────────────────────────────────── */
void getStrInput(const char *prompt, char *dest, int maxLen) {
    printf(WHITE "  %s: " RESET, prompt);
    if (fgets(dest, maxLen, stdin)) {
        dest[strcspn(dest, "\n")] = '\0';
        trimWhitespace(dest);
    }
}

/* ── Hide password while typing (POSIX termios) ─────────────── */
void readPasswordHidden(char *buf, int maxLen) {
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    int i = 0;
    int ch;
    while (i < maxLen - 1 && (ch = getchar()) != '\n' && ch != EOF) {
        buf[i++] = (char)ch;
        putchar('*');
        fflush(stdout);
    }
    buf[i] = '\0';
    putchar('\n');

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

/* ── Backup data directory ──────────────────────────────────── */
void backupData(void) {
    printHeader("BACKUP DATA");
    char dt[30], cmd[200];
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    strftime(dt, sizeof(dt), "%Y%m%d_%H%M%S", tm);
    snprintf(cmd, sizeof(cmd), "cp -r %s %sbackup_%s/ 2>/dev/null", DATA_DIR, BACKUP_DIR, dt);
    if (system(cmd) == 0) {
        printf(GREEN "\n  ✔  Backup created: backup_%s\n" RESET, dt);
        logActivity("Data backup created");
    } else {
        printf(RED "\n  ✘  Backup failed.\n" RESET);
    }
    pauseScreen();
}

/* ── Restore latest backup ──────────────────────────────────── */
void restoreData(void) {
    printHeader("RESTORE DATA");
    if (!confirmAction("This will overwrite current data. Confirm restore")) return;
    char cmd[200];
    snprintf(cmd, sizeof(cmd), "ls -dt %sbackup_*/ 2>/dev/null | head -1 | xargs -I{} cp -r {}. %s", BACKUP_DIR, DATA_DIR);
    if (system(cmd) == 0) {
        printf(GREEN "\n  ✔  Latest backup restored.\n" RESET);
        logActivity("Data restored from backup");
    } else {
        printf(RED "\n  ✘  Restore failed or no backup found.\n" RESET);
    }
    pauseScreen();
}

/* ── Statistics dashboard ───────────────────────────────────── */
void showStatsDashboard(void) {
    /* Implemented in main.c after all globals are available */
    printf(YELLOW "  (Statistics available from main menu)\n" RESET);
}
