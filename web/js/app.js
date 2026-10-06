/* ============================================================
 *  app.js — Main application controller
 *  Smart Hospital Management System — Web Frontend
 *  Modules: Dashboard, Patients, Doctors, Appointments,
 *           Emergency, Ambulance, Beds, Billing, Reports, Logs
 * ============================================================ */

'use strict';

/* ══════════════════════════════════════════════════════════════
   UTILITY HELPERS
══════════════════════════════════════════════════════════════ */
const $ = id => document.getElementById(id);
const el = (tag, cls, html='') => { const e=document.createElement(tag); if(cls)e.className=cls; e.innerHTML=html; return e; };

function toast(msg, type='info') {
  const c = $('toast-container');
  const icons = { success:'✓', error:'✕', info:'ℹ', warning:'⚠' };
  const t = el('div', `toast ${type}`, `<span>${icons[type]||'•'}</span><span>${msg}</span>`);
  c.appendChild(t);
  setTimeout(() => t.remove(), 3500);
}

function formatCurrency(n) { return '₹' + Number(n).toLocaleString('en-IN', {minimumFractionDigits:2}); }
function formatDate(d) { return d ? new Date(d).toLocaleDateString('en-IN',{day:'2-digit',month:'short',year:'numeric'}) : '—'; }

function openModal(id) { $(id).classList.add('open'); }
function closeModal(id) { $(id).classList.remove('open'); }

function confirmDialog(msg) { return confirm(msg); }

function statusBadge(status) {
  const map = {
    'Active':'badge-teal','Scheduled':'badge-blue','Completed':'badge-green',
    'Cancelled':'badge-red','Paid':'badge-green','Partial':'badge-yellow',
    'Pending':'badge-red','Available':'badge-green','Dispatched':'badge-red',
    'Maintenance':'badge-yellow','Occupied':'badge-red','Discharged':'badge-purple'
  };
  return `<span class="badge ${map[status]||'badge-blue'}">${status}</span>`;
}

function genderLabel(g) { return g==='M'?'Male':g==='F'?'Female':'Other'; }

/* ══════════════════════════════════════════════════════════════
   NAVIGATION
══════════════════════════════════════════════════════════════ */
function navigate(page) {
  document.querySelectorAll('.nav-item').forEach(n => n.classList.remove('active'));
  document.querySelectorAll('.page-content').forEach(p => p.classList.remove('active'));
  const navEl = document.querySelector(`.nav-item[data-page="${page}"]`);
  if (navEl) navEl.classList.add('active');
  const pageEl = $(`page-${page}`);
  if (pageEl) pageEl.classList.add('active');
  const titles = {
    dashboard:'Dashboard', patients:'Patient Management', doctors:'Doctor Management',
    appointments:'Appointment Management', emergency:'Emergency Queue',
    ambulance:'Ambulance Management', beds:'Bed Management',
    billing:'Billing System', reports:'Reports', logs:'Activity Log'
  };
  $('topbar-title').textContent = titles[page] || page;
  $('topbar-sub').textContent = new Date().toDateString();
  renderers[page] && renderers[page]();
}

/* ══════════════════════════════════════════════════════════════
   CLOCK
══════════════════════════════════════════════════════════════ */
function startClock() {
  const el = $('topbar-clock');
  setInterval(() => {
    el.textContent = new Date().toLocaleTimeString('en-IN',{hour:'2-digit',minute:'2-digit',second:'2-digit'});
  }, 1000);
}

/* ══════════════════════════════════════════════════════════════
   AUTH
══════════════════════════════════════════════════════════════ */
function login() {
  const user = $('login-user').value.trim();
  const pass = $('login-pass').value;
  const err  = $('login-error');
  if (user === HospitalDB.credentials.username && pass === HospitalDB.credentials.password) {
    HospitalDB.currentUser = user;
    $('login-page').style.display = 'none';
    $('app').style.display = 'flex';
    $('sidebar-username').textContent = user;
    HospitalDB.logs.push({ time: new Date().toLocaleString('en-IN'), action: `Admin "${user}" logged in` });
    navigate('dashboard');
    startClock();
    toast('Welcome back, ' + user + '!', 'success');
  } else {
    err.style.display = 'block';
    err.textContent = 'Invalid username or password. Try admin / admin123';
    $('login-pass').value = '';
  }
}

function logout() {
  if (!confirmDialog('Confirm logout?')) return;
  HospitalDB.logs.push({ time: new Date().toLocaleString('en-IN'), action: 'Admin logged out' });
  $('app').style.display = 'none';
  $('login-page').style.display = 'flex';
  $('login-user').value = ''; $('login-pass').value = '';
  $('login-error').style.display = 'none';
  HospitalDB.currentUser = null;
}

