#ifndef STORAGESIM_H
#define STORAGESIM_H
#include "processsim.h"
#include "tlb.h"
#include "pagetable.h"
#include "memory.h"
#include "disk.h"
#include "common.h"
/* 存储管理模拟类 */
class StorageSim
{
public:
    StorageSim(int MemorySizeInput,int TLBSizeInput,int TLBTimeInput,int MemoryTimeInput,int DiskSeekTimeInput,int DiskRotaTimeInput,int DiskTransTimeInput,int ReplaceAlgorOptionInput,int ReplaceStratOptionInput,QString DiskMappingFilePathInput);//构造函数
    void Access(int ProcessID, int AccessBeginTime, int LogicAddr, AccessResultInfo *Result); //处理进程访问请求
private:
    Disk *Disk_Obj; //外存指针
    Memory *Memory_Obj; //内存指针
    PageTable *PageTable_Obj; //页表指针
    TLB *TLB_Obj; //TLB指针
    ProcessSim *ProcessSim_Obj; //进程指针

    int MemorySize; //内存可容纳的页框数
    int TLBSize; //TLB大小
    int TLBTime; //TLB访问时间
    int MemoryTime; //内存访问时间
    int DiskSeekTime; //外存寻道时间
    int DiskRotaTime; //外存旋转时间
    int DiskTransTime; //外存传输数据时间
    int ReplaceAlgorOption; //置换算法，0表示FIFO，1表示LRU
    int ReplaceStratOption; //置换策略，0表示全局置换，表示局部置换
    QString DiskMappingFilePath; //硬盘映射数据文件

};

#endif // STORAGESIM_H
