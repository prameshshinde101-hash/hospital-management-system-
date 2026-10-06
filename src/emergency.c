/* ============================================================
 *  emergency.c — Emergency handling with priority queue (min-heap)
 *  Smart Hospital Management System
 * ============================================================ */

#include "../include/emergency.h"

/* ── Globals ────────────────────────────────────────────────── */
EmergencyQueue emergQueue = { .size = 0 };
int            emergIDCounter = 3000;

/* ── File I/O ───────────────────────────────────────────────── */
void loadEmergency(void) {
    FILE *f = fopen(EMERG_FILE, "rb");
    if (!f) return;
    fread(&emergIDCounter, sizeof(int), 1, f);
    fread(&emergQueue.size, sizeof(int), 1, f);
    if (emergQueue.size > MAX_EMERG_QUEUE) emergQueue.size = MAX_EMERG_QUEUE;
    fread(emergQueue.data, sizeof(EmergencyPatient), emergQueue.size, f);
    fclose(f);
}

void saveEmergency(void) {
    FILE *f = fopen(EMERG_FILE, "wb");
    if (!f) { printf(RED "  Error saving emergency records.\n" RESET); return; }
    fwrite(&emergIDCounter, sizeof(int), 1, f);
    fwrite(&emergQueue.size, sizeof(int), 1, f);
    fwrite(emergQueue.data, sizeof(EmergencyPatient), emergQueue.size, f);
    fclose(f);
}

/* ── Priority Queue: min-heap by priority value ─────────────── */
/* Lower priority value = higher urgency (CRITICAL=1 is served first) */

void pqHeapifyUp(EmergencyQueue *pq, int i) {
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (pq->data[parent].priority > pq->data[i].priority) {
            EmergencyPatient tmp = pq->data[parent];
            pq->data[parent]    = pq->data[i];
            pq->data[i]         = tmp;
            i = parent;
        } else break;
    }
}

void pqHeapifyDown(EmergencyQueue *pq, int i) {
    int n = pq->size;
    while (1) {
        int smallest = i;
        int l = 2 * i + 1, r = 2 * i + 2;
        if (l < n && pq->data[l].priority < pq->data[smallest].priority) smallest = l;
        if (r < n && pq->data[r].priority < pq->data[smallest].priority) smallest = r;
        if (smallest == i) break;
        EmergencyPatient tmp = pq->data[smallest];
        pq->data[smallest]   = pq->data[i];
        pq->data[i]          = tmp;
        i = smallest;
    }
}

void pqInsert(EmergencyQueue *pq, EmergencyPatient ep) {
    if (pq->size >= MAX_EMERG_QUEUE) {
        printf(RED "  ✘  Emergency queue full!\n" RESET);
        return;
    }
    pq->data[pq->size++] = ep;
    pqHeapifyUp(pq, pq->size - 1);
}

EmergencyPatient pqExtractMin(EmergencyQueue *pq) {
    EmergencyPatient top = pq->data[0];
    pq->data[0] = pq->data[--pq->size];
    pqHeapifyDown(pq, 0);
    return top;
}

int pqIsEmpty(EmergencyQueue *pq) {
    return pq->size == 0;
}

/* ── Priority label helpers ─────────────────────────────────── */
static const char *priorityLabel(EmergencyPriority p) {
    switch (p) {
        case PRIORITY_CRITICAL: return RED    "● CRITICAL" RESET;
        case PRIORITY_HIGH:     return YELLOW "● HIGH    " RESET;
        case PRIORITY_MEDIUM:   return CYAN   "● MEDIUM  " RESET;
        case PRIORITY_LOW:      return GREEN  "● LOW     " RESET;
        default:                return WHITE  "● UNKNOWN " RESET;
    }
}

static const char *priorityName(EmergencyPriority p) {
    switch (p) {
        case PRIORITY_CRITICAL: return "CRITICAL";
        case PRIORITY_HIGH:     return "HIGH";
        case PRIORITY_MEDIUM:   return "MEDIUM";
        case PRIORITY_LOW:      return "LOW";
        default:                return "UNKNOWN";
    }
}