/* ══════════════════════════════════════════════════════════════
   1. DASHBOARD
══════════════════════════════════════════════════════════════ */
function renderDashboard() {
  const db = HospitalDB;
  const totalRevenue  = db.totalRevenue();
  const pendingRev    = db.pendingRevenue();
  const icuOcc        = db.icuOccupied();
  const genOcc        = db.genOccupied();
  const activeP       = db.activePatients();
  const availD        = db.availableDocs();
  const pendingA      = db.pendingAppts();
  const emgQ          = db.emergencyQueue.length;

  $('stat-active-patients').textContent   = activeP;
  $('stat-total-patients').textContent    = db.patients.length;
  $('stat-available-docs').textContent    = availD;
  $('stat-total-docs').textContent        = db.doctors.length;
  $('stat-pending-appts').textContent     = pendingA;
  $('stat-total-appts').textContent       = db.appointments.length;
  $('stat-emg-queue').textContent         = emgQ;
  $('stat-emg-handled').textContent       = '12';
  $('stat-icu-occ').textContent           = icuOcc;
  $('stat-icu-total').textContent         = db.icuBeds.length;
  $('stat-amb-avail').textContent         = db.ambulances.filter(a=>a.status==='Available').length;
  $('stat-amb-total').textContent         = db.ambulances.length;
  $('stat-revenue').textContent           = formatCurrency(totalRevenue);
  $('stat-pending-rev').textContent       = formatCurrency(pendingRev);

  /* Bed occupancy bars */
  const icuPct = Math.round(icuOcc / db.icuBeds.length * 100);
  const genPct = Math.round(genOcc / db.genBeds.length * 100);
  $('bar-icu').style.width  = icuPct + '%';
  $('bar-gen').style.width  = genPct + '%';
  $('bar-icu-label').textContent = `ICU — ${icuOcc}/${db.icuBeds.length} (${icuPct}%)`;
  $('bar-gen-label').textContent = `General — ${genOcc}/${db.genBeds.length} (${genPct}%)`;

  /* Doctor workload bars */
  const wlDiv = $('doctor-workload');
  wlDiv.innerHTML = db.doctors.map(d => {
    const pct = Math.min(100, Math.round(d.patients / 5 * 100));
    const color = pct>80?'var(--red)':pct>50?'var(--yellow)':'var(--teal)';
    return `
    <div class="chart-bar-row">
      <div class="chart-bar-label">${d.name.replace('Dr. ','Dr.')}</div>
      <div class="chart-bar-track"><div class="chart-bar-fill" style="width:${pct}%;background:${color}"></div></div>
      <div class="chart-bar-value">${d.patients} pts</div>
    </div>`;
  }).join('');

  /* Recent activity */
  const actDiv = $('recent-activity');
  actDiv.innerHTML = [...HospitalDB.logs].reverse().slice(0, 8).map(l => `
    <div class="log-entry">
      <div class="log-dot"></div>
      <div style="flex:1"><div style="font-size:13px">${l.action}</div></div>
      <div class="log-time">${l.time}</div>
    </div>`).join('');

  /* Top emergency */
  const topEmg = $('dash-emergency');
  if (db.emergencyQueue.length === 0) {
    topEmg.innerHTML = '<div class="empty-state"><div class="empty-state-icon">✅</div>No active emergencies</div>';
  } else {
    const sorted = [...db.emergencyQueue].sort((a,b) => a.priority - b.priority).slice(0,3);
    topEmg.innerHTML = sorted.map(e => {
      const cls = ['','critical','high','medium','low'][e.priority];
      const pBadge = `<span class="badge priority-${cls}">${e.priorityLabel}</span>`;
      return `<div class="queue-item ${cls}" style="margin-bottom:8px">
        <div class="queue-rank">#${e.priority}</div>
        <div class="queue-info">
          <div class="queue-name">${e.patientName}</div>
          <div class="queue-cond">${e.condition}</div>
          <div class="queue-meta">${e.arrival} · Age ${e.age}</div>
        </div>${pBadge}</div>`;
    }).join('');
  }
}

/* ══════════════════════════════════════════════════════════════
   2. PATIENTS
══════════════════════════════════════════════════════════════ */
let patientSearch = '', patientFilter = 'all', patientSort = 'id';

function renderPatients() {
  let data = [...HospitalDB.patients];
  if (patientSearch) data = data.filter(p =>
    p.name.toLowerCase().includes(patientSearch) ||
    String(p.id).includes(patientSearch) ||
    p.disease.toLowerCase().includes(patientSearch));
  if (patientFilter === 'active')     data = data.filter(p => p.active);
  if (patientFilter === 'discharged') data = data.filter(p => !p.active);
  if (patientSort === 'name') data.sort((a,b) => a.name.localeCompare(b.name));
  if (patientSort === 'age')  data.sort((a,b) => a.age - b.age);

  const tbody = $('patient-tbody');
  if (data.length === 0) {
    tbody.innerHTML = `<tr><td colspan="9"><div class="empty-state"><div class="empty-state-icon">🔍</div>No patients found</div></td></tr>`;
    return;
  }
  tbody.innerHTML = data.map(p => {
    const doc = HospitalDB.getDoctor(p.doctorId);
    return `<tr>
      <td class="mono">${p.id}</td>
      <td><div style="font-weight:600">${p.name}</div><div style="font-size:11px;color:var(--text-muted)">${genderLabel(p.gender)} · ${p.blood}</div></td>
      <td>${p.age}</td>
      <td>${p.disease}</td>
      <td>${doc ? doc.name : '<span class="text-muted">Unassigned</span>'}</td>
      <td>${p.bed ? `<span class="mono">${p.bed}</span>` : '<span class="text-muted">—</span>'}</td>
      <td>${formatDate(p.admitDate)}</td>
      <td>${statusBadge(p.active ? 'Active' : 'Discharged')}</td>
      <td>
        <div style="display:flex;gap:6px">
          <button class="btn btn-outline btn-sm" onclick="viewPatient(${p.id})">View</button>
          <button class="btn btn-outline btn-sm" onclick="editPatient(${p.id})">Edit</button>
          ${p.active ? `<button class="btn btn-red btn-sm" onclick="dischargePatient(${p.id})">Discharge</button>` : ''}
        </div>
      </td>
    </tr>`;
  }).join('');
}

function viewPatient(id) {
  const p = HospitalDB.getPatient(id);
  const doc = HospitalDB.getDoctor(p.doctorId);
  const bill = HospitalDB.bills.find(b => b.patientId === id);
  $('view-patient-content').innerHTML = `
    <div style="display:grid;grid-template-columns:1fr 1fr;gap:20px">
      ${field('Patient ID', p.id, true)} ${field('Status', p.active?'Active':'Discharged')}
      ${field('Full Name', p.name)} ${field('Age / Gender', `${p.age} / ${genderLabel(p.gender)}`)}
      ${field('Blood Group', p.blood)} ${field('Disease', p.disease)}
      ${field('Phone', p.phone)} ${field('Admit Date', formatDate(p.admitDate))}
      ${field('Doctor', doc ? doc.name : 'Unassigned')} ${field('Bed No.', p.bed || 'Not Allocated')}
    </div>
    <div class="divider"></div>
    ${field('Address', p.address)}
    ${bill ? `<div class="divider"></div><div style="font-size:12px;color:var(--text-muted);margin-bottom:8px">BILLING</div>${field('Total Bill', formatCurrency(HospitalDB.billTotal(bill)))} ${field('Paid', formatCurrency(bill.paid))}` : ''}
  `;
  openModal('modal-view-patient');
}

function field(label, value, mono=false) {
  return `<div><div style="font-size:11px;color:var(--text-muted);margin-bottom:4px;text-transform:uppercase;letter-spacing:.06em">${label}</div>
  <div style="font-size:14px;font-weight:500${mono?';font-family:var(--font-mono)':''}">${value}</div></div>`;
}

function editPatient(id) {
  const p = HospitalDB.getPatient(id);
  $('ep-id').value = p.id; $('ep-name').value = p.name; $('ep-age').value = p.age;
  $('ep-gender').value = p.gender; $('ep-blood').value = p.blood;
  $('ep-disease').value = p.disease; $('ep-phone').value = p.phone; $('ep-address').value = p.address;
  openModal('modal-edit-patient');
}

