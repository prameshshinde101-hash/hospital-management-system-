================================================================
  SMART HOSPITAL MANAGEMENT SYSTEM
  Data Structure Explanation
================================================================

This document explains every data structure used in the project,
why it was chosen, and how it operates.

================================================================
1. STRUCTURES (typedef struct)
================================================================

All records are defined as C structures. A structure groups
related fields of different types under one name.

--- 1.1 Patient ---
typedef struct {
    int   patientID;          // Unique identifier
    char  name[60];           // Patient full name
    int   age;                // Age in years
    char  gender;             // 'M', 'F', or 'O'
    char  bloodGroup[5];      // e.g. "A+", "O-"
    char  disease[80];        // Diagnosis / condition
    char  phone[15];          // Contact number
    char  address[120];       // Residential address
    int   assignedDoctorID;   // Foreign key → Doctor
    int   bedNumber;          // 0 = no bed allocated
    char  admitDate[20];      // Admission date-time
    int   isActive;           // 1=admitted, 0=discharged
} Patient;

WHY: Groups all patient attributes. isActive enables soft
delete — discharged patients remain for billing history.

--- 1.2 Doctor ---
typedef struct {
    int  doctorID;
    char name[60];
    char specialization[60];
    char phone[15];
    int  experience;         // Years of practice
    int  isAvailable;        // 1 = available for patients
    int  patientCount;       // Current assigned patients
} Doctor;

WHY: patientCount helps load-balance patient assignment.

--- 1.3 Appointment ---
typedef struct {
    int        tokenNumber;   // Auto-generated unique token
    int        patientID;
    int        doctorID;
    char       date[20];
    char       timeSlot[10];
    ApptStatus status;        // SCHEDULED/COMPLETED/CANCELLED
    char       notes[120];
} Appointment;

WHY: tokenNumber lets patients track their slot easily.
Status enum models appointment lifecycle transitions.

--- 1.4 EmergencyPatient ---
typedef struct {
    int               emergID;
    int               patientID;      // 0 = walk-in
    char              patientName[60];
    int               age;
    char              condition[80];
    EmergencyPriority priority;       // 1=CRITICAL to 4=LOW
    char              arrivalTime[20];
    int               isHandled;
} EmergencyPatient;

WHY: This is the node element stored inside the heap.
Priority (int 1–4) is the heap key — lower = more urgent.

--- 1.5 EmergencyQueue (Min-Heap) ---
typedef struct {
    EmergencyPatient data[MAX_EMERG_QUEUE];  // Heap array
    int              size;                    // Current count
} EmergencyQueue;

WHY: An array-based binary min-heap.
  - Parent of node i  → (i-1)/2
  - Left child of i   → 2i+1
  - Right child of i  → 2i+2
  - Min-heap property: parent.priority ≤ child.priority
  - This means priority=1 (CRITICAL) bubbles to root → served first.

--- 1.6 Ambulance ---
typedef struct {
    int       ambID;
    char      vehicleNumber[20];
    char      driverName[60];
    char      driverPhone[15];
    AmbStatus status;          // AVAILABLE/DISPATCHED/MAINTENANCE
    int       assignedPatientID;
    char      dispatchTime[20];
    int       totalTrips;      // Odometer equivalent
} Ambulance;

--- 1.7 Bill ---
typedef struct {
    int        billID;
    int        patientID;
    char       patientName[60];
    char       billDate[20];
    float      consultationFee;
    float      bedCharges;
    int        daysAdmitted;
    float      emergencyCharges;
    float      medicineCharges;
    float      labCharges;
    float      miscCharges;
    float      totalAmount;
    float      amountPaid;
    BillStatus status;         // PENDING/PARTIAL/PAID
    char       notes[120];
} Bill;

WHY: Each charge component is stored separately so the
itemised invoice can show a breakdown. amountPaid enables
partial payment tracking.

--- 1.8 Bed ---
typedef struct {
    int       bedNumber;
    BedType   type;               // BED_ICU or BED_GENERAL
    BedStatus status;             // BED_AVAILABLE or BED_OCCUPIED
    int       assignedPatientID;
    char      allocatedDate[20];
} Bed;

