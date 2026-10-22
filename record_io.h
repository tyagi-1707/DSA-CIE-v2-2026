#ifndef RECORD_IO_H
#define RECORD_IO_H

#include <cctype>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>

#include "record_struct.h"

using namespace std;
string lowerStr(string s)
{
    for(char&c:s)
    {
        c=tolower((unsigned char)c);
    }
    return s;
}

rec inputRecord()
{
    rec r{};
    cout<<"Patient ID: ";
    cin>>r.pID;
    cout<<"Age: ";
    cin>>r.pAge;
    cout<<"Appointment time (YYYY-MM-DD HH:MM): ";
    cin.ignore(numeric_limits<streamsize>::max(),'\n');
    getline(cin,r.time);
    cout<<"Diagnosis: ";
    getline(cin,r.diagnosis);
    cout<<"Department: ";
    getline(cin,r.department);
    cout<<"Diagnostic value (integer): ";
    cin>>r.diagnosticValue;
    cout<<"Triage priority (0-5; 5 most urgent): ";
    cin>>r.priority;
    return r;
}

void printRecord(const rec&r)
{
    cout<<"ID="<<r.pID<<" | age="<<r.pAge<<" | appointment="<<r.time<<" | dept="<<r.department<<" | diagnosis="<<r.diagnosis<<" | diagnosticValue="<<r.diagnosticValue<<" | triage="<<r.priority<<"\n";
}

#endif // RECORD_IO_H