function saveEditPatient() {
  const id = parseInt($('ep-id').value);
  const p  = HospitalDB.getPatient(id);
  p.name    = $('ep-name').value.trim();
  p.age     = parseInt($('ep-age').value);
  p.gender  = $('ep-gender').value;
  p.blood   = $('ep-blood').value.trim();
  p.disease = $('ep-disease').value.trim();
  p.phone   = $('ep-phone').value.trim();
  p.address = $('ep-address').value.trim();
  closeModal('modal-edit-patient');
  HospitalDB.logs.push({ time: new Date().toLocaleString('en-IN'), action: `Patient updated — ${p.name} (ID: ${id})` });
  toast(`Patient ${p.name} updated`, 'success');
  renderPatients();
}

function dischargePatient(id) {
  const p = HospitalDB.getPatient(id);
  if (!confirmDialog(`Discharge ${p.name}?`)) return;
  p.active = false; p.bed = 0;
  HospitalDB.logs.push({ time: new Date().toLocaleString('en-IN'), action: `Patient discharged — ${p.name} (ID: ${id})` });
  toast(`${p.name} discharged`, 'info');
  renderPatients();
}

function addPatient() {
  const name    = $('ap-name').value.trim();
  const age     = parseInt($('ap-age').value);
  const gender  = $('ap-gender').value;
  const blood   = $('ap-blood').value.trim();
  const disease = $('ap-disease').value.trim();
  const phone   = $('ap-phone').value.trim();
  const address = $('ap-address').value.trim();
  if (!name || !age || !blood || !disease || !phone) { toast('Please fill all required fields', 'error'); return; }
  const id = HospitalDB.nextPatientId();
  HospitalDB.patients.push({ id, name, age, gender, blood, disease, phone, address,
    doctorId:0, bed:0, admitDate: new Date().toISOString().split('T')[0], active: true });
  closeModal('modal-add-patient');
  HospitalDB.logs.push({ time: new Date().toLocaleString('en-IN'), action: `Patient added — ${name} (ID: ${id})` });
  toast(`Patient ${name} added (ID: ${id})`, 'success');
  ['ap-name','ap-age','ap-blood','ap-disease','ap-phone','ap-address'].forEach(f => $(f).value='');
  renderPatients();
}

/* ══════════════════════════════════════════════════════════════
   3. DOCTORS
══════════════════════════════════════════════════════════════ */
let doctorSearch = '';

function renderDoctors() {
  let data = [...HospitalDB.doctors];
  if (doctorSearch) data = data.filter(d =>
    d.name.toLowerCase().includes(doctorSearch) ||
    d.spec.toLowerCase().includes(doctorSearch));

  $('doctor-tbody').innerHTML = data.map(d => `<tr>
    <td class="mono">${d.id}</td>
    <td><div style="font-weight:600">${d.name}</div></td>
    <td>${d.spec}</td>
    <td>${d.exp} yrs</td>
    <td>${d.phone}</td>
    <td><span class="badge ${d.patients>3?'badge-red':'badge-teal'}">${d.patients}</span></td>
    <td>${statusBadge(d.available?'Available':'Occupied')}</td>
    <td>
      <div style="display:flex;gap:6px">
        <button class="btn btn-outline btn-sm" onclick="toggleDoctorAvail(${d.id})">${d.available?'Set Busy':'Set Free'}</button>
        <button class="btn btn-outline btn-sm" onclick="editDoctor(${d.id})">Edit</button>
      </div>
    </td>
  </tr>`).join('');
}

function toggleDoctorAvail(id) {
  const d = HospitalDB.doctors.find(x => x.id === id);
  d.available = !d.available;
  HospitalDB.logs.push({ time: new Date().toLocaleString('en-IN'), action: `Doctor ${d.name} set to ${d.available?'Available':'Unavailable'}` });
  toast(`Dr. ${d.name} is now ${d.available?'Available':'Unavailable'}`, 'info');
  renderDoctors();
}

function editDoctor(id) {
  const d = HospitalDB.doctors.find(x => x.id === id);
  $('ed-id').value=d.id; $('ed-name').value=d.name; $('ed-spec').value=d.spec;
  $('ed-phone').value=d.phone; $('ed-exp').value=d.exp;
  openModal('modal-edit-doctor');
}

function saveEditDoctor() {
  const id = parseInt($('ed-id').value);
  const d  = HospitalDB.doctors.find(x => x.id === id);
  d.name=$('ed-name').value.trim(); d.spec=$('ed-spec').value.trim();
  d.phone=$('ed-phone').value.trim(); d.exp=parseInt($('ed-exp').value);
  closeModal('modal-edit-doctor');
  toast(`${d.name} updated`, 'success');
  renderDoctors();
}

function addDoctor() {
  const name=$('ad-name').value.trim(), spec=$('ad-spec').value.trim();
  const phone=$('ad-phone').value.trim(), exp=parseInt($('ad-exp').value)||0;
  if (!name||!spec||!phone) { toast('Fill all required fields','error'); return; }
  const id = HospitalDB.nextDoctorId();
  HospitalDB.doctors.push({ id, name, spec, phone, exp, available:true, patients:0 });
  closeModal('modal-add-doctor');
  HospitalDB.logs.push({ time: new Date().toLocaleString('en-IN'), action: `Doctor added — ${name} (ID: ${id})` });
  toast(`${name} added (ID: ${id})`, 'success');
  ['ad-name','ad-spec','ad-phone','ad-exp'].forEach(f=>$(f).value='');
  renderDoctors();
}

function assignDoctor() {
  const pid=parseInt($('assign-pid').value), did=parseInt($('assign-did').value);
  const p=HospitalDB.getPatient(pid), d=HospitalDB.doctors.find(x=>x.id===did);
  if (!p) { toast('Patient not found','error'); return; }
  if (!d) { toast('Doctor not found','error'); return; }
  if (p.doctorId && p.doctorId!==did) {
    const old=HospitalDB.doctors.find(x=>x.id===p.doctorId);
    if(old&&old.patients>0) old.patients--;
  }
  p.doctorId=did; d.patients++;
  closeModal('modal-assign-doctor');
  HospitalDB.logs.push({ time: new Date().toLocaleString('en-IN'), action: `${d.name} assigned to ${p.name}` });
  toast(`${d.name} assigned to ${p.name}`, 'success');
  renderDoctors();
}

