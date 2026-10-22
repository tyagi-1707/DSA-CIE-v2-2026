#ifndef RECORD_STRUCT_H
#define RECORD_STRUCT_H

#include<string>

struct Metrics
{
    long long comparisons=0,swaps=0;
};

struct rec
{
    long long pID=0;
    int pAge=0;
    std::string time,diagnosis,department;
    int diagnosticValue=0,priority=0;
    int c=0;
};

#endif // RECORD_STRUCT_H
