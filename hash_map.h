#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>
#include "record_struct.h"

using namespace std;

// Node for separate-chaining hash map
struct HashNode
{
    long long pID = 0;
    rec Record;          // Canonical (first) record for backwards compatibility
    vector<rec> records; // All records / visits for this patient ID
    HashNode* next = nullptr;

    HashNode(rec Rec)
    {
        pID = Rec.pID;
        Record = Rec;
        records.push_back(Rec);
        next = nullptr;
    }
};

// ==================== PATIENT HASH MAP ====================
// Hash Table with separate chaining for O(1) average-case patient lookup,
// duplicate patient ID tracking, and deletion.
class PatientHashMap
{
private:
    vector<HashNode*> table;
    size_t numBuckets;
    size_t numKeys;
    float maxLoadFactor;

    // 64-bit integer mixing hash function (SplitMix64 finalizer)
    // Ensures uniform distribution across buckets even with sequential or patterned IDs
    size_t hashFunction(long long key) const
    {
        unsigned long long x = (unsigned long long)key;
        x ^= x >> 30;
        x *= 0xbf58476d1ce4e5b9ULL;
        x ^= x >> 27;
        x *= 0x94d049bb133111ebULL;
        x ^= x >> 31;
        return (size_t)(x % numBuckets);
    }

    void rehash(size_t newBucketCount)
    {
        vector<HashNode*> newTable(newBucketCount, nullptr);
        size_t oldBuckets = numBuckets;
        numBuckets = newBucketCount;

        for (size_t i = 0; i < oldBuckets; ++i)
        {
            HashNode* curr = table[i];
            while (curr != nullptr)
            {
                HashNode* nextNode = curr->next;
                size_t newIdx = hashFunction(curr->pID);
                curr->next = newTable[newIdx];
                newTable[newIdx] = curr;
                curr = nextNode;
            }
        }
        table = move(newTable);
    }

    void destroy()
    {
        for (size_t i = 0; i < numBuckets; ++i)
        {
            HashNode* curr = table[i];
            while (curr != nullptr)
            {
                HashNode* tmp = curr;
                curr = curr->next;
                delete tmp;
            }
            table[i] = nullptr;
        }
        numKeys = 0;
    }

public:
    explicit PatientHashMap(size_t initialBuckets = 101, float loadFactor = 0.75f)
        : table(initialBuckets, nullptr), numBuckets(initialBuckets), numKeys(0), maxLoadFactor(loadFactor)
    {
    }

    ~PatientHashMap()
    {
        destroy();
    }

    PatientHashMap(const PatientHashMap& other)
        : table(other.numBuckets, nullptr), numBuckets(other.numBuckets), numKeys(0), maxLoadFactor(other.maxLoadFactor)
    {
        for (size_t i = 0; i < other.numBuckets; ++i)
        {
            HashNode* curr = other.table[i];
            while (curr != nullptr)
            {
                for (const auto& r : curr->records)
                {
                    insert(r);
                }
                curr = curr->next;
            }
        }
    }

    PatientHashMap& operator=(const PatientHashMap& other)
    {
        if (this != &other)
        {
            destroy();
            numBuckets = other.numBuckets;
            maxLoadFactor = other.maxLoadFactor;
            table.assign(numBuckets, nullptr);
            numKeys = 0;
            for (size_t i = 0; i < other.numBuckets; ++i)
            {
                HashNode* curr = other.table[i];
                while (curr != nullptr)
                {
                    for (const auto& r : curr->records)
                    {
                        insert(r);
                    }
                    curr = curr->next;
                }
            }
        }
        return *this;
    }

    PatientHashMap(PatientHashMap&& other) noexcept
        : table(move(other.table)), numBuckets(other.numBuckets), numKeys(other.numKeys), maxLoadFactor(other.maxLoadFactor)
    {
        other.numBuckets = 0;
        other.numKeys = 0;
    }

    PatientHashMap& operator=(PatientHashMap&& other) noexcept
    {
        if (this != &other)
        {
            destroy();
            table = move(other.table);
            numBuckets = other.numBuckets;
            numKeys = other.numKeys;
            maxLoadFactor = other.maxLoadFactor;
            other.numBuckets = 0;
            other.numKeys = 0;
        }
        return *this;
    }

    void clear()
    {
        destroy();
    }

