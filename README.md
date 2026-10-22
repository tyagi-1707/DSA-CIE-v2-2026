# Hospital Patient Record Prioritization and Medical Data Analysis System

## Build and run
Requirements: a C++17 compiler (GCC/Clang).

```bash
g++ -std=c++17 -O2 main.cpp -o hospital_system
./hospital_system
```

Or use CMake:
```bash
cmake -S . -B build
cmake --build build
./build/hospital_system
```

The menu is the integrated console frontend for the C++ backend. It supports adding/listing records, searching by patient ID, deleting all records for an ID, duplicate detection, department summaries, sorting, triage dequeue, and benchmarking.

## Existing files integrated
- `record_struct.h`: central patient record, with safe default field values.
- `sbbst_v2.h`: AVL index keyed by patient ID. It supports insertion, search, deletion, inorder traversal and cleanup.
- `hash_map.h`: Hash Map index keyed by patient ID (`PatientHashMap`). Provides O(1) average lookup, duplicate retention, dynamic rehashing, and deletion.
- `patient_system_hash.h`: Patient record management system powered entirely by `PatientHashMap` instead of AVL tree.
- `pq.h`: custom MaxHeap and MinHeap. Larger priority wins in MaxHeap; smaller priority wins in MinHeap; ties are FIFO.
- `sorting_algorithms.h`: the seven sorting algorithms and shared comparison/write counters.
- `benchmark.h`: benchmark result structure and the generic single-algorithm benchmark runner.
- `patient_system.h`: patient record management, AVL/hash indexing, duplicate detection, department summaries, sorting integration, and triage queue coordination.
- `record_io.h`: console input, output, and lowercase helper functions.
- `main.cpp`: console menu and orchestration only.

The headers are decoupled to avoid the old circular include (`pq.h` included `sbbst.h`, while the AVL header included `pq.h`). `sbbst_v2.h` no longer includes the priority queue.

## Triage policy
Priority is an integer from 0 through 5. **5 means most urgent** in this demonstration, so the MaxHeap returns the highest score first. This is a configurable demonstration policy, not a medical decision rule. A real clinical system must use an approved, clinician-defined triage protocol and should not infer priority from sorting by age, ID, appointment time, diagnosis, or billing fields. Equal priority is first-in, first-out.

## Sorting
Supported algorithms: Tim Sort (run-based insertion + merge implementation), Pigeonhole Sort, Comb Sort, Cocktail Shaker Sort, Tree Sort, Gnome Sort and Stooge Sort.

Supported sort fields: `id`, `age`, `time`, `diagnostic`, `priority`, `department`. Appointment times should be entered consistently as `YYYY-MM-DD HH:MM` so string order matches chronological order.

Pigeonhole Sort is restricted to integer-valued fields and refuses a key span above 100,000. Its implementation uses a vector of buckets, retaining equal-key input order. For age, a practical bounded domain is 0–130. The benchmark currently uses generated ages (0–100) and declares a maximum key span of 100,000.

Stooge Sort has time complexity approximately `O(n^(log_1.5 3))` (about `O(n^2.71)`) and is only run for up to 64 records. It is included for demonstration, not practical workloads.

## Benchmarking
Option 9 runs all seven algorithms against random, nearly sorted, reverse-sorted, and duplicate-heavy age datasets. It prints wall-clock time, counted comparisons/writes where instrumented, a space note, and a stability classification. Timing is machine- and dataset-dependent. Treat small timings as noisy.

**Measurement caveat:** the current benchmark's comparison/write counters are algorithm instrumentation, not hardware-level operation counts; Tree Sort's standard-library multimap internals are not instrumented, so its comparison count is not comparable to the hand-instrumented algorithms. The space field is a complexity note, not a runtime memory measurement. For a formal report, add a counting BST/AVL comparator and measure peak allocations, and test with repeated runs.

## Duplicate IDs and indexing
Duplicate patient IDs are retained and reported rather than silently discarded. The AVL stores one canonical entry per unique ID; the duplicate-aware index returns every matching record. Deleting an ID removes all records with that ID. The current program keeps records in memory only; data is lost on exit.

## Scope and limitations
This is an educational DSA demonstration, not a production hospital system. It has no authentication, encryption, persistence, audit log, network API or browser GUI. It does not make clinical recommendations. Before using it for a formal submission, review the benchmark caveats and consider adding CSV persistence and a browser frontend if the instructor specifically expects a graphical web frontend.


## Modular backend layout

The backend is split into small headers so each concern can be reviewed independently:

| Header | Responsibility |
|---|---|
| `record_struct.h` | Shared patient record model |
| `pq.h` | Custom MaxHeap and MinHeap |
| `sbbst_v2.h` | AVL tree implementation |
| `sorting_algorithms.h` | Seven sorting implementations and metrics |
| `benchmark.h` | Benchmark runner and result metadata |
| `patient_system.h` | Record lifecycle, indexes, reports, and orchestration |
| `record_io.h` | Console input/output helpers |
| `main.cpp` | Main menu and user interaction |

Headers are self-contained with include guards. `pq.h` and `sbbst_v2.h` do not include each other, preventing the circular include from the original files. Compile `main.cpp` as before; no separate compilation step is needed for header-only modules.
