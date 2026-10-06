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

> <img width="1467" height="832" alt="Screenshot 2026-10-06 at 10 04 53 PM" src="https://github.com/user-attachments/assets/4e071759-db8f-489f-aa25-092f6bb8cd5a" />

---

## 🧩 Modules & Features

### 1. Patient Management
- Add, update, discharge patients
- Search by ID or name (partial match)
- Sort by name (Merge Sort) or age (Bubble Sort)
- Stores: ID, name, age, gender, blood group, disease, contact, address, admitted date

- <img width="1465" height="831" alt="Screenshot 2026-10-06 at 10 07 21 PM" src="https://github.com/user-attachments/assets/b276e754-1242-42b3-9331-77004d4e2e26" />

### 2. Doctor Management
- Add/view/search doctors
- Assign doctor to patient (updates patient record)
- Toggle availability status
- Sort by specialization (Insertion Sort)

- <img width="1469" height="830" alt="Screenshot 2026-10-06 at 10 08 05 PM" src="https://github.com/user-attachments/assets/a579828b-b256-494f-a988-9cb2fb9d1a9b" />


### 3. Appointment Management
- Book appointments with auto-generated token numbers
- Time slot selection (8 slots available)
- Cancel or mark complete
- Filter view: All / Scheduled / Completed / Cancelled

- <img width="1470" height="831" alt="Screenshot 2026-10-06 at 10 08 33 PM" src="https://github.com/user-attachments/assets/d963d163-3248-421e-b6ae-aa3248132f54" />


### 4. Emergency Handling (Priority Queue)
- Register emergency patients with priority: CRITICAL / HIGH / MEDIUM / LOW
- Min-heap implementation (lower number = served first)
- Handle next emergency (dequeue)
- ASCII visualization of the entire queue

- <img width="1470" height="832" alt="Screenshot 2026-10-06 at 10 08 54 PM" src="https://github.com/user-attachments/assets/8214ffd4-e545-45d5-a3ef-14b9db0f2e8a" />


### 5. Ambulance Management
- Add ambulances with driver details
- Dispatch to patients (marks as Dispatched)
- Release back to fleet
- Fleet status dashboard

- <img width="1470" height="830" alt="Screenshot 2026-10-06 at 10 09 11 PM" src="https://github.com/user-attachments/assets/fbb0149a-9623-4596-a040-6eac5d0937a0" />


### 6. Bed Management
- ICU beds: 1001–1020 (₹3000/day)
- General Ward beds: 2001–2100 (₹800/day)
- Visual occupancy display
- Allocate to patient / release

- <img width="1470" height="833" alt="Screenshot 2026-10-06 at 10 10 52 PM" src="https://github.com/user-attachments/assets/52791b31-3135-4f25-88e7-0224b01d93d3" />


### 7. Billing System
- Generate itemised bills: consultation + bed + emergency + medicine + lab + misc
- Auto-compute totals
- Track partial payments
- Bill statuses: Pending / Partial / Paid

- <img width="1470" height="830" alt="Screenshot 2026-10-06 at 10 11 13 PM" src="https://github.com/user-attachments/assets/1b943d1d-41f8-4354-8961-1a08033f8995" />


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

- <img width="1470" height="837" alt="Screenshot 2026-10-06 at 10 11 34 PM" src="https://github.com/user-attachments/assets/487a74c3-bbc9-4049-b120-afe2396447af" />


### 10. Statistics Dashboard
- Live counts: patients, doctors, appointments, emergency queue
- Bed occupancy with ASCII bar charts
- Financial summary: collected vs pending revenue

- <img width="1470" height="831" alt="Screenshot 2026-10-06 at 10 13 08 PM" src="https://github.com/user-attachments/assets/d6c5cb56-fd4b-46cd-96a4-fc5a112580d1" />


### 11. Advanced Features
- **Activity Log**: every action logged to `logs/activity.log`
- **Backup**: copies `data/` to timestamped `backup/` folder
- **Restore**: restores latest backup
- **Coloured console UI**: ANSI escape codes
- **Loading animations**: progress bars on startup


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



## 👨‍💻 Author

Developed as a 4-credit Engineering First Year Project in C Language.

---