/* ── Register emergency patient ─────────────────────────────── */
void registerEmergency(void) {
    printHeader("EMERGENCY PATIENT REGISTRATION");

    if (emergQueue.size >= MAX_EMERG_QUEUE) {
        printf(RED "  ✘  Emergency queue full! Handle existing cases first.\n" RESET);
        pauseScreen();
        return;
    }

    EmergencyPatient ep;
    memset(&ep, 0, sizeof(EmergencyPatient));
    ep.emergID  = ++emergIDCounter;
    ep.isHandled = 0;

    printf(RED BOLD "  ⚠  EMERGENCY REGISTRATION  ⚠\n\n" RESET);

    /* Optional: link to existing patient */
    printf(WHITE "  Existing Patient ID (0 for walk-in): " RESET);
    char buf[20];
    fgets(buf, sizeof(buf), stdin);
    sscanf(buf, "%d", &ep.patientID);

    getStrInput("Patient Name", ep.patientName, MAX_NAME);
    ep.age = getIntInput("Age", 1, 120);
    getStrInput("Condition / Injury", ep.condition, MAX_DISEASE);

    printf(WHITE "\n  Select Priority Level:\n" RESET);
    printf("  " RED    "1. CRITICAL" RESET " - Cardiac arrest, severe trauma, unconscious\n");
    printf("  " YELLOW "2. HIGH    " RESET " - Severe pain, high fever, fractures\n");
    printf("  " CYAN   "3. MEDIUM  " RESET " - Moderate pain, stable vitals\n");
    printf("  " GREEN  "4. LOW     " RESET " - Minor injuries, non-urgent\n");
    int pChoice = getIntInput("Priority", 1, 4);
    ep.priority = (EmergencyPriority)pChoice;

    getCurrentDateTime(ep.arrivalTime, sizeof(ep.arrivalTime));

    pqInsert(&emergQueue, ep);
    saveEmergency();

    printf(GREEN "\n  ✔  Emergency registered! ID: %d | Priority: %s\n" RESET,
           ep.emergID, priorityName(ep.priority));
    printf(YELLOW "  Queue position: #%d\n" RESET, emergQueue.size);
    logActivity("Emergency patient registered");
    pauseScreen();
}

/* ── Handle (dequeue) next emergency ───────────────────────── */
void handleNextEmergency(void) {
    printHeader("HANDLE NEXT EMERGENCY");

    if (pqIsEmpty(&emergQueue)) {
        printf(GREEN "  ✔  No pending emergencies. Queue is clear!\n" RESET);
        pauseScreen();
        return;
    }

    EmergencyPatient ep = pqExtractMin(&emergQueue);
    ep.isHandled = 1;

    printf(RED BOLD "\n  ══ HANDLING EMERGENCY ══\n\n" RESET);
    printf(CYAN "  Emergency ID : %d\n" RESET, ep.emergID);
    printf(CYAN "  Patient Name : %s\n" RESET, ep.patientName);
    printf(CYAN "  Age          : %d\n" RESET, ep.age);
    printf(CYAN "  Condition    : %s\n" RESET, ep.condition);
    printf(CYAN "  Priority     : %s\n" RESET, priorityLabel(ep.priority));
    printf(CYAN "  Arrival Time : %s\n" RESET, ep.arrivalTime);
    printf(CYAN "  Remaining in queue: %d\n\n" RESET, emergQueue.size);

    /* Save the handled record to a log file */
    FILE *f = fopen("data/emergency_handled.txt", "a");
    if (f) {
        char dt[30];
        getCurrentDateTime(dt, sizeof(dt));
        fprintf(f, "[%s] ID:%d | %s | Age:%d | %s | Priority:%s | Handled\n",
                dt, ep.emergID, ep.patientName, ep.age,
                ep.condition, priorityName(ep.priority));
        fclose(f);
    }

    saveEmergency();
    logActivity("Emergency patient handled");
    pauseScreen();
}

/* ── Display queue ──────────────────────────────────────────── */
void displayEmergencyQueue(void) {
    printHeader("EMERGENCY QUEUE STATUS");

    if (pqIsEmpty(&emergQueue)) {
        printf(GREEN "  ✔  Emergency queue is empty. All clear!\n" RESET);
        pauseScreen();
        return;
    }

    printf(CYAN "  %-6s %-25s %-5s %-12s %-10s\n" RESET,
           "EmID", "Name", "Age", "Priority", "Arrival");
    printLine('-', 72);

    /* Display heap contents (not perfectly sorted but shows what's queued) */
    for (int i = 0; i < emergQueue.size; i++) {
        EmergencyPatient *ep = &emergQueue.data[i];
        printf("  %-6d %-25s %-5d %-20s %s\n",
               ep->emergID, ep->patientName, ep->age,
               priorityLabel(ep->priority), ep->arrivalTime);
    }
    printLine('-', 72);
    printf(YELLOW "  Total in queue: %d\n" RESET, emergQueue.size);
    pauseScreen();
}

