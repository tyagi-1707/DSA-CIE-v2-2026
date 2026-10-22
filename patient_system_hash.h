#ifndef PATIENT_SYSTEM_HASH_H
#define PATIENT_SYSTEM_HASH_H

#include <algorithm>
#include <cmath>
#include <map>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "record_struct.h"
#include "hash_map.h"
#include "pq.h"
#include "benchmark.h"

using namespace std;

// ==================== PATIENT SYSTEM (HASH MAP INDEXED) ====================
class PatientSystemHash
{
    vector<rec> records;
    mutable PatientHashMap idMap;
    MaxHeap urgentMax;
    MinHeap urgentMin;

    void rebuildQueues()
    {
        urgentMax.clear();
        urgentMin.clear();

        for (auto& r : records)
        {
            urgentMax.insert(r.priority, r);
            urgentMin.insert(r.priority, r);
        }
    }

    auto getKey(string field)
    {
        return [field](const rec& r) -> long long
        {
            if (field == "id") return r.pID;
            if (field == "age") return r.pAge;
            if (field == "diagnostic") return r.diagnosticValue;
            if (field == "priority") return r.priority;
            return r.pID;
        };
    }

    auto getLess(string field)
    {
        return [field](const rec& x, const rec& y)
        {
            if (field == "time") return x.time < y.time;
            if (field == "department") return x.department < y.department;
            if (field == "age") return x.pAge < y.pAge;
            if (field == "diagnostic") return x.diagnosticValue < y.diagnosticValue;
            if (field == "priority") return x.priority < y.priority;
            return x.pID < y.pID;
        };
    }

public:
    PatientSystemHash() = default;

    bool add(rec r, string& msg)
    {
        if (r.pID < 0)
        {
            msg = "Patient ID must be non-negative.";
            return false;
        }

        if (r.pAge < 0 || r.pAge > 130)
        {
            msg = "Age must be between 0 and 130.";
            return false;
        }

        if (r.priority < 0 || r.priority > 5)
        {
            msg = "Triage priority must be 0..5 (5 = most urgent).";
            return false;
        }

        Metrics m;
        bool isDuplicate = idMap.contains(r.pID);
        idMap.insert(r, m);
        records.push_back(r);
        rebuildQueues();

        msg = isDuplicate ? "Record added; duplicate patient ID detected and retained." : "Record added successfully.";
        return true;
    }

    bool remove(long long id, string& msg)
    {
        if (!idMap.contains(id))
        {
            msg = "Patient ID not found.";
            return false;
        }

        idMap.del(id);
        records.erase(
            remove_if(
                records.begin(),
                records.end(),
                [&](const rec& r)
                {
                    return r.pID == id;
                }
            ),
            records.end()
        );

        rebuildQueues();
        msg = "All records with patient ID removed successfully.";
        return true;
    }

    // Searches records using Hash Map in O(1) average time
    vector<rec> find(long long id) const
    {
        return idMap.getRecords(id);
    }

    vector<rec> find(long long id, Metrics& m) const
    {
        return idMap.getRecords(id, m);
    }

    vector<pair<long long, int>> duplicates() const
    {
        return idMap.getDuplicates();
    }

    vector<rec> all() const
    {
        return records;
    }

    vector<pair<string, int>> departments() const
    {
        map<string, int> d;
        for (auto& r : records)
        {
            d[r.department]++;
        }
        return {d.begin(), d.end()};
    }

    vector<rec> sorted(string field, string alg, bool& ok)
    {
        auto a = records;
        ok = true;

        if (alg == "Pigeonhole Sort" && (field == "time" || field == "department"))
        {
            ok = false;
            return a;
        }
        auto less = getLess(field);
        auto key = getKey(field);

        try
        {
            SortResult result = runSort(alg, a, less, key);
            ok = result.success;
        }
        catch (...)
        {
            ok = false;
        }
        return a;
    }

    pair<int, rec> nextUrgentMax()
    {
        return urgentMax.pop();
    }

    pair<int, rec> nextUrgentMin()
    {
        return urgentMin.pop();
    }

    pair<int, rec> nextUrgent()
    {
        return urgentMax.pop();
    }

    size_t size() const
    {
        return records.size();
    }

    const PatientHashMap& getHashMap() const
    {
        return idMap;
    }
};

#endif // PATIENT_SYSTEM_HASH_H
