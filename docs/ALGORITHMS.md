================================================================
  SMART HOSPITAL MANAGEMENT SYSTEM
  Algorithm Explanation
================================================================

================================================================
1. MERGE SORT — Sort Patients by Name (patient.c)
================================================================

CONCEPT:
  Divide-and-conquer algorithm. Recursively splits the array
  into halves, sorts each half, then merges them back together.

TIME COMPLEXITY:  O(n log n) — best, average, and worst case
SPACE COMPLEXITY: O(n) — temporary arrays for merging

IMPLEMENTATION (mergeSortName):

  mergeSortName(arr, l, r):
    if l >= r: return          // base case: 1 element
    m = (l + r) / 2
    mergeSortName(arr, l, m)   // sort left half
    mergeSortName(arr, m+1, r) // sort right half
    merge(arr, l, m, r)        // merge sorted halves

  merge(arr, l, m, r):
    Copy arr[l..m] → L[]
    Copy arr[m+1..r] → R[]
    Compare L[i] and R[j] using strcasecmp()
    Place smaller into arr[k], advance pointer

EXAMPLE (4 patients):
  Input:  [Zara, Alice, Mike, Bob]
  Split:  [Zara, Alice] | [Mike, Bob]
  Split:  [Zara][Alice] | [Mike][Bob]
  Merge:  [Alice, Zara] | [Bob, Mike]
  Merge:  [Alice, Bob, Mike, Zara]  ← sorted

WHY MERGE SORT for names:
  Names are strings → comparison is O(k) where k=length.
  Merge sort's O(n log n) comparisons makes it efficient.
  Also stable — equal names preserve original order.

================================================================
2. BUBBLE SORT — Sort Patients by Age (patient.c)
================================================================

CONCEPT:
  Repeatedly steps through the array, compares adjacent
  elements, and swaps them if out of order. "Bubbles" large
  values to the end.

TIME COMPLEXITY:  O(n²) worst/average, O(n) best (sorted)
SPACE COMPLEXITY: O(1) — in-place

IMPLEMENTATION (sortPatientsByAge):

  for i = 0 to n-2:
    for j = 0 to n-i-2:
      if patients[j].age > patients[j+1].age:
        swap(patients[j], patients[j+1])

EXAMPLE (ages: [45, 20, 35, 10]):
  Pass 1: [20,35,10,45]  → 45 bubbles to end
  Pass 2: [20,10,35,45]  → 35 in place
  Pass 3: [10,20,35,45]  → sorted

WHY BUBBLE SORT for age:
  Integer comparison is O(1). Included to demonstrate a
  simpler sorting algorithm alongside Merge Sort.
  Acceptable for small datasets (≤200 patients).

================================================================
3. INSERTION SORT — Sort Doctors by Specialization (doctor.c)
================================================================

CONCEPT:
  Builds sorted array one element at a time. For each element,
  finds its correct position by shifting larger elements right.

TIME COMPLEXITY:  O(n²) worst, O(n) best (nearly sorted)
SPACE COMPLEXITY: O(1) — in-place

IMPLEMENTATION (sortDoctorsBySpecialization):

  for i = 1 to n-1:
    key = doctors[i]
    j = i - 1
    while j >= 0 AND strcasecmp(doctors[j].specialization,
                                key.specialization) > 0:
      doctors[j+1] = doctors[j]
      j--
    doctors[j+1] = key

EXAMPLE (specs: [Surgery, Cardiology, Neurology]):
  i=1: key=Cardiology → shift Surgery right
       → [Cardiology, Surgery, Neurology]
  i=2: key=Neurology → shift Surgery right
       → [Cardiology, Neurology, Surgery]

WHY INSERTION SORT for specializations:
  Doctor list is small (≤50). Insertion sort performs
  well on small or nearly-sorted data.

================================================================
4. LINEAR SEARCH — Search Patients and Doctors
================================================================

CONCEPT:
  Sequentially examine each element until a match is found
  or the array is exhausted.

TIME COMPLEXITY:  O(n) worst case
SPACE COMPLEXITY: O(1)

IMPLEMENTATION (searchPatient by name — partial match):

  toUpperStr(query)
  for i = 0 to patientCount-1:
    toUpperStr(copy of patients[i].name)
    if strstr(nameUpper, queryUpper) != NULL:
      display patient          // partial match found

