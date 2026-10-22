#include<cassert>
#include<iostream>
#include"record_struct.h"
#include"pq.h"
#include"sbbst_v2.h"
#include"hash_map.h"

int main()
{
    rec a{};a.pID=10;a.priority=2;
    rec b{};b.pID=20;b.priority=5;
    rec c{};c.pID=30;c.priority=5;
    MaxHeap maxh;maxh.insert(a.priority,a);maxh.insert(b.priority,b);maxh.insert(c.priority,c);
    assert(maxh.top().second.pID==20); // FIFO for equal priority
    assert(maxh.pop().second.pID==20);
    MinHeap minh;minh.insert(a.priority,a);minh.insert(b.priority,b);
    assert(minh.top().second.pID==10);
    AVLSBTREE tree;tree.head=tree.insert(tree.head,a);tree.head=tree.insert(tree.head,b);tree.head=tree.insert(tree.head,c);
    assert(tree.find(20)!=nullptr);assert(tree.find(99)==nullptr);
    auto sorted=tree.treesort();assert(sorted.size()==3);assert(sorted[0].pID==10&&sorted[2].pID==30);
    assert(tree.del(20));assert(tree.find(20)==nullptr);

    // Core Hash Map tests
    PatientHashMap hmap;hmap.insert(a);hmap.insert(b);hmap.insert(c);
    assert(hmap.find(20)!=nullptr);assert(hmap.find(99)==nullptr);
    assert(hmap.getRecords(20).size()==1);
    assert(hmap.del(20));assert(hmap.find(20)==nullptr);
    assert(!hmap.del(20));

    std::cout<<"Core data-structure tests passed.\n";
    return 0;
}
