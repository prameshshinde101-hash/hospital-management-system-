================================================================
  SMART HOSPITAL MANAGEMENT SYSTEM
  Project Documentation
  Language: C | Standard: C11 | Compiler: GCC
================================================================

TABLE OF CONTENTS
-----------------
1.  Project Overview
2.  System Architecture
3.  Module Descriptions
4.  Data Structures Used
5.  Algorithms Implemented
6.  File Handling Design
7.  Security Implementation
8.  User Interface Design
9.  Error Handling Strategy
10. Compilation & Deployment
11. Limitations & Future Scope

================================================================
1. PROJECT OVERVIEW
================================================================

The Smart Hospital Management System is a console-based
application built entirely in C that digitizes and automates
the core operations of a hospital. The system handles:

  - Patient registration, tracking, and discharge
  - Doctor profiles and patient assignment
  - Appointment booking with token generation
  - Emergency triage using a priority queue
  - Ambulance fleet dispatch and tracking
  - ICU and General Ward bed management
  - Itemised patient billing and payment tracking
  - Role-based secure login with encrypted passwords
  - Persistent binary file storage for all records
  - In-memory linked list for live activity log
  - Automated report generation for all modules
  - Backup and restore of all hospital data
  - Real-time statistics dashboard

================================================================
2. SYSTEM ARCHITECTURE
================================================================

  +----------------------------------------------------------+
  |                        main.c                            |
  |         (Entry Point, Main Menu, Dashboard)              |
  +--------+---------+----------+---------+------------------+
           |         |          |         |
  +--------+  +------+--+ +----+----+ +--+-------+
  | login  |  | patient | | doctor  | | appoint  |
  |  .c/.h |  |  .c/.h  | |  .c/.h  | |  .c/.h   |
  +--------+  +---------+ +---------+ +----------+

  +----------+ +---------+ +--------+ +----------+
  | emergency| |ambulance| |  bed   | | billing  |
  |   .c/.h  | |  .c/.h  | | .c/.h  | |  .c/.h   |
  +----------+ +---------+ +--------+ +----------+

  +--------------+    +-----------------------------+
  | linked_list  |    |         utility             |
  |    .c/.h     |    | .c/.h (shared by all above) |
  +--------------+    +-----------------------------+

  All modules write to:  data/*.dat  (binary files)
  All actions logged to: logs/activity.log
  Backups stored in:     backup/

DATA FLOW:
  User Input → Menu Selection → Module Function →
  Validate → Update In-Memory Array →
  Write to .dat File → Log Activity → Return to Menu

================================================================
3. MODULE DESCRIPTIONS
================================================================

--- 3.1 utility (.c/.h) ---
  Shared infrastructure used by all other modules.
  Provides: ANSI colour macros, printHeader(), printLine(),
  loadingBar(), clearScreen(), pauseScreen(), getIntInput(),
  getStrInput(), getFloatInput(), confirmAction(),
  getCurrentDateTime(), logActivity(), encryptStr(),
  decryptStr(), toUpperStr(), trimWhitespace(),
  backupData(), restoreData().

--- 3.2 login (.c/.h) ---
  Handles admin authentication.
  - Reads password without echo (POSIX termios)
  - Stores password as XOR-encrypted bytes in credentials.dat
  - Locks out after 3 failed attempts
  - Logs every login/logout event
  - createDefaultAdmin() creates admin/admin123 on first run

--- 3.3 patient (.c/.h) ---
  Core module for patient lifecycle management.
  - Stores up to MAX_PATIENTS (200) records in patients[]
  - sortPatientsByName(): Merge Sort — O(n log n)
  - sortPatientsByAge(): Bubble Sort — O(n²)
  - searchPatient(): Linear search by ID or partial name
  - Discharge sets isActive=0 (soft delete, data preserved)
  - File: data/patients.dat (binary)

--- 3.4 doctor (.c/.h) ---
  Manages doctor registry and patient assignments.
  - Stores up to MAX_DOCTORS (50) records in doctors[]
  - sortDoctorsBySpecialization(): Insertion Sort — O(n²)
  - searchDoctor(): by ID, name, or specialization
  - assignDoctorToPatient(): updates both records atomically
  - File: data/doctors.dat (binary)

--- 3.5 appointment (.c/.h) ---
  Appointment scheduling with auto-generated tokens.
  - Token = max existing token + 1 (unique, persistent)
  - 8 predefined time slots per day
  - Status enum: SCHEDULED → COMPLETED or CANCELLED
  - File: data/appointments.dat (binary)

--- 3.6 emergency (.c/.h) ---
  Emergency patient triage using a min-heap priority queue.
  - 4 priority levels: CRITICAL(1), HIGH(2), MEDIUM(3), LOW(4)
  - Min-heap ensures CRITICAL patients are always served first
  - pqInsert(): O(log n), pqExtractMin(): O(log n)
  - ASCII visualizer shows queue in priority order
  - File: data/emergency.dat (binary)

--- 3.7 ambulance (.c/.h) ---
  Ambulance fleet dispatch and tracking.
  - Status enum: AVAILABLE, DISPATCHED, MAINTENANCE
  - Trip counter increments on each dispatch
  - Stores driver name and phone per vehicle
  - File: data/ambulances.dat (binary)

--- 3.8 bed (.c/.h) ---
  Dual-type bed management system.
  - ICU beds numbered 1001–1020 (₹3000/day)
  - General beds numbered 2001–2100 (₹800/day)
  - allocateBed(): O(n) scan for first free bed
  - Visual grid showing FREE/OCC status per bed
  - File: data/beds.dat (binary)

--- 3.9 billing (.c/.h) ---
  Full itemised billing and payment tracking.
  - Components: consultation, bed, emergency, medicine,
                lab/diagnostics, miscellaneous
  - Specialist doctors billed at higher consultation rate
  - Status: PENDING → PARTIAL → PAID
  - Generates formatted invoice to console
  - File: data/billing.dat (binary)

--- 3.10 linked_list (.c/.h) ---
  Singly linked list for in-memory activity log management.
  - Node: entry text + timestamp + sequence number + *next
  - ll_insert(): O(1) append at tail (tail pointer maintained)
  - ll_display(): O(n) forward traversal
  - ll_displayReverse(): O(n) recursive reverse traversal
  - ll_search(): O(n) linear keyword search
  - ll_deleteHead(): O(1) remove oldest entry
  - Bounded to 200 entries; auto-trims on overflow
  - Loads last 100 lines from log file on startup

================================================================
4. DATA STRUCTURES
================================================================

(See DATASTRUCTURES.md for complete explanation)

Summary:
  Structure        | Kind              | Location
  -----------------|-------------------|------------------
  Patient          | Struct + Array    | patient.h
  Doctor           | Struct + Array    | doctor.h
  Appointment      | Struct + Array    | appointment.h
  EmergencyPatient | Struct (heap node)| emergency.h
  EmergencyQueue   | Min-Heap array    | emergency.h
  Ambulance        | Struct + Array    | ambulance.h
  Bill             | Struct + Array    | billing.h
  Bed              | Struct + Array    | bed.h
  Credential       | Struct (single)   | login.h
  LogNode          | Linked list node  | linked_list.h
  LogList          | Linked list head  | linked_list.h

================================================================
5. ALGORITHMS
================================================================

(See ALGORITHMS.md for complete explanation)

  Algorithm      | Location          | Time Complexity
  ---------------|-------------------|----------------
  Merge Sort     | patient.c         | O(n log n)
  Bubble Sort    | patient.c         | O(n²)
  Insertion Sort | doctor.c          | O(n²)
  Linear Search  | patient.c/doctor.c| O(n)
  Heap Insert    | emergency.c       | O(log n)
  Heap Extract   | emergency.c       | O(log n)
  XOR Cipher     | utility.c         | O(n)

================================================================
6. FILE HANDLING DESIGN
================================================================

All data files use BINARY format (fread/fwrite) for:
  - Speed: no parsing overhead
  - Compactness: exact struct size on disk
  - Integrity: no encoding issues

File layout:
  patients.dat   → [int count][Patient × count]
  doctors.dat    → [int count][Doctor × count]
  appointments.dat → [int count][Appointment × count]
  billing.dat    → [int count][Bill × count]
  ambulances.dat → [int count][Ambulance × count]
  beds.dat       → [Bed × MAX_ICU_BEDS][Bed × MAX_GEN_BEDS]
  emergency.dat  → [int idCounter][int size][EmergencyPatient × size]
  credentials.dat → [Credential × 1]

================================================================
7. SECURITY IMPLEMENTATION
================================================================

Password Security:
  - Input hidden using tcsetattr() (POSIX termios ECHO off)
  - Stored as XOR-encrypted bytes (key: 0x5A)
  - 3-attempt lockout on failed logins
  - Password change requires current password verification
  - Minimum password length enforced (6 characters)

Note: XOR cipher is a basic obfuscation suitable for this
academic project. Production systems use bcrypt or Argon2.

================================================================
8. USER INTERFACE DESIGN
================================================================

  - ANSI escape codes for 8-colour terminal output
  - Box-drawing characters for cards and panels
  - Loading bar animation on startup (usleep-based)
  - ASCII welcome screen with project title art
  - Status bar on main menu showing live counts
  - Colour coding: RED=danger/critical, GREEN=success/ok,
    YELLOW=warning/info, CYAN=headers, MAGENTA=doctors,
    WHITE=normal text

================================================================
9. ERROR HANDLING
================================================================

  - All fopen() calls checked for NULL before use
  - Integer input validated in range [lo, hi] with retry loop
  - String input null-terminated and whitespace-trimmed
  - Duplicate ID prevention via patientExists()/doctorExists()
  - Capacity checks before every array insertion
  - Soft-delete for patients (discharged flag) preserves history
  - malloc() NULL checked; failed allocation reports error
  - Binary file corruption: if count > MAX, it is clamped

================================================================
10. COMPILATION & DEPLOYMENT
================================================================

Requirements:
  - GCC 7+ on Linux or macOS
  - POSIX-compliant OS (for termios password hiding)

Build:
  cd hospital_management
  make          # build
  make run      # build + run
  make clean    # remove objects
  make cleanall # remove objects + data

First Run:
  - Directories data/, logs/, backup/ auto-created
  - Default admin: username=admin, password=admin123
  - Bed records initialised (ICU + General) automatically

================================================================
11. LIMITATIONS & FUTURE SCOPE
================================================================

Current Limitations:
  - Single admin user (no multi-user role system)
  - No network connectivity (standalone console app)
  - XOR encryption is weak (academic use only)
  - No date validation on appointment dates entered

Future Scope:
  - Multi-user roles: Admin, Doctor, Nurse, Receptionist
  - Replace XOR with SHA-256 or bcrypt password hashing
  - GUI frontend using GTK or Qt
  - Database backend (SQLite) replacing binary files
  - Network API layer for web/mobile frontend integration
  - SMS/email appointment reminders
  - Drug inventory management module
  - Lab test results management

================================================================
END OF PROJECT DOCUMENTATION
================================================================