IMPLEMENTATION (getPatientIndex by ID — exact match):

  for i = 0 to patientCount-1:
    if patients[i].patientID == id:
      return i
  return -1

WHY LINEAR Search:
  Data is not sorted by ID (sorted by insertion order),
  so binary search is not applicable without pre-sorting.
  For the data sizes in this project (≤200), O(n) is fast.

================================================================
5. HEAP INSERT — pqInsert() (emergency.c)
================================================================

CONCEPT:
  Add element at the end of the heap array, then restore
  the heap property by "bubbling up" (heapifyUp).

TIME COMPLEXITY:  O(log n)

IMPLEMENTATION:

  pqInsert(pq, ep):
    pq.data[pq.size] = ep     // place at end
    pq.size++
    heapifyUp(pq, pq.size-1)

  heapifyUp(pq, i):
    while i > 0:
      parent = (i-1) / 2
      if pq.data[parent].priority > pq.data[i].priority:
        swap(pq.data[parent], pq.data[i])
        i = parent
      else:
        break

EXAMPLE:
  Insert MEDIUM(3) into heap [CRITICAL(1), HIGH(2)]:
  Array: [1, 2, 3]  → 3's parent is 1 → 3 > 1 → no swap
  Heap maintained ✓

================================================================
6. HEAP EXTRACT MIN — pqExtractMin() (emergency.c)
================================================================

CONCEPT:
  Remove root (minimum = most critical), replace with last
  element, then restore heap by "sinking down" (heapifyDown).

TIME COMPLEXITY:  O(log n)

IMPLEMENTATION:

  pqExtractMin(pq):
    top = pq.data[0]           // save minimum
    pq.data[0] = pq.data[--pq.size]  // move last to root
    heapifyDown(pq, 0)
    return top

  heapifyDown(pq, i):
    while true:
      smallest = i
      l = 2*i+1, r = 2*i+2
      if l < size AND pq.data[l].priority < pq.data[smallest].priority:
        smallest = l
      if r < size AND pq.data[r].priority < pq.data[smallest].priority:
        smallest = r
      if smallest == i: break
      swap(pq.data[i], pq.data[smallest])
      i = smallest

EXAMPLE:
  Extract from [1, 2, 3]:
  Save 1. Move 3 to root: [3, 2].
  HeapifyDown: 3 > 2 → swap → [2, 3]. ✓

================================================================
7. XOR CIPHER — Password Encryption (utility.c)
================================================================

CONCEPT:
  Each character byte is XOR'd with a fixed key byte.
  XOR is its own inverse: (x XOR key) XOR key = x.

KEY: 0x5A (01011010 in binary)

IMPLEMENTATION:

  encryptStr(s):
    key = 0x5A
    for each char c in s:
      c = c XOR key

  decryptStr(s):
    encryptStr(s)   // identical — XOR is symmetric

EXAMPLE:
  'a' (0x61) XOR 0x5A = 0x3B (encrypted)
  0x3B XOR 0x5A = 0x61 = 'a' (decrypted) ✓

WHY XOR cipher:
  Simple, deterministic, reversible, and sufficient for
  this academic project. Obfuscates passwords stored on disk
  so they are not human-readable in the binary file.

================================================================
8. LINKED LIST OPERATIONS (linked_list.c)
================================================================

INSERT AT TAIL — O(1):

  ll_insert(list, entry):
    node = malloc(sizeof(LogNode))
    node.entry = entry
    node.next = NULL
    if list.tail == NULL:
      list.head = node       // first node
      list.tail = node
    else:
      list.tail.next = node  // append to existing chain
      list.tail = node
    list.count++

REVERSE DISPLAY (Recursive) — O(n):

  ll_displayReverse(node):
    if node == NULL: return
    ll_displayReverse(node.next)  // recurse to end first
    print(node)                   // print on the way back

  Call stack unwinds in reverse → newest entries print first.

DELETE HEAD — O(1):

  ll_deleteHead(list):
    old = list.head
    list.head = list.head.next
    if list.head == NULL: list.tail = NULL
    free(old)
    list.count--

LINEAR SEARCH — O(n):

  ll_search(list, keyword):
    cur = list.head
    while cur != NULL:
      if strstr(cur.entry, keyword) != NULL:
        return cur            // first match
      cur = cur.next
    return NULL

================================================================
END OF ALGORITHMS DOCUMENT
================================================================
