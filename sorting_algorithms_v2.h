#ifndef SORTING_ALGORITHMS_H
#define SORTING_ALGORITHMS_H

#include<algorithm>
#include<stdexcept>
#include<string>
#include<utility>
#include<vector>
#include"record_struct.h"
using namespace std;

//--------- TREE SORT BEGINS HERE ------
/*
call:
TREESORT(patients,
    [](const rec&x,const rec&y)
    {
        return x.pID<y.pID;
    },
    metrics);
*/
template<class T>
struct TreeSortNode
{
    T value;
    vector<T> equivalent;
    TreeSortNode<T>*l;
    TreeSortNode<T>*r;
    TreeSortNode(const T&v):value(v)
    {
        l=r=nullptr;
    }
};
template<class T,class C>
TreeSortNode<T>*treeSortInsert(TreeSortNode<T>*curr,const T&value,C&less,Metrics&m)
{
    if(curr==nullptr)return new TreeSortNode<T>(value);
    m.comparisons++;
    if(less(value,curr->value))
    {
        curr->l=treeSortInsert(curr->l,value,less,m);
    }
    else
    {
        m.comparisons++;
        if(less(curr->value,value))
        {
            curr->r=treeSortInsert(curr->r,value,less,m);
        }
        else
        {
            curr->equivalent.push_back(value);
        }
    }
    return curr;
}
template<class T>
void treeSortInorder(TreeSortNode<T>*curr,vector<T>&arr,Metrics&m)
{
    if(curr!=nullptr)
    {
        treeSortInorder(curr->l,arr,m);
        arr.push_back(curr->value);
        m.swaps++;
        for(const T&value:curr->equivalent)
        {
            arr.push_back(value);
            m.swaps++;
        }
        treeSortInorder(curr->r,arr,m);
    }
}
template<class T>
void treeSortDestroy(TreeSortNode<T>*curr)
{
    if(curr!=nullptr)
    {
        treeSortDestroy(curr->l);
        treeSortDestroy(curr->r);
        delete curr;
    }
}
template<class T,class C>
void TREESORT(vector<T>&a,C less,Metrics&m)
{
    TreeSortNode<T>*head=nullptr;
    for(const T&value:a)
    {
        head=treeSortInsert(head,value,less,m);
    }
    vector<T>arr;
    arr.reserve(a.size());
    treeSortInorder(head,arr,m);
    treeSortDestroy(head);
    a.swap(arr);
}
//------ TREE SORT ENDS HERE------
template<class T, class C>
void insertionRun(vector<T>& a, int left, int right, C less, Metrics& m)
{
    for(int i=left+1;i<=right;i++)
    {
        T key=a[i];
        int j=i-1;
        while(j>=left)
        {
            m.comparisons++;
            if(!less(key,a[j]))
            break;
            a[j+1]=a[j];
            m.swaps++; // Counts element moves
            j--;
        }
        a[j+1]=key;
        m.swaps++; // Counts placing the key
    }
}

template<class T, class C>
void mergeRuns(vector<T>& a, int left, int mid, int right, C less, Metrics& m)
{
    vector<T>temp;
    temp.reserve(right-left+1);
    
    int i=left;
    int j=mid+1;
    while(i<=mid&&j<=right)
    {
        m.comparisons++;
        // Keep the merge stable when elements are equivalent
        if(less(a[j],a[i])==false)
        temp.push_back(a[i++]);
        else
        temp.push_back(a[j++]);
    }
    while(i<=mid)temp.push_back(a[i++]);
    while(j<=right)temp.push_back(a[j++]);
    for (int k=0;k<(int)temp.size();k++)
    {
        a[left + k] = temp[k];
        m.swaps++; // Counts element moves, not literal swaps
    }
}

/*
Calling it for patient records:
TIMSORT(patients,
    [](const rec& x, const rec& y)
    {
        return x.pID < y.pID;
    },
    metrics);
*/
template<class T, class C>
void TIMSORT(vector<T>& a, C less, Metrics& m)
{
    int n=a.size();
    const int RUN=32;
    
    if(n<=1)
        return;

        // Step 1: Sort small runs using Insertion Sort
    for(int i=0;i<n;i+=RUN)
    {
        insertionRun(a,i,min(i+RUN-1,n-1),less,m);
    }

    // Step 2: Merge adjacent sorted runs
    for(int size=RUN;size<n;size*=2)
    {
        for(int left=0;left<n;left+=2*size)
        {
            int mid=left+size-1;
            int right=min(left+2*size-1,n-1);
            if (mid<n-1)
                mergeRuns(a,left,mid,right,less,m);
        }
    }
}

