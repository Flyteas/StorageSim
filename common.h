#ifndef COMMON_H
#define COMMON_H
struct PCB //PCB结构体
{
    int ProcessID; //进程号
    double AccessBeginTime; //访问到来的时间
    int AccessLogicAddr; //访问逻辑页号
    void *ProcessPageTable; //指向进程所属页表
};

struct AddrMapping //地址映射结构体，即逻辑地址与物理地址的对应
{
    int LogicAddr; //逻辑地址
    int PhysicsAddr; //物理地址
};

struct Trace //外部进程数据
{
    int ProcessID; //进程号
    double AccessTime; //访问时间
    int LogicAddr; //要访问的逻辑地址
};

struct AccessResultInfo //访问结果详细信息
{
    int ProcessID; //进程号
    int LogicAddr; //访问的逻辑地址
    int PhysicsAddr; //返回的物理地址
    int MemoryData; //读取到的内存数据
    bool TLBHit; //TLB是否命中
    bool PageTableHit; //页表是否命中
    bool DiskAccess; //是否访问了外存
    int MemoryDataReplaced; //被置换的内存数据
    AddrMapping TLBReplaced; //被置换的TLB条目
    AddrMapping PageTableReplaced; //被置换的页表条目
    double AccessBeginTime; //访问到来的时间
    int AccessTime; //本次访问所耗时间
    int WaittingTime; //本次访问等待时间
};

#endif // COMMON_H