/* ══════════════════════════════════════════════════════════════
   4. APPOINTMENTS
══════════════════════════════════════════════════════════════ */
let apptFilter = 'all';

function renderAppointments() {
  let data = [...HospitalDB.appointments];
  if (apptFilter !== 'all') data = data.filter(a => a.status === apptFilter);

  $('appt-tbody').innerHTML = data.map(a => {
    const p=HospitalDB.getPatient(a.patientId), d=HospitalDB.getDoctor(a.doctorId);
    return `<tr>
      <td class="mono">#${a.token}</td>
      <td>${p?p.name:'Unknown'}</td>
      <td>${d?d.name:'Unknown'}</td>
      <td>${formatDate(a.date)}</td>
      <td class="mono">${a.time}</td>
      <td>${statusBadge(a.status)}</td>
      <td style="font-size:12px;color:var(--text-secondary)">${a.notes||'—'}</td>
      <td>
        <div style="display:flex;gap:6px">
          ${a.status==='Scheduled'?`
            <button class="btn btn-outline btn-sm" onclick="completeAppt(${a.token})">Complete</button>
            <button class="btn btn-red btn-sm" onclick="cancelAppt(${a.token})">Cancel</button>`:
          '<span class="text-muted" style="font-size:12px">—</span>'}
        </div>
      </td>
    </tr>`;
  }).join('');
}

function completeAppt(token) {
  const a = HospitalDB.appointments.find(x=>x.token===token);
  a.status='Completed';
  HospitalDB.logs.push({ time: new Date().toLocaleString('en-IN'), action: `Appointment completed — Token #${token}` });
  toast(`Token #${token} marked completed`, 'success');
  renderAppointments();
}

function cancelAppt(token) {
  if (!confirmDialog('Cancel this appointment?')) return;
  const a = HospitalDB.appointments.find(x=>x.token===token);
  a.status='Cancelled';
  HospitalDB.logs.push({ time: new Date().toLocaleString('en-IN'), action: `Appointment cancelled — Token #${token}` });
  toast(`Token #${token} cancelled`, 'warning');
  renderAppointments();
}

function bookAppointment() {
  const pid=parseInt($('ba-pid').value), did=parseInt($('ba-did').value);
  const date=$('ba-date').value, time=$('ba-time').value, notes=$('ba-notes').value.trim();
  if (!pid||!did||!date||!time) { toast('Fill all required fields','error'); return; }
  const p=HospitalDB.getPatient(pid), d=HospitalDB.doctors.find(x=>x.id===did);
  if (!p) { toast('Patient ID not found','error'); return; }
  if (!d) { toast('Doctor ID not found','error'); return; }
  const token=HospitalDB.nextToken();
  HospitalDB.appointments.push({token,patientId:pid,doctorId:did,date,time,status:'Scheduled',notes});
  closeModal('modal-book-appt');
  HospitalDB.logs.push({ time: new Date().toLocaleString('en-IN'), action: `Appointment booked — Token #${token} (${p.name} → ${d.name})` });
  toast(`Appointment booked! Token: #${token}`, 'success');
  ['ba-pid','ba-did','ba-date','ba-notes'].forEach(f=>$(f).value='');
  renderAppointments();
}

/* ══════════════════════════════════════════════════════════════
   5. EMERGENCY
══════════════════════════════════════════════════════════════ */
function renderEmergency() {
  const sorted = [...HospitalDB.emergencyQueue].sort((a,b) => a.priority - b.priority);
  const container = $('emergency-queue');
  if (sorted.length === 0) {
    container.innerHTML = '<div class="empty-state"><div class="empty-state-icon">✅</div>Emergency queue is clear</div>';
    return;
  }
  container.innerHTML = sorted.map((e,i) => {
    const cls = ['','critical','high','medium','low'][e.priority];
    const pBadge = `<span class="badge priority-${cls}">${e.priorityLabel}</span>`;
    return `<div class="queue-item ${cls}">
      <div class="queue-rank">#${i+1}</div>
      <div class="queue-info">
        <div class="queue-name">${e.patientName} <span style="font-size:12px;color:var(--text-muted)">Age ${e.age}</span></div>
        <div class="queue-cond">${e.condition}</div>
        <div class="queue-meta">Arrived: ${e.arrival} · ID: ${e.id}</div>
      </div>
      ${pBadge}
      <button class="btn btn-teal btn-sm" onclick="handleEmergency(${e.id})">Handle</button>
    </div>`;
  }).join('');
  $('emg-queue-count').textContent = sorted.length + ' in queue';

  /* Priority distribution bar */
  const counts = {1:0,2:0,3:0,4:0};
  sorted.forEach(e => counts[e.priority]++);
  $('emg-dist').innerHTML = [
    {label:'Critical',key:1,color:'var(--red)'},
    {label:'High',key:2,color:'var(--yellow)'},
    {label:'Medium',key:3,color:'var(--blue)'},
    {label:'Low',key:4,color:'var(--green)'}
  ].map(({label,key,color}) => `
    <div class="chart-bar-row">
      <div class="chart-bar-label">${label}</div>
      <div class="chart-bar-track"><div class="chart-bar-fill" style="width:${counts[key]?counts[key]/sorted.length*100:0}%;background:${color}"></div></div>
      <div class="chart-bar-value">${counts[key]}</div>
    </div>`).join('');
}

function handleEmergency(id) {
  const idx = HospitalDB.emergencyQueue.findIndex(e => e.id === id);
  const e   = HospitalDB.emergencyQueue[idx];
  if (!confirmDialog(`Handle emergency for ${e.patientName} (${e.priorityLabel})?`)) return;
  HospitalDB.emergencyQueue.splice(idx, 1);
  HospitalDB.logs.push({ time: new Date().toLocaleString('en-IN'), action: `Emergency handled — ${e.patientName} (${e.priorityLabel})` });
  toast(`${e.patientName} emergency handled`, 'success');
  renderEmergency();
}

