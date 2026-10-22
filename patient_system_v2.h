#ifndef PATIENT_SYSTEM_H
#define PATIENT_SYSTEM_H

#include<algorithm>
#include<cmath>
#include<map>
#include<string>
#include<tuple>
#include<utility>
#include<vector>

#include"record_struct.h"
#include"sbbst_v2.h"
#include"hash_map.h"
#include"pq.h"
#include"benchmark.h"

using namespace std;

class PatientSystem
{
    vector<rec> records;
    mutable AVLSBTREE idTree;
    mutable PatientHashMap idMap;
    MaxHeap urgentMax;
    MinHeap urgentMin;

    void rebuildQueues()
    {
        urgentMax.clear();
        urgentMin.clear();

        for(auto&r:records)
        {
            urgentMax.insert(r.priority,r);
            urgentMin.insert(r.priority,r);
        }
    }//--> :)

    auto getKey(string field)
    {
        return [field](const rec&r)->long long
        {
            if(field=="id")
            {
                return r.pID;
            }
            if(field=="age")
            {
                return r.pAge;
            }
            if(field=="diagnostic")
            {
                return r.diagnosticValue;
            }
            if(field=="priority")
            {
                return r.priority;
            }

            return r.pID;
        };
    }//--> :)

    auto getLess(string field)
    {
        return [field](const rec&x,const rec&y)
        {
            if(field=="time")
            {
                return x.time<y.time;
            }

            if(field=="department")
            {
                return x.department<y.department;
            }

            if(field=="age")
            {
                return x.pAge<y.pAge;
            }

            if(field=="diagnostic")
            {
                return x.diagnosticValue<y.diagnosticValue;
            }

            if(field=="priority")
            {
                return x.priority<y.priority;
            }

            return x.pID<y.pID;
        };
    }//--> :)

public:
    bool add(rec r,string&msg)
    {
        if(r.pID<0)
        {
            msg="Patient ID must be non-negative.";
            return false;
        }

        if(r.pAge<0||r.pAge>130)
        {
            msg="Age must be between 0 and 130.";
            return false;
        }

        if(r.priority<0||r.priority>5)
        {
            msg="Triage priority must be 0..5 (5 = most urgent).";
            return false;
        }

        Metrics m;
        bool isDuplicate=(idTree.search(idTree.head,r.pID)!=nullptr);
        idTree.head=idTree.insert(idTree.head,r,m);
        idMap.insert(r);
        records.push_back(r);
        rebuildQueues();

        msg=isDuplicate?"Record added; duplicate patient ID detected and retained.":"Record added successfully.";
        return true;
    }//--> :)

    bool remove(long long id,string&msg)
    {
        node*found=idTree.search(idTree.head,id);

        if(found==nullptr)
        {
            msg="Patient ID not found.";
            return false;
        }

        idTree.del(id);
        idMap.del(id);
        records.erase(
            remove_if(
                records.begin(),
                records.end(),
                [&](const rec&r)
                {
                    return r.pID==id;
                }
            ),
            records.end()
        );

        rebuildQueues();
        msg="All records with patient ID removed successfully.";
        return true;
    }//--> :)

    // Search via AVL Tree O(log N)
    vector<rec> find(long long id)const
    {
        node*found=idTree.search(idTree.head,id);

        if(found!=nullptr)
        {
            return found->records;
        }

        return {};
    }// -> :)

    // Search via Hash Map O(1) average
    vector<rec> findByHashMap(long long id)const
    {
        return idMap.getRecords(id);
    }

    vector<rec> findByHashMap(long long id,Metrics&m)const
    {
        return idMap.getRecords(id,m);
    }

    const PatientHashMap& getHashMap()const
    {
        return idMap;
    }

    const AVLSBTREE& getAvlTree()const
    {
        return idTree;
    }

    vector<pair<long long,int>> duplicates()const
    {
        return idTree.getDuplicates();
    }// -> :)

    vector<rec> all()const
    {
        return records;
    }//--> :)

    vector<pair<string,int>> departments()const
    {
        map<string,int> d;

        for(auto&r:records)
        {
            d[r.department]++;
        }

        return {d.begin(),d.end()};
    }//--> :)

    vector<rec> sorted(string field,string alg,bool&ok)
    {
        auto a=records;
        ok=true;

        if(alg=="Pigeonhole Sort"&&(field=="time"||field=="department"))
        {
            ok=false;
            return a;
        }
        auto less=getLess(field);
        auto key=getKey(field);
        try
        {
            SortResult result=runSort(alg,a,less,key);
            ok=result.success;
        }
        catch(...)
        {
            ok=false;
        }
        return a;
    }//TO EXPLAIN: This function sorts the records based on a specified field and algorithm. It first checks if the chosen algorithm is compatible with the field. If not, it sets `ok` to false and returns the unsorted records. Otherwise, it retrieves the appropriate comparison and key functions for sorting, attempts to sort the records, and updates `ok` based on the success of the sorting operation.

    vector<SortResult> benchmarkReport(string field)
    {//RE
        vector<SortResult> report;
        vector<string> algorithms=
        {
            "Tim Sort",
            "Pigeonhole Sort",
            "Comb Sort",
            "Cocktail Shaker Sort",
            "Tree Sort",
            "Gnome Sort",
            "Stooge Sort"
        };
        auto less=getLess(field);
        auto key=getKey(field);
        for(const string&alg:algorithms)
        {
            if(alg=="Pigeonhole Sort"&&(field=="time"||field=="department"))
            {
                continue;
            }

            auto data=records;
            try
            {
                report.push_back(runSort(alg,data,less,key));
            }
            catch(...)
            {
                report.push_back(
                    SortResult
                    {
                        alg,
                        0,
                        0,
                        0,
                        "Unknown",
                        "Unknown",
                        false,
                        0
                    }
                );
            }
        }

        return report;
    }

    pair<int,rec> nextUrgent()
    {
        if(urgentMax.empty())
        {
            return {-1,{}};
        }
        pair<int,rec> patient=urgentMax.pop();
        long long id=patient.second.pID;
        urgentMin.delete_by_id(id);
        return patient;
    }

    pair<int,rec> nextUrgentMin()
    {
        if(urgentMin.empty())
        {
            return {-1,{}};
        }
        pair<int,rec> patient=urgentMin.pop();
        long long id=patient.second.pID;
        urgentMax.delete_by_id(id);
        return patient;
    }

    vector<pair<int,rec>> getUrgentQueue()const
    {
        return urgentMax.get_heap();
    }

    size_t size()const
    {
        return records.size();
    }
};

#endif // PATIENT_SYSTEM_H
