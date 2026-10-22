#ifndef BENCHMARK_H
#define BENCHMARK_H

using namespace std;

#include <chrono>
#include <map>
#include <string>
#include <utility>
#include <vector>
#include "record_struct.h"
#include "sorting_algorithms_v2.h"

struct SortResult
{
    string algorithm;
    double milliseconds=0;
    long long comparisons=0,swaps=0;
    string space,stable;
    bool success=true;
    long long writes=0;
};
template<class T,class C,class Key>
SortResult runSort(string name,vector<T>&data,C less,Key key)
{
    Metrics m;
    auto start=chrono::steady_clock::now();//
    bool ok=false;
    try
    {
        if(name=="Tim Sort")
        {
            TIMSORT(data,less,m);
            ok=true;
        }
        else if(name=="Pigeonhole Sort")
        {
            PIGEONHOLE(data,key,m);
            ok=true;
        }
        else if(name=="Comb Sort")
        {
            COMB(data,less,m);
            ok=true;
        }
        else if(name=="Cocktail Shaker Sort")
        {
            COCKTAIL(data,less,m);
            ok=true;
        }
        else if(name=="Tree Sort")
        {
            TREESORT(data,less,m);
            ok=true;
        }
        else if(name=="Gnome Sort")
        {
            GNOME(data,less,m);
            ok=true;
        }
        else if(name=="Stooge Sort")
        {
            STOOGE(data,less,m);
            ok=true;
        }
    }
    catch(const exception& e)
    {
        ok=false;
    }
    auto stop=chrono::steady_clock::now();//
    string space="O(1) auxiliary";
    if(name=="Tim Sort")space="O(n) auxiliary";
    else if(name=="Pigeonhole Sort")space="O(n + k) auxiliary";
    else if(name=="Tree Sort")space="O(n) tree storage";
    else if(name=="Stooge Sort")space="O(log n) recursion stack";
    SortResult r
    {
        name,
        chrono::duration<double,milli>(stop-start).count(),
        m.comparisons,
        m.swaps,
        space,
        (name=="Comb Sort"||name=="Stooge Sort")?"Unstable":"Stable",
        ok,
        m.swaps
    };
    return r;
}

#endif // BENCHMARK_H
