================================================================
  SMART HOSPITAL MANAGEMENT SYSTEM
  Viva Questions & Answers (50 Questions)
================================================================

================================================================
SECTION A — C LANGUAGE FUNDAMENTALS
================================================================

Q1. Why did you choose C for this project?
A:  C offers direct memory control via pointers and malloc/free,
    efficient struct-based data layout, binary file I/O with
    fread/fwrite, and low-level POSIX API access (termios for
    password hiding). These make it ideal for a systems project.

Q2. What is the difference between struct and typedef struct?
A:  'struct Patient p;' requires the 'struct' keyword every time.
    'typedef struct { ... } Patient;' creates an alias so you
    can write 'Patient p;' directly. All structs in this project
    use typedef for cleaner code.

Q3. Explain how pointers are used in this project.
A:  Pointers are used to:
    - Pass structs by reference to avoid copying (e.g., void
      displayPatientCard(Patient *p))
    - Build the linked list (LogNode *next, *head, *tail)
    - Dynamic memory in merge sort (Patient *L = malloc(...))
    - Return heap-allocated nodes from ll_insert()

Q4. What is the difference between pass-by-value and
    pass-by-reference in C?
A:  C is always pass-by-value. To simulate pass-by-reference,
    we pass a pointer. E.g., void ll_insert(LogList *list, ...)
    receives the address of the list so changes persist in the
    caller's scope.

Q5. What is dynamic memory allocation? Where is it used?
A:  malloc() requests heap memory at runtime. Used in:
    - mergeSortName(): Patient *L = malloc(n1 * sizeof(Patient))
    - ll_insert(): LogNode *node = malloc(sizeof(LogNode))
    All malloc'd memory is freed with free() to prevent leaks.

Q6. What happens if malloc() returns NULL?
A:  The allocation failed (out of memory). In this project we
    check: if (!L || !R) { free(L); free(R); return; }
    and for the linked list: if (!node) { print error; return; }

Q7. Explain the difference between stack and heap memory.
A:  Stack: automatic, fast, limited size, freed on function
    return. Local variables live here.
    Heap: manual, slower, large, persists until free().
    malloc() allocates from heap. Linked list nodes and merge
    sort temp arrays use the heap.

Q8. What are header guards and why are they used?
A:  #ifndef PATIENT_H / #define PATIENT_H / #endif prevents a
    header from being included multiple times in the same
    translation unit, avoiding duplicate declaration errors.

Q9. What is the purpose of extern in this project?
A:  extern declares a variable defined in another .c file.
    E.g., in bed.c: 'extern Patient patients[];' tells the
    compiler this array exists in patient.c without redefining.

Q10. What is the difference between fread/fwrite and fprintf/fscanf?
A:  fread/fwrite operate on binary data (raw bytes). They are
    faster and preserve exact struct layout. fprintf/fscanf work
    on formatted text. This project uses binary I/O because it
    is more compact and doesn't require parsing.

================================================================
SECTION B — DATA STRUCTURES
================================================================

Q11. What data structures did you implement in this project?
A:  1. Static arrays (Patient[], Doctor[], etc.)
    2. Singly linked list (LogList/LogNode — activity log)
    3. Min-heap / priority queue (EmergencyQueue)
    4. Structures (all records)
    5. Enumerations (status types)

Q12. Explain the linked list implementation.
A:  LogList has head and tail pointers and a count.
    Each LogNode has: entry text, timestamp, sequence number,
    and a *next pointer. insert() appends at tail in O(1).
    delete() removes from head in O(1). A tail pointer makes
    append O(1) instead of O(n).

Q13. What is a min-heap and how does it work?
A:  A min-heap is a complete binary tree where every parent's
    key ≤ its children's keys. Stored as an array:
    parent(i)=(i-1)/2, left(i)=2i+1, right(i)=2i+2.
    The minimum (root) is always at index 0. Insert appends
    and heapifyUp; extract removes root, moves last to root,
    and heapifyDown restores the property.

Q14. Why use a min-heap for the emergency queue?
A:  Priority=1 (CRITICAL) must be served before priority=4 (LOW).
    A min-heap always keeps the minimum priority at the root,
    so extracting it is O(log n). A sorted array would need O(n)
    insertion. A min-heap gives O(log n) for both operations.

Q15. What is the time complexity of heap operations?
A:  Insert: O(log n) — heapifyUp traverses at most tree height.
    Extract-min: O(log n) — heapifyDown traverses tree height.
    Peek (view min): O(1) — just read data[0].
    Build heap from n elements: O(n).

Q16. What is the difference between a linked list and an array?
A:  Array: O(1) access by index, fixed size, contiguous memory.
    Linked list: O(n) access, dynamic size, scattered in heap,
    O(1) insert/delete at head/tail with pointer manipulation.
    Arrays are used for patient/doctor records (fast indexed
    access). Linked list is used for the log (dynamic growth,
    FIFO deletion).