    // Inserts record into hash map; duplicate pIDs are added to the existing node's bucket
    void insert(rec toadd, Metrics& m)
    {
        if ((float)(numKeys + 1) / numBuckets > maxLoadFactor)
        {
            rehash(numBuckets * 2 + 1);
        }

        size_t idx = hashFunction(toadd.pID);
        HashNode* curr = table[idx];

        while (curr != nullptr)
        {
            m.comparisons++;
            if (curr->pID == toadd.pID)
            {
                // Duplicate pID: retain record in the bucket list
                curr->records.push_back(toadd);
                return;
            }
            curr = curr->next;
        }

        // New patient ID: insert at head of bucket chain (O(1))
        HashNode* newNode = new HashNode(toadd);
        newNode->next = table[idx];
        table[idx] = newNode;
        numKeys++;
        m.swaps++; // Node write/placement
    }

    void insert(rec toadd)
    {
        Metrics dummy;
        insert(toadd, dummy);
    }

    // Searches for patient ID and returns pointer to HashNode (or nullptr if not found)
    HashNode* search(long long ID, Metrics& m) const
    {
        size_t idx = hashFunction(ID);
        HashNode* curr = table[idx];
        while (curr != nullptr)
        {
            m.comparisons++;
            if (curr->pID == ID)
            {
                return curr;
            }
            curr = curr->next;
        }
        return nullptr;
    }

    HashNode* search(long long ID) const
    {
        Metrics dummy;
        return search(ID, dummy);
    }

    // Convenience method matching AVL Tree interface
    HashNode* find(long long ID) const
    {
        return search(ID);
    }

    // Retrieves all records for a patient ID
    vector<rec> getRecords(long long ID) const
    {
        HashNode* node = search(ID);
        if (node != nullptr)
        {
            return node->records;
        }
        return {};
    }

    vector<rec> getRecords(long long ID, Metrics& m) const
    {
        HashNode* node = search(ID, m);
        if (node != nullptr)
        {
            return node->records;
        }
        return {};
    }

    // Deletes patient by ID, removing all associated visits
    bool del(long long ID)
    {
        size_t idx = hashFunction(ID);
        HashNode* curr = table[idx];
        HashNode* prev = nullptr;

        while (curr != nullptr)
        {
            if (curr->pID == ID)
            {
                if (prev == nullptr)
                {
                    table[idx] = curr->next;
                }
                else
                {
                    prev->next = curr->next;
                }
                delete curr;
                numKeys--;
                return true;
            }
            prev = curr;
            curr = curr->next;
        }
        return false;
    }

    bool erase(long long ID)
    {
        return del(ID);
    }

    bool contains(long long ID) const
    {
        return search(ID) != nullptr;
    }

    // Retrieves all duplicate patient IDs with their occurrence count (Task vi)
    vector<pair<long long, int>> getDuplicates() const
    {
        vector<pair<long long, int>> dups;
        for (size_t i = 0; i < numBuckets; ++i)
        {
            HashNode* curr = table[i];
            while (curr != nullptr)
            {
                if (curr->records.size() > 1)
                {
                    dups.push_back({curr->pID, (int)curr->records.size()});
                }
                curr = curr->next;
            }
        }
        return dups;
    }

    // Retrieves all records across all buckets
    vector<rec> allRecords() const
    {
        vector<rec> out;
        for (size_t i = 0; i < numBuckets; ++i)
        {
            HashNode* curr = table[i];
            while (curr != nullptr)
            {
                for (const auto& r : curr->records)
                {
                    out.push_back(r);
                }
                curr = curr->next;
            }
        }
        return out;
    }

    size_t size() const
    {
        return numKeys;
    }

    size_t totalRecords() const
    {
        size_t cnt = 0;
        for (size_t i = 0; i < numBuckets; ++i)
        {
            HashNode* curr = table[i];
            while (curr != nullptr)
            {
                cnt += curr->records.size();
                curr = curr->next;
            }
        }
        return cnt;
    }

    bool empty() const
    {
        return numKeys == 0;
    }

    size_t bucketCount() const
    {
        return numBuckets;
    }

    float loadFactor() const
    {
        return (float)numKeys / numBuckets;
    }

    size_t maxChainLength() const
    {
        size_t maxLen = 0;
        for (size_t i = 0; i < numBuckets; ++i)
        {
            size_t len = 0;
            HashNode* curr = table[i];
            while (curr != nullptr)
            {
                len++;
                curr = curr->next;
            }
            if (len > maxLen) maxLen = len;
        }
        return maxLen;
    }
};

// Typedef alias
using HashMap = PatientHashMap;

#endif // HASH_MAP_H
