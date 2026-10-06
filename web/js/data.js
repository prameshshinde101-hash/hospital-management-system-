/* ============================================================
 *  data.js — Shared application state & demo data
 *  Smart Hospital Management System — Web Frontend
 * ============================================================ */

const HospitalDB = {

  /* ── Auth ─────────────────────────────────────────────────── */
  credentials: { username: 'admin', password: 'admin123' },
  currentUser: null,

  /* ── Patients ─────────────────────────────────────────────── */
  patients: [
    { id: 1001, name: 'Rajesh Kumar',    age: 45, gender: 'M', blood: 'B+',  disease: 'Hypertension',        phone: '9876543210', address: '42, MG Road, Pune',       doctorId: 201, bed: 2001, admitDate: '2025-05-01', active: true },
    { id: 1002, name: 'Priya Desai',     age: 32, gender: 'F', blood: 'A+',  disease: 'Diabetes Type II',    phone: '9845012345', address: '15, Shivaji Nagar, Pune',  doctorId: 202, bed: 1001, admitDate: '2025-05-03', active: true },
    { id: 1003, name: 'Anil Sharma',     age: 58, gender: 'M', blood: 'O+',  disease: 'Cardiac Arrhythmia',  phone: '9012345678', address: '7, FC Road, Pune',         doctorId: 201, bed: 1002, admitDate: '2025-05-05', active: true },
    { id: 1004, name: 'Sunita Patil',    age: 27, gender: 'F', blood: 'AB-', disease: 'Appendicitis',        phone: '8765432109', address: '88, Koregaon Park, Pune',  doctorId: 203, bed: 2002, admitDate: '2025-05-07', active: true },
    { id: 1005, name: 'Vikram Joshi',    age: 41, gender: 'M', blood: 'O-',  disease: 'Kidney Stones',       phone: '9988776655', address: '3, Baner Road, Pune',      doctorId: 204, bed: 0,    admitDate: '2025-05-08', active: true },
    { id: 1006, name: 'Meena Iyer',      age: 65, gender: 'F', blood: 'A-',  disease: 'Osteoporosis',        phone: '9123456780', address: '22, Viman Nagar, Pune',    doctorId: 202, bed: 2003, admitDate: '2025-05-09', active: true },
    { id: 1007, name: 'Suresh Mehta',    age: 52, gender: 'M', blood: 'B-',  disease: 'Lung Infection',      phone: '9234567891', address: '11, Kothrud, Pune',        doctorId: 205, bed: 0,    admitDate: '2025-05-10', active: false },
    { id: 1008, name: 'Anita Kulkarni',  age: 38, gender: 'F', blood: 'B+',  disease: 'Migraine',            phone: '9345678902', address: '55, Aundh, Pune',          doctorId: 203, bed: 2004, admitDate: '2025-05-11', active: true },
    { id: 1009, name: 'Ravi Nair',       age: 73, gender: 'M', blood: 'A+',  disease: 'Heart Failure',       phone: '9456789013', address: '9, Wakad, Pune',           doctorId: 201, bed: 1003, admitDate: '2025-05-12', active: true },
    { id: 1010, name: 'Deepa Chatterjee',age: 29, gender: 'F', blood: 'O+',  disease: 'Fracture - Left Arm', phone: '9567890124', address: '31, Hadapsar, Pune',       doctorId: 204, bed: 2005, admitDate: '2025-05-13', active: true },
  ],

  /* ── Doctors ──────────────────────────────────────────────── */
  doctors: [
    { id: 201, name: 'Dr. Priya Sharma',    spec: 'Cardiology',       phone: '9111000001', exp: 15, available: true,  patients: 3 },
    { id: 202, name: 'Dr. Arvind Kulkarni', spec: 'Endocrinology',    phone: '9111000002', exp: 12, available: true,  patients: 2 },
    { id: 203, name: 'Dr. Sneha Joshi',     spec: 'General Surgery',  phone: '9111000003', exp: 8,  available: false, patients: 2 },
    { id: 204, name: 'Dr. Ramesh Patel',    spec: 'Orthopedics',      phone: '9111000004', exp: 20, available: true,  patients: 2 },
    { id: 205, name: 'Dr. Kavita Menon',    spec: 'Pulmonology',      phone: '9111000005', exp: 10, available: true,  patients: 1 },
  ],

  /* ── Appointments ─────────────────────────────────────────── */
  appointments: [
    { token: 1001, patientId: 1001, doctorId: 201, date: '2025-05-15', time: '10:00 AM', status: 'Scheduled',  notes: 'BP follow-up' },
    { token: 1002, patientId: 1002, doctorId: 202, date: '2025-05-15', time: '11:00 AM', status: 'Scheduled',  notes: 'Insulin adjustment' },
    { token: 1003, patientId: 1003, doctorId: 201, date: '2025-05-14', time: '09:00 AM', status: 'Completed',  notes: 'ECG review' },
    { token: 1004, patientId: 1004, doctorId: 203, date: '2025-05-14', time: '02:00 PM', status: 'Completed',  notes: 'Post-op check' },
    { token: 1005, patientId: 1005, doctorId: 204, date: '2025-05-16', time: '03:00 PM', status: 'Scheduled',  notes: 'Ultrasound results' },
    { token: 1006, patientId: 1008, doctorId: 203, date: '2025-05-13', time: '04:00 PM', status: 'Cancelled',  notes: 'Rescheduled' },
    { token: 1007, patientId: 1009, doctorId: 201, date: '2025-05-16', time: '09:00 AM', status: 'Scheduled',  notes: 'Cardiac review' },
    { token: 1008, patientId: 1010, doctorId: 204, date: '2025-05-17', time: '10:00 AM', status: 'Scheduled',  notes: 'X-Ray follow-up' },
  ],

  /* ── Emergency Queue ──────────────────────────────────────── */
  emergencyQueue: [
    { id: 3001, patientName: 'Govind Rao',    age: 68, condition: 'Cardiac arrest, unconscious',  priority: 1, priorityLabel: 'CRITICAL', arrival: '2025-05-13 08:15' },
    { id: 3002, patientName: 'Fatima Sheikh',  age: 34, condition: 'Severe head trauma, bleeding', priority: 2, priorityLabel: 'HIGH',     arrival: '2025-05-13 09:02' },
    { id: 3003, patientName: 'Santosh Gupta',  age: 50, condition: 'Chest pain, breathlessness',   priority: 2, priorityLabel: 'HIGH',     arrival: '2025-05-13 09:45' },
    { id: 3004, patientName: 'Rekha Verma',    age: 45, condition: 'High fever, seizures',          priority: 3, priorityLabel: 'MEDIUM',   arrival: '2025-05-13 10:20' },
    { id: 3005, patientName: 'Arjun Singh',    age: 22, condition: 'Fractured ankle, sports injury',priority: 4, priorityLabel: 'LOW',      arrival: '2025-05-13 11:00' },
  ],

  /* ── Ambulances ───────────────────────────────────────────── */
  ambulances: [
    { id: 501, vehicle: 'MH12AB1234', driver: 'Ramesh Yadav',  phone: '9800001111', status: 'Available',   patientId: 0,    trips: 142 },
    { id: 502, vehicle: 'MH12CD5678', driver: 'Sunil Pawar',   phone: '9800002222', status: 'Dispatched',  patientId: 1001, trips: 98  },
    { id: 503, vehicle: 'MH12EF9012', driver: 'Ajay Kshirsagar',phone:'9800003333', status: 'Available',   patientId: 0,    trips: 211 },
    { id: 504, vehicle: 'MH12GH3456', driver: 'Dinesh Mane',   phone: '9800004444', status: 'Maintenance', patientId: 0,    trips: 76  },
  ],

  /* ── Beds ─────────────────────────────────────────────────── */
  icuBeds: Array.from({length: 20}, (_, i) => ({
    number: 1001 + i,
    type: 'ICU',
    status: [1001,1002,1003].includes(1001+i) ? 'Occupied' : 'Available',
    patientId: [1001,1002,1003].includes(1001+i) ? [1002,1003,1009][i < 3 ? i : 0] : 0
  })),
  genBeds: Array.from({length: 100}, (_, i) => ({
    number: 2001 + i,
    type: 'General',
    status: [0,1,2,3,4].includes(i) ? 'Occupied' : 'Available',
    patientId: [0,1,2,3,4].includes(i) ? [1001,1004,1006,1008,1010][i] : 0
  })),

  /* ── Bills ────────────────────────────────────────────────── */
  bills: [
    { id: 9001, patientId: 1001, patientName: 'Rajesh Kumar',     date: '2025-05-13', consult: 1000, bed: 3000*12, emergency: 0,    medicine: 2500, lab: 1500, misc: 500,  paid: 40000, status: 'Paid' },
    { id: 9002, patientId: 1002, patientName: 'Priya Desai',      date: '2025-05-13', consult: 1000, bed: 3000*10, emergency: 2000, medicine: 3200, lab: 2000, misc: 800,  paid: 15000, status: 'Partial' },
    { id: 9003, patientId: 1004, patientName: 'Sunita Patil',     date: '2025-05-13', consult: 500,  bed: 800*6,   emergency: 2000, medicine: 1800, lab: 900,  misc: 300,  paid: 0,     status: 'Pending' },
    { id: 9004, patientId: 1008, patientName: 'Anita Kulkarni',   date: '2025-05-13', consult: 500,  bed: 800*2,   emergency: 0,    medicine: 600,  lab: 400,  misc: 100,  paid: 3200,  status: 'Paid' },
    { id: 9005, patientId: 1009, patientName: 'Ravi Nair',        date: '2025-05-13', consult: 1000, bed: 3000*1,  emergency: 2000, medicine: 4500, lab: 2500, misc: 1000, paid: 5000,  status: 'Partial' },
  ],

  /* ── Activity Log ─────────────────────────────────────────── */
  logs: [
    { time: '2025-05-13 06:00:01', action: 'Admin logged in' },
    { time: '2025-05-13 06:05:12', action: 'Patient added — ID: 1010' },
    { time: '2025-05-13 06:10:44', action: 'Appointment booked — Token: 1008' },
    { time: '2025-05-13 07:15:33', action: 'Emergency patient registered — ID: 3005' },
    { time: '2025-05-13 07:45:09', action: 'Ambulance dispatched — MH12CD5678' },
    { time: '2025-05-13 08:12:00', action: 'Emergency patient registered — ID: 3001 CRITICAL' },
    { time: '2025-05-13 08:30:55', action: 'Bill generated — Bill ID: 9005' },
    { time: '2025-05-13 09:00:10', action: 'Doctor assigned to patient — Dr. Priya Sharma → Ravi Nair' },
    { time: '2025-05-13 09:22:18', action: 'Bed allocated — Bed 1003 → Patient 1009' },
    { time: '2025-05-13 09:55:44', action: 'Payment updated — Bill 9001 FULLY PAID' },
  ],

  /* ── Helpers ──────────────────────────────────────────────── */
  getPatient(id)   { return this.patients.find(p => p.id === id); },
  getDoctor(id)    { return this.doctors.find(d => d.id === id); },
  nextPatientId()  { return Math.max(...this.patients.map(p => p.id)) + 1; },
  nextDoctorId()   { return Math.max(...this.doctors.map(d => d.id)) + 1; },
  nextToken()      { return Math.max(...this.appointments.map(a => a.token)) + 1; },
  nextBillId()     { return Math.max(...this.bills.map(b => b.id)) + 1; },
  nextEmergId()    { return Math.max(...this.emergencyQueue.map(e => e.id)) + 1; },
  nextAmbId()      { return Math.max(...this.ambulances.map(a => a.id)) + 1; },

  billTotal(b) {
    return b.consult + b.bed + b.emergency + b.medicine + b.lab + b.misc;
  },
  icuOccupied()  { return this.icuBeds.filter(b => b.status === 'Occupied').length; },
  genOccupied()  { return this.genBeds.filter(b => b.status === 'Occupied').length; },
  activePatients(){ return this.patients.filter(p => p.active).length; },
  availableDocs() { return this.doctors.filter(d => d.available).length; },
  pendingAppts()  { return this.appointments.filter(a => a.status === 'Scheduled').length; },
  totalRevenue()  { return this.bills.reduce((s,b) => s + b.paid, 0); },
  pendingRevenue(){ return this.bills.reduce((s,b) => s + (this.billTotal(b) - b.paid), 0); },
};
