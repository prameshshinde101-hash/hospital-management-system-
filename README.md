# 🏥 Smart Hospital Management System
### A Complete Console-Based C Project | 4-Credit Engineering Course

---

## 📋 Project Overview

The **Smart Hospital Management System** is a fully menu-driven, modular hospital administration application written in **pure C**. It covers patient management, doctor scheduling, emergency handling, billing, bed allocation, ambulance dispatch, and more — all backed by file-based persistent storage.

---

## 🗂️ Project Structure

```
hospital_management/
├── include/                 # Header files
│   ├── utility.h            # Shared macros, colours, prototypes
│   ├── patient.h            # Patient structure & prototypes
│   ├── doctor.h             # Doctor structure & prototypes
│   ├── appointment.h        # Appointment structure & prototypes
│   ├── emergency.h          # Emergency priority queue
│   ├── ambulance.h          # Ambulance fleet management
│   ├── billing.h            # Billing & payment system
│   ├── login.h              # Authentication system
│   └── bed.h                # ICU & General bed management
│
├── src/                     # Source files
│   ├── main.c               # Entry point, main menu, dashboard
│   ├── utility.c            # Helper functions, logger, I/O
│   ├── login.c              # Login & password change
│   ├── patient.c            # Patient CRUD + sort/search
│   ├── doctor.c             # Doctor CRUD + assignment
│   ├── appointment.c        # Appointment booking & tokens
│   ├── emergency.c          # Priority queue + visualization
│   ├── ambulance.c          # Fleet dispatch & tracking
│   ├── billing.c            # Bill generation & payment
│   └── bed.c                # Bed allocation & release
│
├── data/                    # Persistent binary data files
│   ├── patients.dat
│   ├── doctors.dat
│   ├── appointments.dat
│   ├── billing.dat
│   ├── ambulances.dat
│   ├── beds.dat
│   ├── emergency.dat
│   └── credentials.dat
│
├── logs/                    # Activity log
│   └── activity.log
│
├── backup/                  # Auto-backup directory
├── Makefile
└── README.md
```

---

## ⚙️ Compilation & Running

### Prerequisites
- GCC compiler (version 7+)
- Linux/macOS (uses POSIX termios for password hiding)

### Build & Run
```bash
# Clone or extract project folder
cd hospital_management

# Build
make

# Run
./hospital

# Or build + run in one step
make run

# Clean build artifacts
make clean

# Full clean (including data)
make cleanall
```

### Manual Compilation (without Make)
```bash
gcc -Wall -std=c11 -Iinclude \
    src/main.c src/utility.c src/login.c src/patient.c \
    src/doctor.c src/appointment.c src/emergency.c \
    src/ambulance.c src/billing.c src/bed.c \
    -o hospital
mkdir -p data logs backup
./hospital
```

---

## 🔐 Default Login Credentials

| Field    | Value      |
|----------|------------|
| Username | `admin`    |
| Password | `admin123` |

> Change the password immediately after first login via **Settings → Change Password**.

---

## 🧩 Modules & Features

### 1. Patient Management
- Add, update, discharge patients
- Search by ID or name (partial match)
- Sort by name (Merge Sort) or age (Bubble Sort)
- Stores: ID, name, age, gender, blood group, disease, contact, address, admitted date

### 2. Doctor Management
- Add/view/search doctors
- Assign doctor to patient (updates patient record)
- Toggle availability status
- Sort by specialization (Insertion Sort)

### 3. Appointment Management
- Book appointments with auto-generated token numbers
- Time slot selection (8 slots available)
- Cancel or mark complete
- Filter view: All / Scheduled / Completed / Cancelled

### 4. Emergency Handling (Priority Queue)
- Register emergency patients with priority: CRITICAL / HIGH / MEDIUM / LOW
- Min-heap implementation (lower number = served first)
- Handle next emergency (dequeue)
- ASCII visualization of the entire queue

### 5. Ambulance Management
- Add ambulances with driver details
- Dispatch to patients (marks as Dispatched)
- Release back to fleet
- Fleet status dashboard

### 6. Bed Management
- ICU beds: 1001–1020 (₹3000/day)
- General Ward beds: 2001–2100 (₹800/day)
- Visual occupancy display
- Allocate to patient / release

### 7. Billing System
- Generate itemised bills: consultation + bed + emergency + medicine + lab + misc
- Auto-compute totals
- Track partial payments
- Bill statuses: Pending / Partial / Paid

