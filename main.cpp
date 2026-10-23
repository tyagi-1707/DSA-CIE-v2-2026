#include<algorithm>
#include<chrono>
#include<iomanip>
#include<iostream>
#include<limits>
#include<map>
#include<random>
#include<string>
#include<vector>

#include"record_struct.h"
#include"sbbst_v2.h"
#include"hash_map.h"
#include"pq.h"
#include"sorting_algorithms_v2.h"
#include"benchmark.h"
#include"patient_system_v2.h"
#include"record_io.h"

using namespace std;

void clearInput()
{
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(),'\n');
}

int main()
{
    PatientSystem sys;

    const vector<string> algs=
    {
        "Tim Sort",
        "Pigeonhole Sort",
        "Comb Sort",
        "Cocktail Shaker Sort",
        "Tree Sort",
        "Gnome Sort",
        "Stooge Sort"
    };

    while(true)
    {
        cout<<"\n=== Hospital Patient Record & Prioritization System ===\n";
        cout<<" 1 Add patient record\n";
        cout<<" 2 List all records\n";
        cout<<" 3 Search records by patient ID (AVL tree)\n";
        cout<<" 4 Delete records by patient ID\n";
        cout<<" 5 Detect duplicate patient IDs\n";
        cout<<" 6 Department summary report\n";
        cout<<" 7 Sort records by field\n";
        cout<<" 8 View appointment schedule (chronological order)\n";
        cout<<" 9 Next urgent patient (MaxHeap triage: 5=most urgent)\n";
        cout<<"10 Next urgent patient (MinHeap triage: 0/1=most urgent)\n";
        cout<<"11 View urgent triage queue\n";
        cout<<"12 Benchmark all 7 sorting algorithms on datasets\n";
        cout<<"13 Stooge Sort demonstration & scalability analysis\n";
        cout<<"14 Search records by patient ID (Hash Map index)\n";
        cout<<" 0 Exit\n";
        cout<<"Choice: ";

        int ch;
        if(!(cin>>ch))
        {
            clearInput();
            continue;
        }

        if(ch==0)
        {
            break;
        }
        else if(ch==1)
        {
            rec r=inputRecord();
            string msg;
            if(sys.add(r,msg))
            {
                cout<<msg<<"\n";
            }
            else
            {
                cout<<"Error: "<<msg<<"\n";
            }
        }
        else if(ch==2)
        {
            auto a=sys.all();
            if(a.empty())
            {
                cout<<"No records found.\n";
            }
            else
            {
                for(const auto&r:a)
                {
                    printRecord(r);
                }
                cout<<"Total records: "<<a.size()<<"\n";
            }
        }
        else if(ch==3)
        {
            long long id;
            cout<<"Patient ID to search: ";
            if(!(cin>>id))
            {
                clearInput();
                continue;
            }
            auto a=sys.find(id);
            if(a.empty())
            {
                cout<<"Patient ID "<<id<<" not found.\n";
            }
            else
            {
                cout<<"Found "<<a.size()<<" record(s) for patient ID "<<id<<":\n";
                for(const auto&r:a)
                {
                    printRecord(r);
                }
            }
        }
        else if(ch==4)
        {
            long long id;
            cout<<"Patient ID to delete: ";
            if(!(cin>>id))
            {
                clearInput();
                continue;
            }
            string msg;
            sys.remove(id,msg);
            cout<<msg<<"\n";
        }
        else if(ch==5)
        {
            auto d=sys.duplicates();
            if(d.empty())
            {
                cout<<"No duplicate patient IDs detected.\n";
            }
            else
            {
                cout<<"Duplicate Patient IDs Detected:\n";
                for(const auto&[id,n]:d)
                {
                    cout<<"  Patient ID "<<id<<" occurs "<<n<<" times.\n";
                }
            }
        }
        else if(ch==6)
        {
            auto depts=sys.departments();
            if(depts.empty())
            {
                cout<<"No records in system.\n";
            }
            else
            {
                cout<<"Department-Wise Report:\n";
                for(const auto&[dept,n]:depts)
                {
                    cout<<"  "<<dept<<": "<<n<<" patient record(s)\n";
                }
            }
        }
        else if(ch==7)
        {
            string field,alg;
            cout<<"Field (id/age/time/diagnostic/priority/department): ";
            cin>>field;
            field=lowerStr(field);

            cout<<"Algorithm (Tim/Pigeonhole/Comb/Cocktail/Tree/Gnome/Stooge): ";
            cin>>alg;
            alg=lowerStr(alg);

            map<string,string> names=
            {
                {"tim","Tim Sort"},
                {"pigeonhole","Pigeonhole Sort"},
                {"comb","Comb Sort"},
                {"cocktail","Cocktail Shaker Sort"},
                {"tree","Tree Sort"},
                {"gnome","Gnome Sort"},
                {"stooge","Stooge Sort"}
            };

            if(!names.count(alg))
            {
                cout<<"Unknown algorithm name.\n";
                continue;
            }

            bool ok=true;
            auto a=sys.sorted(field,names[alg],ok);
            if(!ok)
            {
                cout<<"Sort not run: unsupported field for Pigeonhole Sort, key span exceeded, or Stooge dataset > 64.\n";
            }
            else
            {
                cout<<"Sorted "<<a.size()<<" record(s) by "<<field<<" using "<<names[alg]<<":\n";
                for(const auto&r:a)
                {
                    printRecord(r);
                }
            }
        }
        else if(ch==8)
        {
            bool ok=true;
            auto schedule=sys.sorted("time","Tim Sort",ok);
            if(schedule.empty())
            {
                cout<<"No appointments scheduled.\n";
            }
            else
            {
                cout<<"Chronological Appointment Schedule:\n";
                for(const auto&r:schedule)
                {
                    cout<<"  ["<<r.time<<"] ID="<<r.pID<<" | Age="<<r.pAge<<" | Dept="<<r.department<<" | Diagnosis="<<r.diagnosis<<" | Priority="<<r.priority<<"\n";
                }
                cout<<"Total appointments: "<<schedule.size()<<"\n";
            }
        }
        else if(ch==9)
        {
            auto p=sys.nextUrgent();
            if(p.first==-1)
            {
                cout<<"No urgent patients in MaxHeap queue.\n";
            }
            else
            {
                cout<<"Next triage patient (MaxHeap priority "<<p.first<<" - higher value first):\n";
                printRecord(p.second);
            }
        }
        else if(ch==10)
        {
            auto p=sys.nextUrgentMin();
            if(p.first==-1)
            {
                cout<<"No urgent patients in MinHeap queue.\n";
            }
            else
            {
                cout<<"Next triage patient (MinHeap priority "<<p.first<<" - lower value first):\n";
                printRecord(p.second);
            }
        }
        else if(ch==11)
        {
            auto q=sys.getUrgentQueue();
            if(q.empty())
            {
                cout<<"Triage queue is empty.\n";
            }
            else
            {
                cout<<"Current Urgent Triage Queue ("<<q.size()<<" patients waiting):\n";
                for(size_t i=0;i<q.size();i++)
                {
                    cout<<"  #"<<(i+1)<<" Priority="<<q[i].first<<" | ID="<<q[i].second.pID<<" | Dept="<<q[i].second.department<<" | Diagnosis="<<q[i].second.diagnosis<<"\n";
                }
            }
        }
        else if(ch==12)
        {
            int n;
            cout<<"Dataset size (1..10000; Stooge will only run if <=64): ";
            if(!(cin>>n)||n<1||n>10000)
            {
                cout<<"Invalid size.\n";
                clearInput();
                continue;
            }

            vector<rec> base(n);
            mt19937 rng(42);
            uniform_int_distribution<int> age(0,100),diag(0,1000);

            for(int i=0;i<n;i++)
            {
                base[i]=rec{};
                base[i].pID=i+1;
                base[i].pAge=age(rng);
                base[i].diagnosticValue=diag(rng);
            }

            vector<pair<string,vector<rec>>> datasets;
            datasets.push_back({"random",base});

            auto nearly=base;
            sort(nearly.begin(),nearly.end(),[](const rec&a,const rec&b){return a.pAge<b.pAge;});
            for(int i=0;i<n/20;i++)
            {
                swap(nearly[rng()%n],nearly[rng()%n]);
            }
            datasets.push_back({"nearly-sorted",nearly});

            auto reverseData=base;
            sort(reverseData.begin(),reverseData.end(),[](const rec&a,const rec&b){return a.pAge<b.pAge;});
            reverse(reverseData.begin(),reverseData.end());
            datasets.push_back({"reverse-sorted",reverseData});

            auto duplicates=base;
            for(auto&r:duplicates)
            {
                r.pAge=rng()%5;
            }
            datasets.push_back({"duplicate-heavy",duplicates});

            cout<<"Benchmarking on field: pAge | Pigeonhole bounded key range: 100,000 keys\n";

            for(auto&ds:datasets)
            {
                cout<<"\n--- Dataset: "<<ds.first<<" (N="<<ds.second.size()<<") ---\n";
                for(const auto&name:algs)
                {
                    auto result=runSort(name,ds.second,[](const rec&a,const rec&b){return a.pAge<b.pAge;},[](const rec&r){return r.pAge;});
                    cout<<left<<setw(23)<<name<<" time="<<fixed<<setprecision(3)<<setw(8)<<result.milliseconds<<" ms"
                        <<" comparisons="<<setw(8)<<result.comparisons
                        <<" swaps="<<setw(8)<<result.swaps
                        <<" space="<<setw(26)<<result.space
                        <<" stability="<<result.stable;
                    if(!result.success)
                    {
                        cout<<" [skipped / size > 64 / unsupported]";
                    }
                    cout<<"\n";
                }
            }
        }
        else if(ch==13)
        {
            cout<<"\n=== Stooge Sort Theoretical Analysis & Demonstration ===\n";
            cout<<"Recurrence Relation: T(n) = 3 * T(2n/3) + O(1)\n";
            cout<<"By Master Theorem:   Time Complexity = O(n^(log_1.5 3)) ~= O(n^2.7095)\n";
            cout<<"Auxiliary Space:     O(log n) recursion stack depth\n";
            cout<<"Practical Impact:    Extremely poor scalability. While Tim Sort takes O(n log n)\n";
            cout<<"                     operations, Stooge Sort takes thousands of operations even for small N.\n";
            cout<<"                     Therefore, Stooge Sort is strictly for demonstration (capped at N<=64).\n\n";

            cout<<"Running demonstration for N in {4, 8, 16, 32, 64}:\n";
            for(int sz:{4,8,16,32,64})
            {
                vector<rec> test_arr(sz);
                for(int i=0;i<sz;i++)
                {
                    test_arr[i]=rec{};
                    test_arr[i].pID=sz-i;
                    test_arr[i].pAge=sz-i;
                }

                auto res=runSort("Stooge Sort",test_arr,[](const rec&a,const rec&b){return a.pAge<b.pAge;},[](const rec&r){return r.pAge;});
                cout<<"  N="<<setw(2)<<sz
                    <<" | Comparisons: "<<setw(6)<<res.comparisons
                    <<" | Swaps: "<<setw(6)<<res.swaps
                    <<" | Time: "<<fixed<<setprecision(3)<<setw(7)<<res.milliseconds<<" ms\n";
            }
        }
        else if(ch==14)
        {
            long long id;
            cout<<"Patient ID to search (Hash Map): ";
            if(!(cin>>id))
            {
                clearInput();
                continue;
            }
            Metrics mHash;
            auto a=sys.findByHashMap(id,mHash);
            if(a.empty())
            {
                cout<<"Patient ID "<<id<<" not found in Hash Map (Bucket probe comparisons: "<<mHash.comparisons<<").\n";
            }
            else
            {
                cout<<"Found "<<a.size()<<" record(s) for patient ID "<<id<<" via Hash Map (Bucket probe comparisons: "<<mHash.comparisons<<"):\n";
                for(const auto&r:a)
                {
                    printRecord(r);
                }
            }
        }
        else
        {
            cout<<"Invalid choice. Please select from menu.\n";
        }
    }

    cout<<"Exiting Hospital System. Goodbye!\n";
    return 0;
}