function registerEmergency() {
  const name=$('emg-name').value.trim(), age=parseInt($('emg-age').value);
  const condition=$('emg-condition').value.trim(), priority=parseInt($('emg-priority').value);
  if (!name||!age||!condition||!priority) { toast('Fill all required fields','error'); return; }
  const labels={1:'CRITICAL',2:'HIGH',3:'MEDIUM',4:'LOW'};
  const id=HospitalDB.nextEmergId();
  const now=new Date().toLocaleString('en-IN');
  HospitalDB.emergencyQueue.push({id,patientName:name,age,condition,priority,priorityLabel:labels[priority],arrival:now});
  closeModal('modal-add-emergency');
  HospitalDB.logs.push({ time: now, action: `Emergency registered — ${name} (${labels[priority]}) ID: ${id}` });
  toast(`Emergency registered for ${name}`, priority===1?'error':'warning');
  ['emg-name','emg-age','emg-condition'].forEach(f=>$(f).value='');
  renderEmergency();
}

/* ══════════════════════════════════════════════════════════════
   6. AMBULANCE
══════════════════════════════════════════════════════════════ */
function renderAmbulance() {
  $('amb-tbody').innerHTML = HospitalDB.ambulances.map(a => `<tr>
    <td class="mono">${a.id}</td>
    <td class="mono">${a.vehicle}</td>
    <td>${a.driver}</td>
    <td>${a.phone}</td>
    <td>${statusBadge(a.status)}</td>
    <td>${a.patientId ? `<span class="mono">${a.patientId}</span>` : '—'}</td>
    <td class="mono">${a.trips}</td>
    <td>
      <div style="display:flex;gap:6px">
        ${a.status==='Available' ? `<button class="btn btn-teal btn-sm" onclick="dispatchAmb(${a.id})">Dispatch</button>` : ''}
        ${a.status==='Dispatched' ? `<button class="btn btn-outline btn-sm" onclick="returnAmb(${a.id})">Return</button>` : ''}
        ${a.status!=='Dispatched' ? `<button class="btn btn-outline btn-sm" onclick="toggleMaint(${a.id})">${a.status==='Maintenance'?'Activate':'Maintenance'}</button>` : ''}
      </div>
    </td>
  </tr>`).join('');

  /* Fleet stats */
  const avail = HospitalDB.ambulances.filter(a=>a.status==='Available').length;
  const disp  = HospitalDB.ambulances.filter(a=>a.status==='Dispatched').length;
  const maint = HospitalDB.ambulances.filter(a=>a.status==='Maintenance').length;
  $('amb-avail-count').textContent = avail;
  $('amb-disp-count').textContent  = disp;
  $('amb-maint-count').textContent = maint;
}

function dispatchAmb(id) {
  const pid = prompt('Enter Patient ID to assign (leave blank for external):');
  const a   = HospitalDB.ambulances.find(x=>x.id===id);
  a.status='Dispatched'; a.patientId=parseInt(pid)||0; a.trips++;
  HospitalDB.logs.push({ time: new Date().toLocaleString('en-IN'), action: `Ambulance ${a.vehicle} dispatched` });
  toast(`${a.vehicle} dispatched`, 'info');
  renderAmbulance();
}

function returnAmb(id) {
  const a=HospitalDB.ambulances.find(x=>x.id===id);
  a.status='Available'; a.patientId=0;
  HospitalDB.logs.push({ time: new Date().toLocaleString('en-IN'), action: `Ambulance ${a.vehicle} returned to fleet` });
  toast(`${a.vehicle} is now available`, 'success');
  renderAmbulance();
}

function toggleMaint(id) {
  const a=HospitalDB.ambulances.find(x=>x.id===id);
  a.status = a.status==='Maintenance' ? 'Available' : 'Maintenance';
  toast(`${a.vehicle} → ${a.status}`, 'info');
  renderAmbulance();
}

function addAmbulance() {
  const vehicle=$('aa-vehicle').value.trim(), driver=$('aa-driver').value.trim(), phone=$('aa-phone').value.trim();
  if (!vehicle||!driver||!phone) { toast('Fill all required fields','error'); return; }
  const id=HospitalDB.nextAmbId();
  HospitalDB.ambulances.push({id,vehicle,driver,phone,status:'Available',patientId:0,trips:0});
  closeModal('modal-add-ambulance');
  HospitalDB.logs.push({ time: new Date().toLocaleString('en-IN'), action: `Ambulance added — ${vehicle}` });
  toast(`Ambulance ${vehicle} added`, 'success');
  ['aa-vehicle','aa-driver','aa-phone'].forEach(f=>$(f).value='');
  renderAmbulance();
}

/* ══════════════════════════════════════════════════════════════
   7. BEDS
══════════════════════════════════════════════════════════════ */
let bedTypeView = 'icu';

function renderBeds() {
  const isIcu = bedTypeView === 'icu';
  const beds  = isIcu ? HospitalDB.icuBeds : HospitalDB.genBeds;
  const occ   = beds.filter(b=>b.status==='Occupied').length;
  const free  = beds.length - occ;
  const pct   = Math.round(occ/beds.length*100);

  $('bed-occ-count').textContent  = occ;
  $('bed-free-count').textContent = free;
  $('bed-total-count').textContent= beds.length;
  $('bed-occ-pct').textContent    = pct + '%';
  $('bed-occ-bar').style.width    = pct + '%';
  $('bed-occ-bar').style.background = pct>80?'var(--red)':pct>50?'var(--yellow)':'var(--teal)';

  const grid = $('bed-grid');
  grid.innerHTML = beds.map(b => {
    const cls = b.status==='Occupied' ? 'occupied' : 'available';
    const tip = b.status==='Occupied' ? `Patient: ${b.patientId}` : 'Free';
    return `<div class="bed-cell ${cls}" title="${tip}" onclick="bedCellClick(${b.number},'${b.status}',${b.patientId})">
      <div class="bed-cell-icon">${b.status==='Occupied'?'🛏':'🛏'}</div>
      <div>${b.number}</div>
      <div style="font-size:9px">${b.status==='Occupied'?'OCC':'FREE'}</div>
    </div>`;
  }).join('');
}

function bedCellClick(number, status, patientId) {
  if (status === 'Occupied') {
    if (confirmDialog(`Release bed ${number} (Patient ${patientId})?`)) {
      const beds = number>=2001 ? HospitalDB.genBeds : HospitalDB.icuBeds;
      const b = beds.find(x=>x.number===number);
      const p = HospitalDB.getPatient(b.patientId);
      b.status='Available'; b.patientId=0;
      if (p) p.bed=0;
      HospitalDB.logs.push({ time: new Date().toLocaleString('en-IN'), action: `Bed ${number} released` });
      toast(`Bed ${number} released`, 'success');
      renderBeds();
    }
  }
}

