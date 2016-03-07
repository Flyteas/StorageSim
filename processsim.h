#ifndef PROCESSSIM_H
#define PROCESSSIM_H
#include "tlb.h"
#include "memory.h"
#include "common.h"
/* 进程类 */
class ProcessSim
{
public:
    ProcessSim(TLB *TLB_Input,Memory *Memory_Input); //构造函数
    void SetProcessPCB(PCB NewPCB); //设置PCB信息
    void SetTLB(TLB *NewTLB); //设置TLB指针
    void SetMemory(Memory *NewMemory); //设置内存指针
    void RefreshResult(AccessResultInfo *Result); //更新结果指针
    int AccessMemory();//访问内存
private:
    Memory *Memory_Obj; //内存指针
    TLB *TLB_Obj; //TLB指针
    PCB ProcessPCB; //进程PCB信息
    AccessResultInfo *Result; //访问结果
};

#endif // PROCESS_H
