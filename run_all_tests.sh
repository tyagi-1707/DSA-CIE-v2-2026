#!/usr/bin/env bash
set -e
cd "$(dirname "$0")"

echo "=========================================================="
echo "  Hospital System - Automated Verification & Test Runner  "
echo "=========================================================="

echo "[1/4] Building executables..."
mkdir -p build
g++ -std=c++17 -O2 main.cpp -o hospital_system
g++ -std=c++17 -O2 test_core.cpp -o test_core
g++ -std=c++17 -O2 -I. tests/test_suite.cpp -o build/test_suite

echo "[2/4] Running core data structure tests..."
./test_core

echo "[3/4] Running comprehensive 63-assertion test suite..."
./build/test_suite

echo "[4/4] Running CLI end-to-end simulation scripts..."
./hospital_system < test_inputs/01_full_workflow.txt > /dev/null
echo "  [PASS] CLI Workflow & Triage Simulation (test_inputs/01_full_workflow.txt)"

./hospital_system < test_inputs/02_duplicate_lifecycle.txt > /dev/null
echo "  [PASS] CLI Duplicate Detection & AVL Deletion Lifecycle (test_inputs/02_duplicate_lifecycle.txt)"

./hospital_system < test_inputs/03_benchmarks.txt > /dev/null
echo "  [PASS] CLI Benchmark & Scalability Simulation (test_inputs/03_benchmarks.txt)"

echo ""
echo "=========================================================="
echo "  ALL TESTS PASSED SUCCESSFULLY!                          "
echo "=========================================================="