function allocateBed() {
  const pid=parseInt($('alloc-pid').value);
  const type=$('alloc-type').value;
  if (!pid) { toast('Enter patient ID','error'); return; }
  const p=HospitalDB.getPatient(pid);
  if (!p) { toast('Patient not found','error'); return; }
  const beds = type==='ICU' ? HospitalDB.icuBeds : HospitalDB.genBeds;
  const free  = beds.find(b=>b.status==='Available');
  if (!free) { toast('No '+type+' beds available','error'); return; }
  free.status='Occupied'; free.patientId=pid; p.bed=free.number;
  closeModal('modal-alloc-bed');
  HospitalDB.logs.push({ time: new Date().toLocaleString('en-IN'), action: `Bed ${free.number} allocated to Patient ${pid}` });
  toast(`Bed ${free.number} allocated to ${p.name}`, 'success');
  $('alloc-pid').value='';
  renderBeds();
}

/* ══════════════════════════════════════════════════════════════
   8. BILLING
══════════════════════════════════════════════════════════════ */
let billFilter = 'all';

function renderBilling() {
  let data = [...HospitalDB.bills];
  if (billFilter !== 'all') data = data.filter(b => b.status === billFilter);

  $('bill-tbody').innerHTML = data.map(b => {
    const total   = HospitalDB.billTotal(b);
    const balance = total - b.paid;
    return `<tr>
      <td class="mono">${b.id}</td>
      <td>${b.patientName}</td>
      <td>${formatDate(b.date)}</td>
      <td class="mono">${formatCurrency(total)}</td>
      <td class="mono text-green">${formatCurrency(b.paid)}</td>
      <td class="mono ${balance>0?'text-red':'text-green'}">${formatCurrency(balance)}</td>
      <td>${statusBadge(b.status)}</td>
      <td>
        <div style="display:flex;gap:6px">
          <button class="btn btn-outline btn-sm" onclick="viewBill(${b.id})">View</button>
          ${b.status!=='Paid'?`<button class="btn btn-teal btn-sm" onclick="payBill(${b.id})">Pay</button>`:''}
        </div>
      </td>
    </tr>`;
  }).join('');
}

function viewBill(id) {
  const b=HospitalDB.bills.find(x=>x.id===id);
  const total=HospitalDB.billTotal(b), balance=total-b.paid;
  $('bill-detail-content').innerHTML = `
    <div style="font-family:var(--font-mono);font-size:12px">
      <div style="text-align:center;margin-bottom:20px">
        <div style="font-family:var(--font-display);font-size:20px">SMART HOSPITAL</div>
        <div style="color:var(--text-muted);margin-top:4px">Tax Invoice</div>
      </div>
      <div style="display:grid;grid-template-columns:1fr 1fr;gap:12px;margin-bottom:20px">
        ${field('Bill ID','#'+b.id,true)} ${field('Date',formatDate(b.date))}
        ${field('Patient',b.patientName)} ${field('Patient ID',b.patientId,true)}
      </div>
      <div class="divider"></div>
      <table style="width:100%;margin:16px 0">
        ${billRow('Consultation Fee',b.consult)}
        ${billRow('Bed Charges',b.bed)}
        ${b.emergency?billRow('Emergency Charges',b.emergency):''}
        ${b.medicine?billRow('Medicine Charges',b.medicine):''}
        ${b.lab?billRow('Lab / Diagnostics',b.lab):''}
        ${b.misc?billRow('Miscellaneous',b.misc):''}
      </table>
      <div class="divider"></div>
      <div style="display:flex;justify-content:space-between;font-weight:700;margin:8px 0;font-size:14px">
        <span>TOTAL</span><span>${formatCurrency(total)}</span></div>
      <div style="display:flex;justify-content:space-between;color:var(--green);margin:4px 0">
        <span>Paid</span><span>${formatCurrency(b.paid)}</span></div>
      <div style="display:flex;justify-content:space-between;color:${balance>0?'var(--red)':'var(--green)'};margin:4px 0;font-weight:700">
        <span>Balance</span><span>${formatCurrency(balance)}</span></div>
      <div style="text-align:center;margin-top:16px">${statusBadge(b.status)}</div>
    </div>`;
  openModal('modal-view-bill');
}

function billRow(label,amount) {
  if (!amount) return '';
  return `<tr><td style="padding:4px 0;color:var(--text-secondary)">${label}</td>
    <td style="text-align:right;padding:4px 0">${formatCurrency(amount)}</td></tr>`;
}

function payBill(id) {
  const b=HospitalDB.bills.find(x=>x.id===id);
  const total=HospitalDB.billTotal(b), balance=total-b.paid;
  const amt=parseFloat(prompt(`Enter payment amount (Balance: ${formatCurrency(balance)})`));
  if (!amt||amt<=0) return;
  b.paid=Math.min(total, b.paid+amt);
  b.status = b.paid>=total ? 'Paid' : 'Partial';
  HospitalDB.logs.push({ time: new Date().toLocaleString('en-IN'), action: `Payment received — Bill #${id} ${b.status}` });
  toast(`Payment of ${formatCurrency(amt)} recorded. Status: ${b.status}`, 'success');
  renderBilling();
}

function generateBill() {
  const pid=parseInt($('gb-pid').value), days=parseInt($('gb-days').value)||0;
  const emergency=parseFloat($('gb-emergency').value)||0;
  const medicine=parseFloat($('gb-medicine').value)||0;
  const lab=parseFloat($('gb-lab').value)||0, misc=parseFloat($('gb-misc').value)||0;
  if (!pid) { toast('Enter Patient ID','error'); return; }
  const p=HospitalDB.getPatient(pid);
  if (!p) { toast('Patient not found','error'); return; }
  const doc=HospitalDB.getDoctor(p.doctorId);
  const consult = doc&&doc.exp>=10 ? 1000 : 500;
  const bedRate = p.bed>=1001&&p.bed<=1020 ? 3000 : 800;
  const bed = bedRate*days;
  const id=HospitalDB.nextBillId();
  const total=consult+bed+emergency+medicine+lab+misc;
  HospitalDB.bills.push({id,patientId:pid,patientName:p.name,
    date:new Date().toISOString().split('T')[0],consult,bed,emergency,medicine,lab,misc,paid:0,status:'Pending'});
  closeModal('modal-gen-bill');
  HospitalDB.logs.push({ time: new Date().toLocaleString('en-IN'), action: `Bill generated — #${id} for ${p.name} (${formatCurrency(total)})` });
  toast(`Bill #${id} generated: ${formatCurrency(total)}`, 'success');
  ['gb-pid','gb-days','gb-emergency','gb-medicine','gb-lab','gb-misc'].forEach(f=>$(f).value='');
  renderBilling();
}