Q17. What is a singly linked list vs doubly linked list?
A:  Singly: each node has one *next pointer. Traversal is
    forward only. This project uses singly linked list.
    Doubly: each node has *next and *prev. Allows backward
    traversal without recursion but uses more memory.

Q18. How does recursive reverse traversal of the linked list work?
A:  ll_displayReverse(node): if node==NULL return; recurse on
    node->next; then print node. The call stack unwinds from
    the tail backwards, printing newest entries first.
    Stack depth = number of nodes = O(n) space.

Q19. Why are global arrays used instead of dynamic arrays?
A:  For simplicity and to avoid realloc complexity. The
    MAX_PATIENTS=200 limit is realistic for a small hospital.
    A production system would use dynamic arrays or a database.

Q20. How is the bed numbering scheme designed?
A:  ICU: 1001–1020, General: 2001–2100. The range is used to
    determine bed type without storing an extra flag:
    if (bedNo >= 1001 && bedNo <= 1020) → ICU.
    This encodes type in the number itself.

================================================================
SECTION C — ALGORITHMS
================================================================

Q21. Which sorting algorithms did you implement and why?
A:  Merge Sort for patient names: O(n log n), stable, good for
    string comparison. Bubble Sort for patient age: O(n²),
    simple, demonstrates contrast. Insertion Sort for doctor
    specializations: O(n²) but O(n) for nearly-sorted data,
    small dataset (≤50 doctors).

Q22. Explain merge sort with an example.
A:  Input: [Zara, Alice, Mike]
    Split: [Zara, Alice] | [Mike]
    Split: [Zara] | [Alice] | [Mike]
    Merge: [Alice, Zara] | [Mike]
    Merge: [Alice, Mike, Zara] ✓
    Key: merge compares heads of two sorted halves, placing
    the smaller into the result array.

Q23. What is the space complexity of merge sort?
A:  O(n) — requires temporary arrays L[] and R[] of combined
    size n for the merge step. These are malloc'd and free'd
    at each level. In-place merge sort exists but is complex.

Q24. When does bubble sort perform at O(n)?
A:  When the array is already sorted. Adding an early-exit flag:
    if no swaps occurred in a full pass, the array is sorted
    and we break. This project's basic version is O(n²) always.

Q25. How does linear search work in searchPatient()?
A:  For name search: converts both query and patient name to
    uppercase, then uses strstr() to check if the query is a
    substring of the patient name. This enables partial matching.
    For ID search: exact equality check on patientID.

Q26. Why not use binary search for patients?
A:  Binary search requires a sorted array. The patient array
    is sorted by insertion time (newest last), not by ID or
    name, unless the user explicitly calls sort. Maintaining
    a sorted insert order would require shifting elements on
    every add — O(n) — which outweighs the search benefit for
    small datasets.

Q27. What is the XOR cipher and is it secure?
A:  XOR cipher applies a fixed key byte to each character.
    XOR is its own inverse so encrypt=decrypt. It is NOT
    cryptographically secure — a known-plaintext attack breaks
    it instantly. For this academic project it obfuscates
    passwords on disk. Production systems use bcrypt/Argon2.

================================================================
SECTION D — FILE HANDLING
================================================================

Q28. Why use binary files instead of text files?
A:  fwrite(struct, size, 1, file) writes the exact byte layout
    of the struct. No parsing, no format conversion, faster I/O,
    and the struct is read back identically with fread(). Text
    files require fprintf/fscanf with format strings and are
    slower.

Q29. What happens if a data file is corrupted?
A:  fread() would read garbage data. The project guards against
    this by clamping count to MAX after reading:
    if (patientCount > MAX_PATIENTS) patientCount = MAX_PATIENTS.
    A proper solution would use a checksum or magic number.

Q30. How is the log file different from data files?
A:  Log file (activity.log) is a text file opened with "a"
    (append mode) — new entries are added without overwriting.
    Data files use "wb"/"rb" (binary write/read), rewriting
    the entire file on each save.

Q31. Why is the entire array rewritten on each save?
A:  For simplicity. fwrite(&count, ...) then fwrite(array, ...)
    writes all records atomically in one call. A more efficient
    approach would seek to a specific record's offset and update
    only that record: fseek(f, offset, SEEK_SET).

Q32. How does backup work?
A:  backupData() calls system("cp -r data/ backup/backup_TIMESTAMP/").
    This uses the OS shell to copy the data directory to a
    timestamped subdirectory. restoreData() copies the latest
    backup back to data/.

================================================================
SECTION E — MODULES
================================================================

Q33. How is duplicate patient ID prevented?
A:  getNextPatientID() scans all existing patients and returns
    maxID + 1. Since IDs only increase, the new ID is always
    unique. IDs are never reused even after discharge.

Q34. How does soft delete work for patients?
A:  Discharged patients have isActive=0 set. They remain in
    the patients[] array and patients.dat. This preserves
    billing history and allows auditing. Active patients are
    shown with isActive=1 filter in reports.

