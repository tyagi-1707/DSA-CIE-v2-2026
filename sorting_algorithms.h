#ifndef SORTING_ALGORITHMS_H
#define SORTING_ALGORITHMS_H

using namespace std;

#include <algorithm>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

// Instrumentation used by the sorting and benchmarking implementations.
struct Metrics { long long comparisons=0, writes=0; };
template<class T, class C> void insertionRun(vector<T>& a,int lo,int hi,C less,Metrics& m){
    for(int i=lo+1;i<hi;i++){ T x=a[i]; int j=i; while(j>lo){m.comparisons++; if(!less(x,a[j-1])) break; a[j]=a[j-1];m.writes++;--j;} a[j]=x;m.writes++; }
}
template<class T,class C> void mergeRuns(vector<T>&a,int l,int mid,int r,C less,Metrics&m){
    vector<T> tmp;tmp.reserve(r-l);int i=l,j=mid;
    while(i<mid&&j<r){m.comparisons++; if(!less(a[j],a[i]))tmp.push_back(a[i++]);else tmp.push_back(a[j++]);}
    while(i<mid)tmp.push_back(a[i++]);while(j<r)tmp.push_back(a[j++]);
    for(int k=l;k<r;k++){a[k]=move(tmp[k-l]);m.writes++;}
}
template<class T,class C> void timSort(vector<T>&a,C less,Metrics&m){
    const int RUN=32; int n=a.size();
    for(int i=0;i<n;i+=RUN) insertionRun(a,i,min(i+RUN,n),less,m);
    for(int sz=RUN;sz<n;sz*=2) for(int l=0;l<n;l+=2*sz){int mid=min(l+sz,n),r=min(l+2*sz,n);if(mid<r)mergeRuns(a,l,mid,r,less,m);}
}
template<class T,class Key> bool pigeonholeSort(vector<T>&a,Key key,Metrics&m,long long maxRange=100000){
    if(a.empty())return true;
    long long mn=key(a[0]),mx=mn;
    for(auto&x:a){m.comparisons+=2;mn=min(mn,(long long)key(x));mx=max(mx,(long long)key(x));}
    if(mx-mn+1>maxRange)return false;
    vector<vector<T>> holes((size_t)(mx-mn+1));
    for(auto&x:a)holes[(long long)key(x)-mn].push_back(x);
    size_t k=0;for(auto&h:holes)for(auto&x:h){a[k++]=move(x);m.writes++;}return true;
}
template<class T,class C> void combSort(vector<T>&a,C less,Metrics&m){
    int n=a.size(),gap=n;bool swapped=true;
    while(gap>1||swapped){gap=max(1,(gap*10)/13);swapped=false;for(int i=0;i+gap<n;i++){m.comparisons++;if(less(a[i+gap],a[i])){swap(a[i],a[i+gap]);m.writes+=2;swapped=true;}}}
}
template<class T,class C> void cocktailSort(vector<T>&a,C less,Metrics&m){
    if(a.size()<2)return;size_t start=0,end=a.size()-1;bool swapped=true;
    while(swapped){swapped=false;for(size_t i=start;i<end;i++){m.comparisons++;if(less(a[i+1],a[i])){swap(a[i],a[i+1]);m.writes+=2;swapped=true;}}
        if(!swapped)break;swapped=false;if(end==0)break;--end;
        for(size_t i=end;i>start;i--){m.comparisons++;if(less(a[i],a[i-1])){swap(a[i],a[i-1]);m.writes+=2;swapped=true;}}++start;}
}
template<class T,class C> void gnomeSort(vector<T>&a,C less,Metrics&m){
    size_t i=1;while(i<a.size()){if(i==0){i++;continue;}m.comparisons++;if(!less(a[i],a[i-1]))i++;else{swap(a[i],a[i-1]);m.writes+=2;i--;}}
}
template<class T,class C> void stoogeRec(vector<T>&a,int l,int r,C less,Metrics&m){
    if(l>=r)return;m.comparisons++;if(less(a[r],a[l])){swap(a[l],a[r]);m.writes+=2;}
    if(r-l+1>2){int t=(r-l+1)/3;stoogeRec(a,l,r-t,less,m);stoogeRec(a,l+t,r,less,m);stoogeRec(a,l,r-t,less,m);}
}
template<class T,class C> void stoogeSort(vector<T>&a,C less,Metrics&m){if(a.size()>64)throw runtime_error("Stooge Sort is limited to 64 records in this demonstration.");if(!a.empty())stoogeRec(a,0,(int)a.size()-1,less,m);}


#endif // SORTING_ALGORITHMS_H