WHY: BedType enables different billing rates. Bed numbers
follow a range convention (ICU: 1001–1020, General: 2001–2100)
so the type can be inferred from the number alone.

--- 1.9 Credential ---
typedef struct {
    char username[20];
    char passwordHash[32];   // XOR-encrypted bytes
    int  role;               // 0=admin, 1=staff
    char lastLogin[20];
} Credential;

WHY: Separating credentials from patient/doctor data
follows the principle of least privilege.

================================================================
2. ARRAYS (Static / Fixed-Size)
================================================================

Global arrays store all records in RAM during runtime:

  Patient   patients[200]        — max 200 patients
  Doctor    doctors[50]          — max 50 doctors
  Appointment appointments[300]  — max 300 appointments
  Bill      bills[200]           — max 200 bills
  Ambulance ambulances[20]       — max 20 ambulances
  Bed       icuBeds[20]          — 20 ICU beds
  Bed       genBeds[100]         — 100 General beds

WHY arrays:
  + O(1) random access by index
  + Simple to serialise with fwrite()
  + No pointer overhead
  - Fixed size (acceptable for hospital scale in this project)

================================================================
3. SINGLY LINKED LIST (linked_list.c)
================================================================

Structure:
  [HEAD] → [Node1] → [Node2] → ... → [NodeN] → NULL
                                                  ↑
  [TAIL] ────────────────────────────────────────┘

Node structure:
  typedef struct LogNode {
      char            entry[256];   // Log message
      char            timestamp[30];
      int             seqNo;        // Sequence counter
      struct LogNode *next;         // Pointer to next node
  } LogNode;

Container:
  typedef struct {
      LogNode *head;    // Points to oldest entry
      LogNode *tail;    // Points to newest entry (O(1) insert)
      int      count;
  } LogList;

Operations:
  ll_insert()      — O(1) append at tail
  ll_deleteHead()  — O(1) remove from head (oldest first)
  ll_display()     — O(n) forward traversal
  ll_displayReverse() — O(n) recursive reverse traversal
  ll_search()      — O(n) linear keyword search
  ll_freeAll()     — O(n) release all heap memory

WHY linked list for activity log:
  + Dynamic size — no fixed capacity
  + O(1) insert at tail (constant regardless of log size)
  + O(1) delete at head (pruning oldest entries)
  + Natural ordering — insertion order = time order
  + Demonstrates pointer manipulation and heap allocation

================================================================
4. MIN-HEAP (Priority Queue) — emergency.c
================================================================

The EmergencyQueue uses an array-based binary min-heap.

Heap property: data[parent].priority ≤ data[child].priority

Visual example (after inserting CRITICAL=1, HIGH=2, MEDIUM=3):

             [CRITICAL:1]           ← ROOT (served first)
            /             \
        [HIGH:2]        [MEDIUM:3]

Array layout: [1, 2, 3]

Index relationships:
  parent(i)      = (i-1) / 2
  leftChild(i)   = 2*i + 1
  rightChild(i)  = 2*i + 2

pqInsert():
  1. Place new element at end of array
  2. heapifyUp: swap with parent while parent > child
  Time: O(log n)

pqExtractMin():
  1. Save root (minimum priority = most critical)
  2. Move last element to root
  3. heapifyDown: swap with smallest child while violated
  Time: O(log n)

WHY min-heap:
  + Always O(log n) insert and extract (vs O(n) for sorted list)
  + In-place — no extra memory beyond the array
  + Guarantees CRITICAL patients always served before HIGH, etc.

================================================================
5. ENUMERATIONS
================================================================

  ApptStatus  { SCHEDULED=0, COMPLETED, CANCELLED }
  BillStatus  { PENDING=0, PAID, PARTIAL }
  AmbStatus   { AVAILABLE=0, DISPATCHED, MAINTENANCE }
  BedType     { ICU=0, GENERAL }
  BedStatus   { AVAILABLE=0, OCCUPIED }
  EmergencyPriority { CRITICAL=1, HIGH=2, MEDIUM=3, LOW=4 }

WHY enums: Named constants make code readable and prevent
magic number bugs. Each enum models a finite state machine.

================================================================
END OF DATA STRUCTURES DOCUMENT
================================================================
