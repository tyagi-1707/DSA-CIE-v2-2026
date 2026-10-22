#ifndef PATIENT_SYSTEM_H
#define PATIENT_SYSTEM_H

using namespace std;

#include <algorithm>
#include <cmath>
#include <map>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>
#include "record_struct.h"
#include "sbbst_v2.h"
#include "pq.h"
#include "sorting_algorithms.h"

class PatientSystem {
    vector<rec> records;
    AVLSBTREE idTree;
    unordered_map<long long,vector<size_t>> idIndex;
    MaxHeap urgentMax; MinHeap urgentMin;
    int arrival=0;
    void rebuild(){
        idTree.~AVLSBTREE(); new(&idTree) AVLSBTREE();
        idIndex.clear(); urgentMax.clear();urgentMin.clear();
        for(size_t i=0;i<records.size();++i){idIndex[records[i].pID].push_back(i);if(idIndex[records[i].pID].size()==1)idTree.head=idTree.insert(idTree.head,records[i]);}
        // Rebuild queues using current records. Larger priority means more urgent in MaxHeap.
        for(auto &r:records){urgentMax.insert(r.priority,r);urgentMin.insert(r.priority,r);}
    }
public:
    bool add(rec r,string&msg){
        if(r.pID<0){msg="Patient ID must be non-negative.";return false;}
        if(r.pAge<0||r.pAge>130){msg="Age must be between 0 and 130.";return false;}
        if(r.priority<0||r.priority>5){msg="Triage priority must be 0..5 (5 = most urgent).";return false;}
        records.push_back(r);rebuild();
        msg=idIndex[r.pID].size()>1?"Record added; duplicate patient ID detected and retained.":"Record added.";
        return true;
    }
    bool remove(long long id,string&msg){
        auto it=idIndex.find(id);if(it==idIndex.end()){msg="Patient ID not found.";return false;}
        records.erase(remove_if(records.begin(),records.end(),[&](const rec&r){return r.pID==id;}),records.end());rebuild();msg="All records with that patient ID removed.";return true;
    }
    vector<rec> find(long long id)const{
        vector<rec> out;
        // AVL lookup locates the canonical record in O(log n); the hash index
        // returns all records when duplicate IDs have deliberately been retained.
        if(!idTree.find(id)) return out;
        auto it=idIndex.find(id); if(it!=idIndex.end()) for(size_t i:it->second) if(i<records.size()) out.push_back(records[i]);
        return out;
    }
    vector<rec> all()const{return records;}
    vector<pair<string,int>> departments()const{map<string,int> d;for(auto&r:records)d[r.department]++;return {d.begin(),d.end()};}
    vector<pair<long long,int>> duplicates()const{map<long long,int>d;for(auto&r:records)d[r.pID]++;vector<pair<long long,int>>o;for(auto [id,n]:d)if(n>1)o.push_back({id,n});return o;}
    vector<rec> sorted(string field,string alg,bool&ok){
        auto a=records;
        auto key=[&](const rec&r)->long long{
            if(field=="id")return r.pID;if(field=="age")return r.pAge;if(field=="diagnostic")return r.diagnosticValue;
            if(field=="priority")return r.priority;
            if(field=="department")return 0;return r.pID;
        };
        auto less=[&](const rec&x,const rec&y){
            if(field=="time")return x.time<y.time;if(field=="department")return tie(x.department,x.pID)<tie(y.department,y.pID);
            return key(x)<key(y);
        };
        Metrics m;
        try{
            if(alg=="Tim Sort")timSort(a,less,m);
            else if(alg=="Pigeonhole Sort"){if(field=="time"||field=="department")ok=false;else ok=pigeonholeSort(a,key,m);}
            else if(alg=="Comb Sort")combSort(a,less,m);
            else if(alg=="Cocktail Shaker Sort")cocktailSort(a,less,m);
            else if(alg=="Tree Sort"){
                if(field=="id"){
                    // Inorder traversal of the AVL yields sorted unique IDs; expand each ID
                    // through the duplicate-aware index so no duplicate record disappears.
                    vector<rec> unique=idTree.treesort(); a.clear();
                    for(const rec& u:unique){auto matches=find(u.pID);a.insert(a.end(),matches.begin(),matches.end());}
                }
                else {multimap<pair<long long,long long>,rec> t;for(auto&r:a)t.emplace(make_pair(key(r),r.pID),r);size_t i=0;for(auto&v:t)a[i++]=v.second;}
            }
            else if(alg=="Gnome Sort")gnomeSort(a,less,m);
            else if(alg=="Stooge Sort")stoogeSort(a,less,m);
        }catch(...){ok=false;}
        return a;
    }
    pair<int,rec> nextUrgent(){return urgentMax.pop();}
    size_t size()const{return records.size();}
};

#endif // PATIENT_SYSTEM_H