/* ── ASCII queue visualizer ─────────────────────────────────── */
void visualizeQueue(void) {
    printHeader("EMERGENCY QUEUE VISUALIZATION");

    if (pqIsEmpty(&emergQueue)) {
        printf(GREEN "  Queue is empty.\n" RESET);
        pauseScreen();
        return;
    }

    /* Make a sorted copy for display */
    EmergencyQueue temp = emergQueue;
    EmergencyPatient sorted[MAX_EMERG_QUEUE];
    int n = 0;
    while (!pqIsEmpty(&temp)) {
        sorted[n++] = pqExtractMin(&temp);
    }

    printf(WHITE "  Priority order (next to be handled first):\n\n" RESET);
    printf("  FRONT ─── QUEUE ──────────────────────────────► REAR\n\n");

    for (int i = 0; i < n; i++) {
        EmergencyPatient *ep = &sorted[i];
        const char *colour =
            ep->priority == PRIORITY_CRITICAL ? RED    :
            ep->priority == PRIORITY_HIGH     ? YELLOW :
            ep->priority == PRIORITY_MEDIUM   ? CYAN   : GREEN;

        printf("  %s┌──────────────────────┐\n" RESET, colour);
        printf("  %s│ #%-3d %-17s│\n" RESET, colour, i+1, ep->patientName);
        printf("  %s│ %-22s│\n" RESET, colour, priorityName(ep->priority));
        printf("  %s└──────────────────────┘\n" RESET, colour);
        if (i < n - 1) printf("              │\n              ▼\n");
    }

    printf("\n");
    pauseScreen();
}

/* ── Emergency report ───────────────────────────────────────── */
void generateEmergencyReport(void) {
    printHeader("EMERGENCY REPORT");

    FILE *f = fopen("data/emergency_report.txt", "w");
    char dt[30];
    getCurrentDateTime(dt, sizeof(dt));

    if (f) {
        fprintf(f, "====== EMERGENCY REPORT ======\nGenerated: %s\n\n", dt);
        fprintf(f, "Current Queue Size: %d\n\n", emergQueue.size);
        int counts[5] = {0};
        for (int i = 0; i < emergQueue.size; i++)
            counts[(int)emergQueue.data[i].priority]++;
        fprintf(f, "Critical: %d | High: %d | Medium: %d | Low: %d\n",
                counts[1], counts[2], counts[3], counts[4]);

        fprintf(f, "\n--- Active Emergency Queue ---\n");
        for (int i = 0; i < emergQueue.size; i++) {
            EmergencyPatient *ep = &emergQueue.data[i];
            fprintf(f, "ID: %d | %s | Age: %d | %s | Priority: %s | %s\n",
                    ep->emergID, ep->patientName, ep->age,
                    ep->condition, priorityName(ep->priority), ep->arrivalTime);
        }
        fclose(f);
        printf(GREEN "  ✔  Emergency report saved.\n" RESET);
    }

    displayEmergencyQueue();
}

/* ── Emergency menu ─────────────────────────────────────────── */
void emergencyMenu(void) {
    int choice;
    do {
        printHeader("EMERGENCY MANAGEMENT");
        printf(WHITE "  1.  Register Emergency Patient\n");
        printf("  2.  Handle Next Emergency (Dequeue)\n");
        printf("  3.  View Emergency Queue\n");
        printf("  4.  Visualize Queue (ASCII)\n");
        printf("  5.  Generate Emergency Report\n");
        printf(YELLOW "  0.  Back to Main Menu\n" RESET);
        choice = getIntInput("Select option", 0, 5);
        switch (choice) {
            case 1: registerEmergency();      break;
            case 2: handleNextEmergency();    break;
            case 3: displayEmergencyQueue();  break;
            case 4: visualizeQueue();         break;
            case 5: generateEmergencyReport();break;
        }
    } while (choice != 0);
}