Q35. What happens when a doctor is assigned to a patient?
A:  patients[pIdx].assignedDoctorID = doctorID (patient updated).
    If patient had a previous doctor, that doctor's patientCount
    is decremented. New doctor's patientCount is incremented.
    Both are saved atomically: savePatients() + saveDoctors().

Q36. How is the appointment token number guaranteed unique?
A:  getNextToken() iterates all appointments and finds the
    maximum token, then returns max+1. Even after cancellation,
    cancelled tokens are never reassigned. Unique across program
    restarts because appointments.dat persists all tokens.

Q37. Explain the billing rate determination logic.
A:  Bed type is inferred from bedNumber range:
      1001–1020 → ICU → ₹3000/day
      2001–2100 → General → ₹800/day
    Consultation rate depends on doctor experience:
      experience >= 10 years → Specialist → ₹1000
      else → General → ₹500

Q38. How does the ambulance module track driver details?
A:  Each Ambulance struct stores driverName[60] and
    driverPhone[15] fields. When dispatched, dispatchTime is
    recorded and totalTrips increments. Fleet history is
    maintained in ambulances.dat.

Q39. How does the system prevent double-allocation of a bed?
A:  allocateBed() scans icuBeds[] or genBeds[] for the first
    BED_AVAILABLE entry. Once allocated, it sets status =
    BED_OCCUPIED. The scan skips occupied beds so the same
    bed cannot be allocated twice.

Q40. How is emergency handled log maintained?
A:  When handleNextEmergency() dequeues a patient, it appends
    the record to data/emergency_handled.txt as a text line
    with timestamp. The binary emergency.dat stores the current
    queue; the text file preserves handled history.

================================================================
SECTION F — ADVANCED TOPICS
================================================================

Q41. What is a priority queue? How is it different from a queue?
A:  A regular queue is FIFO — first in, first out, regardless
    of content. A priority queue always serves the element with
    the highest priority first, regardless of insertion order.
    This project's emergency queue serves CRITICAL before HIGH
    even if HIGH arrived first.

Q42. How does POSIX termios hide the password?
A:  tcgetattr() saves current terminal attributes. We modify
    the copy by clearing the ECHO flag (newt.c_lflag &= ~ECHO),
    then apply it with tcsetattr(). Characters typed are sent
    to the program but not displayed. Restore original settings
    after reading.

Q43. Explain the activity log linked list lifecycle.
A:  Startup: ll_loadFromFile() reads last 100 lines from
    activity.log and inserts them into the linked list.
    Runtime: every module calls logActivity() which appends
    to the file AND ll_insert() adds to the in-memory list.
    Shutdown: ll_freeAll() releases all malloc'd nodes to
    prevent memory leaks.

Q44. What is a memory leak and how is it prevented here?
A:  Memory leak: malloc'd memory is never freed, so it
    accumulates until the OS reclaims it on process exit.
    Prevention: merge sort temp arrays are free()'d after
    each merge step. ll_freeAll() traverses the linked list
    and free()'s every node. ll_deleteHead() free()'s the
    removed node.

Q45. How does the statistics dashboard work without a database?
A:  It iterates all global arrays at runtime and computes:
    - activePatients: count where isActive==1
    - availDoctors: count where isAvailable==1
    - scheduledAppts: count where status==APPT_SCHEDULED
    - ICU/Gen occupancy: count BED_OCCUPIED in each array
    - Revenue: sum of amountPaid and (total-paid) across bills
    All O(n) scans. No database query needed.

Q46. How does the queue visualizer work?
A:  visualizeQueue() makes a copy of the EmergencyQueue struct,
    then repeatedly calls pqExtractMin() on the copy to get
    patients in priority order. It renders each as an ASCII
    box with colour-coded priority. The original queue is
    unchanged because we work on the copy.

Q47. What would you change to support multiple admin users?
A:  Change Credential from a single struct to a Credential[]
    array. Add username uniqueness check on creation.
    Implement role-based access: admin can access all modules,
    staff can only access patient/appointment modules.

Q48. How would you scale this to 10,000 patients?
A:  Replace static arrays with dynamic arrays using realloc().
    Replace binary flat files with SQLite for indexed queries.
    Use hash table for O(1) patient ID lookup instead of O(n)
    linear search. Add pagination to display functions.

Q49. What is the difference between fopen modes "wb" and "ab"?
A:  "wb": open for binary write — overwrites existing content
    from the start. Used when saving entire data arrays.
    "ab": open for binary append — writes start at end of file.
    Used for adding to logs without overwriting previous entries.

Q50. Explain how heapifyDown restores the heap property.
A:  After placing the last element at the root following
    ExtractMin, heapifyDown compares the node at index i with
    its children at 2i+1 and 2i+2. It finds the smallest of
    the three. If the smallest is a child (not i itself), it
    swaps i with that child and recurses down. Continues until
    the node is smaller than both children or reaches a leaf.
    Guarantees the min-heap property is restored in O(log n).

================================================================
END OF VIVA QUESTIONS DOCUMENT
================================================================
