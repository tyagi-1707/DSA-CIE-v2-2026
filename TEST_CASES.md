# Hospital Patient Record System - Test Cases Documentation

This document describes the test cases created for verifying the Hospital Patient Record Prioritization and Medical Data Analysis System.

---

## 1. Streamlined Patient Record Model (`record_struct.h`)

The record structure was simplified by removing 5 redundant fields (`admissionDate`, `dischargeDate`, `status`, `doctorID`, and `billAmount`), leaving the 7 core required fields:

| Field | Type | Purpose |
| :--- | :--- | :--- |
| `pID` | `long long` | Key for AVL Tree indexing, duplicate detection, and search/deletion. |
| `pAge` | `int` | Age (0–130) for sorting and bounded Pigeonhole Sort. |
| `time` | `std::string` | Appointment timestamp (`YYYY-MM-DD HH:MM`) for chronological scheduling. |
| `department` | `std::string` | Clinical department for department-wise summary aggregation. |
| `diagnosis` | `std::string` | Diagnostic description for patient records and triage queue output. |
| `diagnosticValue` | `int` | Integer metric for numerical sorting and benchmark comparisons. |
| `priority` | `int` | Clinical triage priority (`0` = routine, `5` = emergency / critical). |
| `c` | `int` | Internal arrival sequence counter for FIFO tie-breaking in priority queues. |

---

## 2. Automated Test Suite (`tests/test_suite.cpp`)

The automated test suite runs **63 programmatic assertions** across 12 test suites:

| Suite # | Test Category | Scenarios Covered |
| :--- | :--- | :--- |
| **1** | **Input Validation & Boundaries** | Valid additions, negative ID rejection, age lower bound (`0`), age upper bound (`130`), invalid negative age (`-1`), out-of-range age (`131`), priority bounds (`0..5`), negative priority (`-1`), priority overflow (`6`). |
| **2** | **Duplicate Detection & Multi-Record AVL** | Flags duplicate ID on second visit, retains all encounters in AVL node, confirms duplicate report with exact frequency, retrieves all records for patient ID, handles missing ID query. |
| **3** | **AVL Deletion & Cleanup** | Removes all encounter records for specified patient ID, clears duplicate tracking, preserves unaffected patients, properly rejects re-deletion of non-existent ID. |
| **4** | **Clinical Triage Queues & FIFO Tie-Breaking** | MaxHeap extracts priority 5 first. **FIFO tie-break verification**: between two priority 5 patients, the first-arrived patient is dequeued first. MinHeap extracts priority 1 first. |
| **5** | **Triage vs Sorting Separation** | Explicitly verifies that sorting records by age, ID, or appointment time does not alter or corrupt the triage priority queue. |
| **6** | **Department Summary & Chronological Schedule** | Aggregates counts across departments (Cardiology, Pediatrics, Neurology). Arranges appointments chronologically from earliest to latest. |
| **7** | **All 7 Sorting Algorithms Correctness** | Validates Tim Sort, Pigeonhole Sort, Comb Sort, Cocktail Shaker Sort, Tree Sort, Gnome Sort, and Stooge Sort on test records. |
| **8** | **Algorithm Stability** | Confirms that stable algorithms (Tim Sort, Pigeonhole Sort, Tree Sort) preserve original arrival order for records with duplicate keys. |
| **9** | **Pigeonhole Range & Field Restrictions** | Rejects non-integer fields (`time`, `department`). Enforces 100,000 maximum key range bound. |
| **10** | **Stooge Sort Scalability Restriction** | Executes successfully for $N \le 64$. Gracefully rejects/refuses datasets $N > 64$. |
| **11** | **Benchmark Distributions** | Runs benchmarking harness across Random, Nearly-Sorted, Reverse-Sorted, and Duplicate-Heavy datasets. |
| **12** | **Hash Map Indexing & Search Verification** | Validates separate-chaining `PatientHashMap` (`hash_map.h`) insertion, $O(1)$ search, duplicate identification, deletion, dynamic rehashing ($N=150$), dual equivalence with AVL Tree in `PatientSystem`, and end-to-end `PatientSystemHash` (`patient_system_hash.h`). |

---

## 3. End-to-End CLI Simulation Test Inputs (`test_inputs/`)

1. **`test_inputs/01_full_workflow.txt`**:
   - Simulates complete interactive clinical workflow: adds 6 patient records (including emergency cases and readmissions for patient ID 101), lists records, inspects duplicate detection, queries AVL tree by ID, generates department summaries, sorts by age and diagnostic value, inspects triage queue, dequeues urgent patients via MaxHeap (confirming FIFO tie-breaking), and runs Stooge Sort analysis.
2. **`test_inputs/02_duplicate_lifecycle.txt`**:
   - Simulates duplicate visit lifecycle: adds 3 visits for ID 201 and 1 visit for ID 202, verifies duplicate detection, searches ID 201 to see all 3 visits, deletes ID 201 (removing all visits), and verifies ID 202 remains intact.
3. **`test_inputs/03_benchmarks.txt`**:
   - Runs the multi-algorithm benchmark harness across all 4 dataset types and exits.

---

## 4. How to Run the Tests

To run all automated test suites and CLI simulations with one command:

```bash
./run_all_tests.sh
```

Or run individual components:

```bash
# Run the 63-assertion test suite:
./build/test_suite

# Run interactive simulation with sample data:
./hospital_system < test_inputs/01_full_workflow.txt
```