/* ══════════════════════════════════════════════════════════════
   9. REPORTS
══════════════════════════════════════════════════════════════ */
function renderReports() {
  const db=HospitalDB;
  /* Patient report */
  const bloodGroups={};
  db.patients.forEach(p=>{bloodGroups[p.blood]=(bloodGroups[p.blood]||0)+1;});
  $('rpt-patient-body').innerHTML = `
    <div style="display:grid;grid-template-columns:repeat(3,1fr);gap:16px;margin-bottom:20px">
      <div class="card" style="text-align:center"><div style="font-size:28px;font-weight:700;color:var(--teal)">${db.patients.length}</div><div style="font-size:12px;color:var(--text-secondary)">Total Patients</div></div>
      <div class="card" style="text-align:center"><div style="font-size:28px;font-weight:700;color:var(--green)">${db.activePatients()}</div><div style="font-size:12px;color:var(--text-secondary)">Active</div></div>
      <div class="card" style="text-align:center"><div style="font-size:28px;font-weight:700;color:var(--purple)">${db.patients.filter(p=>!p.active).length}</div><div style="font-size:12px;color:var(--text-secondary)">Discharged</div></div>
    </div>
    <div style="font-size:12px;color:var(--text-secondary);margin-bottom:8px;text-transform:uppercase;letter-spacing:.06em">Blood Group Distribution</div>
    ${Object.entries(bloodGroups).map(([bg,count])=>`
      <div class="chart-bar-row">
        <div class="chart-bar-label">${bg}</div>
        <div class="chart-bar-track"><div class="chart-bar-fill" style="width:${count/db.patients.length*100}%;background:var(--teal)"></div></div>
        <div class="chart-bar-value">${count}</div>
      </div>`).join('')}`;

  /* Billing report */
  const total=db.bills.reduce((s,b)=>s+HospitalDB.billTotal(b),0);
  const paid=db.totalRevenue(), pending=db.pendingRevenue();
  $('rpt-billing-body').innerHTML = `
    <div style="display:grid;grid-template-columns:repeat(3,1fr);gap:16px;margin-bottom:20px">
      <div class="card" style="text-align:center"><div style="font-size:22px;font-weight:700;color:var(--teal);font-family:var(--font-mono)">${formatCurrency(total)}</div><div style="font-size:12px;color:var(--text-secondary)">Total Billed</div></div>
      <div class="card" style="text-align:center"><div style="font-size:22px;font-weight:700;color:var(--green);font-family:var(--font-mono)">${formatCurrency(paid)}</div><div style="font-size:12px;color:var(--text-secondary)">Collected</div></div>
      <div class="card" style="text-align:center"><div style="font-size:22px;font-weight:700;color:var(--red);font-family:var(--font-mono)">${formatCurrency(pending)}</div><div style="font-size:12px;color:var(--text-secondary)">Pending</div></div>
    </div>
    <div class="chart-bar-row"><div class="chart-bar-label">Paid</div><div class="chart-bar-track"><div class="chart-bar-fill" style="width:${paid/total*100}%;background:var(--green)"></div></div><div class="chart-bar-value">${db.bills.filter(b=>b.status==='Paid').length} bills</div></div>
    <div class="chart-bar-row"><div class="chart-bar-label">Partial</div><div class="chart-bar-track"><div class="chart-bar-fill" style="width:${db.bills.filter(b=>b.status==='Partial').length/db.bills.length*100}%;background:var(--yellow)"></div></div><div class="chart-bar-value">${db.bills.filter(b=>b.status==='Partial').length} bills</div></div>
    <div class="chart-bar-row"><div class="chart-bar-label">Pending</div><div class="chart-bar-track"><div class="chart-bar-fill" style="width:${db.bills.filter(b=>b.status==='Pending').length/db.bills.length*100}%;background:var(--red)"></div></div><div class="chart-bar-value">${db.bills.filter(b=>b.status==='Pending').length} bills</div></div>`;

  /* Emergency report */
  const prioLabels={1:'Critical',2:'High',3:'Medium',4:'Low'};
  const prioCounts={1:0,2:0,3:0,4:0};
  db.emergencyQueue.forEach(e=>prioCounts[e.priority]++);
  $('rpt-emergency-body').innerHTML = `
    <div style="display:grid;grid-template-columns:repeat(4,1fr);gap:12px;margin-bottom:20px">
      <div class="card" style="text-align:center;border-color:rgba(255,76,106,0.3)"><div style="font-size:28px;font-weight:700;color:var(--red)">${prioCounts[1]}</div><div style="font-size:11px;color:var(--text-secondary)">Critical</div></div>
      <div class="card" style="text-align:center;border-color:rgba(255,184,39,0.3)"><div style="font-size:28px;font-weight:700;color:var(--yellow)">${prioCounts[2]}</div><div style="font-size:11px;color:var(--text-secondary)">High</div></div>
      <div class="card" style="text-align:center;border-color:rgba(59,158,255,0.3)"><div style="font-size:28px;font-weight:700;color:var(--blue)">${prioCounts[3]}</div><div style="font-size:11px;color:var(--text-secondary)">Medium</div></div>
      <div class="card" style="text-align:center;border-color:rgba(48,217,123,0.3)"><div style="font-size:28px;font-weight:700;color:var(--green)">${prioCounts[4]}</div><div style="font-size:11px;color:var(--text-secondary)">Low</div></div>
    </div>
    <div style="font-size:12px;color:var(--text-secondary);margin-bottom:12px">Current queue: ${db.emergencyQueue.length} patients</div>
    ${db.emergencyQueue.length===0?'<div class="empty-state">Queue is clear ✅</div>':db.emergencyQueue.slice(0,5).map(e=>`<div style="display:flex;justify-content:space-between;padding:8px 0;border-bottom:1px solid var(--border);font-size:13px"><span>${e.patientName}</span><span class="badge priority-${['','critical','high','medium','low'][e.priority]}">${e.priorityLabel}</span></div>`).join('')}`;

  /* Doctor report */
  $('rpt-doctor-body').innerHTML = `
    <div style="margin-bottom:16px">
    ${db.doctors.map(d=>`
      <div style="display:flex;align-items:center;gap:12px;padding:10px 0;border-bottom:1px solid var(--border)">
        <div style="width:36px;height:36px;background:var(--teal-glow);border:1px solid var(--teal);border-radius:50%;display:grid;place-items:center;font-weight:700;color:var(--teal);font-size:12px;flex-shrink:0">${d.name.split(' ')[1]?.[0]||'D'}</div>
        <div style="flex:1"><div style="font-weight:600">${d.name}</div><div style="font-size:11px;color:var(--text-secondary)">${d.spec} · ${d.exp} yrs exp</div></div>
        <div style="text-align:right"><div style="font-family:var(--font-mono);font-size:13px">${d.patients} patients</div>${statusBadge(d.available?'Available':'Occupied')}</div>
      </div>`).join('')}
    </div>`;
}

