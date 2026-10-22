#ifndef AVLSBTREE_H
#define AVLSBTREE_H

#include<algorithm>
#include<iostream>
#include<utility>
#include<vector>
#include"record_struct.h"

using namespace std;

typedef struct Node
{
    long long pID=0;
    rec Record;          // Canonical (first) record for backwards compatibility
    vector<rec> records; // All records / visits for this patient ID
    int height=0;
    Node*l=nullptr,*r=nullptr;

    Node(rec Rec)
    {
        pID=Rec.pID;
        Record=Rec;
        records.push_back(Rec);
        height=0;
        l=r=nullptr;
    }
} node;

class AVLSBTREE
{
private:
    void destroy(node*curr)
    {
        if(curr)
        {
            destroy(curr->l);
            destroy(curr->r);
            delete curr;
        }
    }

    void collectDuplicates(node*curr,vector<pair<long long,int>>&out)const
    {
        if(curr)
        {
            collectDuplicates(curr->l,out);
            if(curr->records.size()>1)
            {
                out.push_back({curr->pID,(int)curr->records.size()});
            }
            collectDuplicates(curr->r,out);
        }
    }

public:
    node*head=nullptr;

    AVLSBTREE():head(nullptr)
    {
    }

    ~AVLSBTREE()
    {
        destroy(head);
        head=nullptr;
    }

    void clear()
    {
        destroy(head);
        head=nullptr;
    }

    int height_c(node*p)const
    {
        return p?p->height:-1;
    }

    node*autobalance(node*curr)
    {
        curr->height=1+max(height_c(curr->l),height_c(curr->r));
        int bal=height_c(curr->l)-height_c(curr->r);
        if(bal>1)
        {
            int intnl_bal=height_c(curr->l->l)-height_c(curr->l->r);
            if(intnl_bal>=0)
            {
                return rightRotate(curr);
            } // LL Imbalance
            else
            {
                curr->l=leftRotate(curr->l);
                return rightRotate(curr);
            } // LR Imbalance
        } // left primary imbalance
        else if(bal<-1)
        {
            int intnl_bal=height_c(curr->r->l)-height_c(curr->r->r);
            if(intnl_bal>0)
            {
                curr->r=rightRotate(curr->r);
                return leftRotate(curr);
            } // RL Imbalance
            else
            {
                return leftRotate(curr);
            } // RR Imbalance
        } // right primary imbalance
        else
        {
            return curr;
        }
    }

    node*rightRotate(node*curr)
    {
        if(!curr||!curr->l)return curr;
        node*x=curr->l;
        node*temp=x->r;
        x->r=curr;
        curr->l=temp;
        curr->height=1+max(height_c(curr->l),height_c(curr->r));
        x->height=1+max(height_c(x->l),height_c(x->r));
        return x;
    }

    node*leftRotate(node*curr)
    {
        if(!curr||!curr->r)return curr;
        node*x=curr->r;
        node*temp=x->l;
        x->l=curr;
        curr->r=temp;
        x->height=1+max(height_c(x->l),height_c(x->r));
        curr->height=1+max(height_c(curr->l),height_c(curr->r));
        return x;
    }

    // Inserts record into AVL tree; duplicate pIDs are added to the existing node's bucket
    node*insert(node*curr,rec toadd,Metrics&m)
    {
        if(curr==nullptr)return new node(toadd);
        m.comparisons++;
        if(toadd.pID<curr->pID)
        {
            curr->l=insert(curr->l,toadd,m);
        }
        else if(toadd.pID>curr->pID)
        {
            m.comparisons++;
            curr->r=insert(curr->r,toadd,m);
        }
        else
        {
            // Duplicate pID: retain record in the bucket list
            curr->records.push_back(toadd);
            return curr;
        }
        return autobalance(curr);
    }

    // Convenience 2-argument insert overload
    node*insert(node*curr,rec toadd)
    {
        Metrics dummy;
        return insert(curr,toadd,dummy);
    }

    node*search(node*curr,long long ID)const
    {
        while(curr)
        {
            if(ID<curr->pID)curr=curr->l;
            else if(ID>curr->pID)curr=curr->r;
            else return curr;
        }
        return nullptr;
    }

    // Convenience search method
    node*find(long long ID)const
    {
        return search(head,ID);
    }

    bool del(long long ID)
    {
        bool deleted=false;
        head=erase(head,ID,deleted);
        return deleted;
    }

    node*erase(node*curr,long long ID,bool&deleted)
    {
        if(curr==nullptr)return nullptr;
        if(ID<curr->pID)curr->l=erase(curr->l,ID,deleted);
        else if(ID>curr->pID)curr->r=erase(curr->r,ID,deleted);
        else
        {
            deleted=true;
            if(curr->l==nullptr)
            {
                node*replacement=curr->r;
                delete curr;
                return replacement;
            }
            if(!curr->r)
            {
                node*replacement=curr->l;
                delete curr;
                return replacement;
            }
            node*succ=curr->r;
            while(succ->l)succ=succ->l;
            curr->pID=succ->pID;
            curr->Record=succ->Record;
            curr->records=succ->records;
            bool succdel=false;
            curr->r=erase(curr->r,succ->pID,succdel);
        }
        return autobalance(curr);
    }

    void inorder(node*curr,vector<rec>&arr,Metrics&m)const
    {
        if(curr)
        {
            inorder(curr->l,arr,m);
            for(const auto&r:curr->records)
            {
                arr.push_back(r);
                m.swaps++;
            }
            inorder(curr->r,arr,m);
        }
    }

    vector<rec>treesort(Metrics&m)const
    {
        vector<rec>arr;
        inorder(head,arr,m);
        return arr;
    }

    vector<rec>treesort()const
    {
        Metrics m;
        return treesort(m);
    }

    // Retrieves all duplicate patient IDs with their occurrence count (Task vi)
    vector<pair<long long,int>>getDuplicates()const
    {
        vector<pair<long long,int>>out;
        collectDuplicates(head,out);
        return out;
    }
};

#endif // AVLSBTREE_H