/*
Call it like this:
PIGEONHOLE(patients,
    [](const rec&x)
    {
        return x.pAge;
    },
    metrics);
*/
template<class T,class K>
void PIGEONHOLE(vector<T>&a,K key,Metrics&m,long long maxRange=100000)
{
    int n=a.size();
    if(n<=1)
    {
        return;
    }
    long long mn=key(a[0]);
    long long mx=key(a[0]);
    for(int i=1;i<n;i++)
    {
        long long k=key(a[i]);
        m.comparisons+=2;
        if(k<mn)mn=k;
        if(k>mx)mx=k;
    }
    if(mx-mn+1>maxRange)
    {
        throw runtime_error("Key range exceeds bounded limit of Pigeonhole Sort.");
    }
    vector<vector<T>>holes(mx-mn+1);
    for(int i=0;i<n;i++)
    {
        holes[key(a[i])-mn].push_back(a[i]);
    }
    int idx=0;
    for(auto&hole:holes)
    {
        for(auto&x:hole)
        {
            a[idx++]=x;
            m.swaps++;
        }
    }
}

/*
Call it like this:
COCKTAIL(patients,
    [](const rec&x,const rec&y)
    {
        return x.pID<y.pID;
    },
    metrics);
*/
template<class T,class C>
void COCKTAIL(vector<T>&a,C less,Metrics&m)
{
    int n=a.size();
    if(n<=1)
    {
        return;
    }
    int start=0;
    int end=n-1;
    bool swapped=true;
    while(swapped)
    {
        swapped=false;
        for(int i=start;i<end;i++)
        {
            m.comparisons++;
            if(less(a[i+1],a[i]))
            {
                swap(a[i],a[i+1]);
                m.swaps++;
                swapped=true;
            }
        }
        if(!swapped)
        {
            break;
        }
        swapped=false;
        end--;
        for(int i=end;i>start;i--)
        {
            m.comparisons++;
            if(less(a[i],a[i-1]))
            {
                swap(a[i],a[i-1]);
                m.swaps++;
                swapped=true;
            }
        }
        start++;
    }
}

/*
COMB(patients,
    [](const rec&x,const rec&y)
    {
        return x.pID<y.pID;
    },
    metrics);
*/
template<class T,class C>
void COMB(vector<T>&a,C less,Metrics&m)
{
    int n=a.size();
    int gap=n;
    bool swapped=true;
    while(gap>1||swapped)
    {
        gap=max(1,(gap*10)/13);
        swapped=false;
        for(int i=0;i+gap<n;i++)
        {
            m.comparisons++;

            if(less(a[i+gap],a[i]))
            {
                swap(a[i],a[i+gap]);
                m.swaps++;
                swapped=true;
            }
        }
    }
}

/*
GNOME(patients,
    [](const rec&x,const rec&y)
    {
        return x.pID<y.pID;
    },
    metrics);
*/
template<class T,class C>
void GNOME(vector<T>&a,C less,Metrics&m)
{
    int n=a.size();
    int i=1;
    while(i<n)
    {
        if(i==0)
        {
            i++;
            continue;
        }
        m.comparisons++;
        if(!less(a[i],a[i-1]))
        {
            i++;
        }
        else
        {
            swap(a[i],a[i-1]);
            m.swaps++;
            i--;
        }
    }
}

/*
STOOGE(patients,
    [](const rec&x,const rec&y)
    {
        return x.pID<y.pID;
    },
    metrics);
*/
template<class T,class C>
void stoogeSort(vector<T>&a,int l,int r,C less,Metrics&m)
{
    if(l>=r)
    {
        return;
    }
    m.comparisons++;
    if(less(a[r],a[l]))
    {
        swap(a[l],a[r]);
        m.swaps++;
    }
    if(r-l+1>2)
    {
        int t=(r-l+1)/3;
        stoogeSort(a,l,r-t,less,m);
        stoogeSort(a,l+t,r,less,m);
        stoogeSort(a,l,r-t,less,m);
    }
}
template<class T,class C>
void STOOGE(vector<T>&a,C less,Metrics&m)
{
    if(a.size()>64)
    {
        throw runtime_error("Stooge Sort is limited to 64 records in this demonstration.");
    }
    if(a.size()<=1)
    {
        return;
    }
    stoogeSort(a,0,a.size()-1,less,m);
}


#endif