/* ══════════════════════════════════════════════════════════════
   10. ACTIVITY LOG (LINKED LIST)
══════════════════════════════════════════════════════════════ */
let logSearch = '';

function renderLogs() {
  let data = [...HospitalDB.logs].reverse();
  if (logSearch) data = data.filter(l => l.action.toLowerCase().includes(logSearch) || l.time.includes(logSearch));
  $('log-count').textContent = HospitalDB.logs.length + ' entries in linked list';
  $('log-list').innerHTML = data.length === 0
    ? '<div class="empty-state">No log entries match your search</div>'
    : data.map(l => `
      <div class="log-entry">
        <div class="log-dot"></div>
        <div style="flex:1"><div style="font-size:13px">${l.action}</div></div>
        <div class="log-time">${l.time}</div>
      </div>`).join('');
}

function addLogEntry() {
  const msg = $('log-manual-entry').value.trim();
  if (!msg) { toast('Enter a log message','error'); return; }
  HospitalDB.logs.push({ time: new Date().toLocaleString('en-IN'), action: msg });
  $('log-manual-entry').value = '';
  toast('Log entry added','success');
  renderLogs();
}

/* ══════════════════════════════════════════════════════════════
   RENDERER MAP
══════════════════════════════════════════════════════════════ */
const renderers = {
  dashboard:    renderDashboard,
  patients:     renderPatients,
  doctors:      renderDoctors,
  appointments: renderAppointments,
  emergency:    renderEmergency,
  ambulance:    renderAmbulance,
  beds:         renderBeds,
  billing:      renderBilling,
  reports:      renderReports,
  logs:         renderLogs,
};

/* ══════════════════════════════════════════════════════════════
   EVENT WIRING ON DOM READY
══════════════════════════════════════════════════════════════ */
document.addEventListener('DOMContentLoaded', () => {

  /* Login */
  $('login-btn').addEventListener('click', login);
  $('login-pass').addEventListener('keydown', e => { if(e.key==='Enter') login(); });

  /* Logout */
  $('logout-btn').addEventListener('click', logout);

  /* Nav */
  document.querySelectorAll('.nav-item[data-page]').forEach(n => {
    n.addEventListener('click', () => navigate(n.dataset.page));
  });

  /* Modal close buttons */
  document.querySelectorAll('[data-close-modal]').forEach(btn => {
    btn.addEventListener('click', () => closeModal(btn.dataset.closeModal));
  });
  document.querySelectorAll('.modal-overlay').forEach(overlay => {
    overlay.addEventListener('click', e => { if(e.target===overlay) overlay.classList.remove('open'); });
  });

  /* Patient module */
  $('btn-add-patient').addEventListener('click', () => openModal('modal-add-patient'));
  $('btn-save-patient').addEventListener('click', addPatient);
  $('btn-save-edit-patient').addEventListener('click', saveEditPatient);
  $('patient-search').addEventListener('input', e => { patientSearch=e.target.value.toLowerCase(); renderPatients(); });
  $('patient-filter').addEventListener('change', e => { patientFilter=e.target.value; renderPatients(); });
  $('patient-sort').addEventListener('change', e => { patientSort=e.target.value; renderPatients(); });

  /* Doctor module */
  $('btn-add-doctor').addEventListener('click', () => openModal('modal-add-doctor'));
  $('btn-save-doctor').addEventListener('click', addDoctor);
  $('btn-save-edit-doctor').addEventListener('click', saveEditDoctor);
  $('btn-assign-doctor').addEventListener('click', () => openModal('modal-assign-doctor'));
  $('btn-do-assign').addEventListener('click', assignDoctor);
  $('doctor-search').addEventListener('input', e => { doctorSearch=e.target.value.toLowerCase(); renderDoctors(); });

  /* Appointment module */
  $('btn-book-appt').addEventListener('click', () => openModal('modal-book-appt'));
  $('btn-save-appt').addEventListener('click', bookAppointment);
  $('appt-filter').addEventListener('change', e => { apptFilter=e.target.value; renderAppointments(); });

  /* Emergency module */
  $('btn-add-emergency').addEventListener('click', () => openModal('modal-add-emergency'));
  $('btn-save-emergency').addEventListener('click', registerEmergency);

  /* Ambulance module */
  $('btn-add-ambulance').addEventListener('click', () => openModal('modal-add-ambulance'));
  $('btn-save-ambulance').addEventListener('click', addAmbulance);

  /* Bed module */
  $('btn-bed-icu').addEventListener('click', () => { bedTypeView='icu'; $('btn-bed-icu').className='btn btn-teal'; $('btn-bed-gen').className='btn btn-outline'; renderBeds(); });
  $('btn-bed-gen').addEventListener('click', () => { bedTypeView='gen'; $('btn-bed-gen').className='btn btn-teal'; $('btn-bed-icu').className='btn btn-outline'; renderBeds(); });
  $('btn-alloc-bed').addEventListener('click', () => openModal('modal-alloc-bed'));
  $('btn-do-alloc-bed').addEventListener('click', allocateBed);

  /* Billing module */
  $('btn-gen-bill').addEventListener('click', () => openModal('modal-gen-bill'));
  $('btn-save-bill').addEventListener('click', generateBill);
  $('bill-filter').addEventListener('change', e => { billFilter=e.target.value; renderBilling(); });

  /* Log module */
  $('log-search').addEventListener('input', e => { logSearch=e.target.value.toLowerCase(); renderLogs(); });
  $('btn-add-log').addEventListener('click', addLogEntry);
  $('btn-clear-log-search').addEventListener('click', () => { logSearch=''; $('log-search').value=''; renderLogs(); });
});
