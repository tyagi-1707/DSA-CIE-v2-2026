# Benchmark & Stability Analysis: Gnome Sort & Cocktail Shaker Sort

**Author:** Shrinidhi Sardeshmukh (Shri)  
**Assigned Algorithms:** Gnome Sort, Cocktail Shaker Sort  
**Component:** Administrative Scheduling Engine & Benchmarking Harness

---

## 1. Algorithmic Comparison

| Algorithm | Category | Best Time | Worst Time | Space Complexity | Stability |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **Gnome Sort** | Single-Pointer Adaptive Exchange | $O(N)$ | $O(N^2)$ | $O(1)$ In-Place | **Stable** |
| **Cocktail Shaker Sort** | Bidirectional Bubble Sort | $O(N)$ | $O(N^2)$ | $O(1)$ In-Place | **Stable** |

---

## 2. Dataset Performance & Observations

### A. Nearly-Sorted Datasets (Real-world Hospital OPD Model)
- **Finding:** Both Gnome Sort and Cocktail Shaker Sort achieve near-linear $O(N)$ execution speed.
- **Cocktail Shaker:** The early-termination `swapped` flag exits on the second pass when minimal inversions are present.
- **Gnome Sort:** Moves sequentially forward without triggering backwards swap loops.

### B. Duplicate-Heavy Datasets (Triage & Age Bucketing)
- **Stability Enforcement:** Controlled strictly by `!less(a[i], a[i-1])`.
- Inversions are never triggered on equivalent keys, guaranteeing strict FIFO arrival order preservation.

### C. Reverse-Sorted Datasets (Stress Benchmark)
- Worst-case quadratic bounds observed due to maximal adjacent swap penalties.
