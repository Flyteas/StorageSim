#ifndef PAGETABLE_H
#define PAGETABLE_H
#include <vector>
#include "memory.h"
#include "disk.h"
#include "common.h"
/* 页表类 */
using namespace std;
class PageTable
{
public:
    PageTable(Memory* Memory_Input,Disk* Disk_Input,int PageTableSize_Input,int PageTableTimeInput,int ReplaceAlgorOption,int ReplaceStratOption); //构造函数
    int GetPhysicsAddr(int LogicAddr); //通过逻辑地址获取物理地址，响应TLB的请求，也就是供TLB调用
    void SetPageTableSize(int NewSize); //设置页表大小
    void SetMemory(Memory* NewMemory); //设置内存指针
    void SetDisk(Disk* NewDisk); //设置外存指针
    void RefreshResult(AccessResultInfo *Result); //更新结果指针
private:
    int ReplaceAlgorOption; //置换算法，0表示FIFO，1表示LRU
    int ReplaceStratOption; //置换策略，0表示全局置换，表示局部置换
    int PageTableTime; //访问页表所需时间
    vector<AddrMapping> Table; //页表数据
    int GetPhysicsAddrByTable(int LogicAddr); //查找页表,查找失败返回-1
    int AccessDisk(int LogicAddr); //将外存数据调入内存，返回调入的内存的数据在内存中的物理地址
    int TableReplaceFIFO(int LogicAddr); //使用FIFO替换TLB
    int TableReplaceLRU(int LogicAddr); //使用LRU替换TLB
    int PageTableSize; //页表大小
    Memory* Memory_Obj; //指向内存
    Disk* Disk_Obj; //指向硬盘
    int FIFOPos; //FIFO最早进入的位置
    AccessResultInfo *Result; //访问结果
};
#endif // PAGETABLE_H