### 8. Login System
- XOR-encrypted password storage
- Password hidden during entry (termios)
- 3-attempt lockout on failure
- Password change with old password verification

### 9. Reports
- Patient report → `data/patient_report.txt`
- Doctor report → `data/doctor_report.txt`
- Appointment report → `data/appointment_report.txt`
- Emergency report → `data/emergency_report.txt`
- Billing report → `data/billing_report.txt`

### 10. Statistics Dashboard
- Live counts: patients, doctors, appointments, emergency queue
- Bed occupancy with ASCII bar charts
- Financial summary: collected vs pending revenue

### 11. Advanced Features
- **Activity Log**: every action logged to `logs/activity.log`
- **Backup**: copies `data/` to timestamped `backup/` folder
- **Restore**: restores latest backup
- **Coloured console UI**: ANSI escape codes
- **Loading animations**: progress bars on startup

---

## 🏗️ Data Structures Used

| Structure          | Usage                          |
|--------------------|--------------------------------|
| `struct Patient`   | Patient records (array)        |
| `struct Doctor`    | Doctor records (array)         |
| `struct Appointment` | Appointment records (array)  |
| `struct EmergencyPatient` | Emergency queue nodes   |
| `EmergencyQueue`   | Min-heap (priority queue)      |
| `struct Ambulance` | Fleet records (array)          |
| `struct Bill`      | Billing records (array)        |
| `struct Bed`       | ICU/General beds (arrays)      |
| `struct Credential`| Admin login data               |

---

## 🔁 Algorithms Used

| Algorithm       | Applied To                        |
|-----------------|-----------------------------------|
| Merge Sort      | Sort patients by name             |
| Bubble Sort     | Sort patients by age              |
| Insertion Sort  | Sort doctors by specialization    |
| Linear Search   | Search patients/doctors by ID/name|
| Min-Heap (PQ)   | Emergency priority queue          |
| XOR Cipher      | Password encryption               |

---

## 💾 File Handling

All records are stored in **binary format** using `fread`/`fwrite` for efficiency. Data files are in the `data/` directory and are loaded at startup and saved after every modification.

---

## 📊 Sample Output

```
╔══════════════════════════════════════════════════════╗
      1.  Patient Management
      2.  Doctor Management
      3.  Appointment Management
      4.  Emergency Handling
      5.  Ambulance Management
      6.  Bed Management
      7.  Billing System
      8.  Reports & Analytics
      9.  Statistics Dashboard
      10. Settings & Administration
      0.  Logout & Exit
╚══════════════════════════════════════════════════════╝

Patients: 12 active  |  Doctors: 5  |  Emergency Queue: 2  |  Appointments: 18
```

---

## 🎓 Viva Questions & Answers

**Q1. What data structure is used for the emergency queue?**  
A: A min-heap (priority queue). Patients with lower priority numbers (e.g., CRITICAL=1) are extracted first using the `heapifyDown` and `heapifyUp` operations.

**Q2. How is patient data persisted between runs?**  
A: Binary file I/O using `fwrite`/`fread` with the `Patient` structure. Files are saved to `data/patients.dat` after every write operation.

**Q3. How is the password protected?**  
A: XOR cipher encryption with key `0x5A` is applied before storing to disk. Password input is hidden using POSIX `termios` to disable echo.

**Q4. What sorting algorithms are used?**  
A: Merge Sort (O(n log n)) for patient names, Bubble Sort (O(n²)) for age, Insertion Sort (O(n²)) for doctor specializations.

**Q5. How does appointment token generation work?**  
A: `getNextToken()` scans all existing appointments and returns `maxToken + 1`, ensuring uniqueness across sessions.

**Q6. What is the time complexity of the priority queue operations?**  
A: Insert: O(log n), Extract-Min: O(log n), due to heap operations.

**Q7. How are multiple source files linked?**  
A: The Makefile compiles each `.c` to an `.o` object file separately, then the linker combines them with `gcc -o hospital *.o`.

**Q8. How does bed management distinguish ICU from General beds?**  
A: ICU beds are numbered 1001–1020, General beds 2001–2100. The `BedType` enum and separate arrays (`icuBeds[]`, `genBeds[]`) are used.

**Q9. What happens when 3 login attempts fail?**  
A: The login function returns 0, the main function exits with status 1, and the event is logged to `activity.log`.

**Q10. How is the statistics dashboard built without a database?**  
A: By iterating over the in-memory global arrays at runtime and computing counts/sums on the fly before display.

---

## 👨‍💻 Author

Developed as a 4-credit Engineering Final Year Project in C Language.

---
