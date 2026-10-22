#ifndef PQ_H
#define PQ_H

#include<bits/stdc++.h>
#include"record_struct.h"
//#include"sbbst_v2.h"
using namespace std;

// ==================== MAX HEAP ====================
// Priority Queue for urgent cases (higher value = higher priority)

class MaxHeap
{
private:
    vector<pair<int,rec>>heap; // {priority, patient_record}
    int c=0;                   // Counter for arrival order (FIFO tie-breaking)

public:
    MaxHeap() {}

    // Read-only accessors for UI inspection and queue monitoring
    const vector<pair<int, rec>>& get_heap() const { return heap; }
    size_t size() const { return heap.size(); }
    bool empty() const { return heap.empty(); }
    void clear() { heap.clear(); c = 0; }

    // Inserts a new patient record into the heap
    void insert(int x,rec r)
    {
        r.c=++c;
        heap.push_back({x,r});
        HeapifyUp(heap.size()-1);
    }

    // Restores max-heap property upwards; breaks equal priority ties via FIFO (smaller c)
    void HeapifyUp(int i)
    {
        while (i>0)
        {
            int par_ind=(i-1)/2;

            if(heap[i].first<heap[par_ind].first||(heap[i].first==heap[par_ind].first&&heap[i].second.c > heap[par_ind].second.c))
            {
                return;
            }

            swap(heap[i],heap[par_ind]);
            i=par_ind;
        }
    }

    // Deletes record at index i, restores heap property, and returns the deleted element
    pair<int, rec> delete_index(int i)
    {
        if (i<0||i>=(int)heap.size())
            return {-1,{}};

        int last=(int)heap.size()-1;
        auto temp=heap[i];

        heap[i]=heap[last];
        heap.pop_back();

        if(i==last)
            return temp;

        if(i>0&&(heap[i].first>heap[(i-1)/2].first||(heap[i].first==heap[(i-1)/2].first&&heap[i].second.c<heap[(i-1)/2].second.c)))
        {
            HeapifyUp(i);
        }
        else
        {
            HeapifyDown(i);
        }
        return temp;
    }

    // Removes and returns the highest priority patient record
    pair<int, rec> pop()
    {
        return delete_index(0);
    }

    // Restores max-heap property downwards; favors higher priority or earlier arrival
    void HeapifyDown(int i)
    {
        while (true)
        {
            int left=2*i+1;
            int right=2*i+2;
            int largest=i;

            if(left<(int)heap.size()&&(heap[left].first>heap[largest].first||(heap[left].first==heap[largest].first&&heap[left].second.c < heap[largest].second.c)))
            {
                largest = left;
            }

            if(right<(int)heap.size()&&(heap[right].first>heap[largest].first||(heap[right].first==heap[largest].first&&heap[right].second.c<heap[largest].second.c)))
            {
                largest=right;
            }

            if (largest==i)
                return;

            swap(heap[i],heap[largest]);
            i=largest;
        }
    }

    // Deletes patient by unique patient ID (pID)
    pair<int, rec> delete_by_id(long long ID)
    {
        for (size_t i=0;i<heap.size();i++)
        {
            if (heap[i].second.pID==ID)
            {
                return delete_index((int)i);
            }
        }
        return {-1,{}};
    }

    // Deletes the first patient encountered with priority x
    pair<int, rec> delete_by_priority(int x)
    {
        for (size_t i=0;i<heap.size();i++)
        {
            if (heap[i].first==x)
            {
                return delete_index((int)i);
            }
        }
        return {-1,{}};
    }

    // Alias for delete_by_priority
    pair<int, rec> delete_element(int x)
    {
        return delete_by_priority(x);
    }

    // Returns the highest priority record without removing it
    pair<int, rec> top()
    {
        if (!heap.empty())
            return heap[0];

        return {-1,{}};
    }
};


// ==================== MIN HEAP ====================
// Priority Queue (lower value = higher priority)

class MinHeap
{
private:
    vector<pair<int, rec>> heap; // {priority, patient_record}
    int c = 0;                   // Counter for arrival order (FIFO tie-breaking)

public:
    MinHeap() {}

    // Read-only accessors for UI inspection and queue monitoring
    const vector<pair<int, rec>>& get_heap() const { return heap; }
    size_t size() const { return heap.size(); }
    bool empty() const { return heap.empty(); }
    void clear() { heap.clear(); c = 0; }

    // Inserts a new patient record into the heap
    void insert(int x,rec r)
    {
        r.c=++c;
        heap.push_back({x,r});
        HeapifyUp(heap.size()-1);
    }

    // Restores min-heap property upwards; breaks equal priority ties via FIFO (smaller c)
    void HeapifyUp(int i)
    {
        while (i>0)
        {
            int par_ind=(i-1)/2;

            if(heap[i].first>heap[par_ind].first||(heap[i].first==heap[par_ind].first&&heap[i].second.c>heap[par_ind].second.c))
            {
                return;
            }

            swap(heap[i],heap[par_ind]);
            i=par_ind;
        }
    }

    // Deletes record at index i, restores heap property, and returns the deleted element
    pair<int, rec> delete_index(int i)
    {
        if (i<0||i>=(int)heap.size())
            return {-1,{}};

        int last=(int)heap.size()-1;
        auto temp=heap[i];

        heap[i]=heap[last];
        heap.pop_back();

        if (i==last)
            return temp;

        if (i>0&&(heap[i].first<heap[(i-1)/2].first||(heap[i].first==heap[(i-1)/2].first&&heap[i].second.c<heap[(i - 1)/2].second.c)))
        {
            HeapifyUp(i);
        }
        else
        {
            HeapifyDown(i);
        }
        return temp;
    }

    // Removes and returns the minimum priority patient record
    pair<int, rec> pop()
    {
        return delete_index(0);
    }

    // Restores min-heap property downwards; favors lower priority or earlier arrival
    void HeapifyDown(int i)
    {
        while (true)
        {
            int left=2*i+1;
            int right=2*i+2;
            int smallest=i;

            if(left<(int)heap.size()&&(heap[left].first<heap[smallest].first||(heap[left].first==heap[smallest].first&&heap[left].second.c<heap[smallest].second.c)))
            {
                smallest=left;
            }

            if(right<(int)heap.size()&&(heap[right].first<heap[smallest].first||(heap[right].first==heap[smallest].first&&heap[right].second.c < heap[smallest].second.c)))
            {
                smallest=right;
            }

            if (smallest==i)
                return;

            swap(heap[i],heap[smallest]);
            i=smallest;
        }
    }

    // Deletes patient by unique patient ID (pID)
    pair<int, rec> delete_by_id(long long ID)
    {
        for (size_t i=0;i<heap.size();i++)
        {
            if(heap[i].second.pID==ID)
            {
                return delete_index((int)i);
            }
        }
        return {-1,{}};
    }

    // Deletes the first patient encountered with priority x
    pair<int, rec> delete_by_priority(int x)
    {
        for (size_t i=0;i<heap.size();i++)
        {
            if(heap[i].first==x)
            {
                return delete_index((int)i);
            }
        }
        return {-1,{}};
    }

    // Alias for delete_by_priority
    pair<int, rec> delete_element(int x)
    {
        return delete_by_priority(x);
    }

    // Returns the minimum priority record without removing it
    pair<int, rec> top()
    {
        if (!heap.empty())
            return heap[0];
        return {-1,{}};
    }
};

#endif
