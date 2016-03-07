#include "processsim.h"
ProcessSim::ProcessSim(TLB *TLB_Input, Memory *Memory_Input) //构造函数
{
    this->ProcessPCB.ProcessPageTable = NULL;
    this->ProcessPCB.AccessBeginTime = 0;
    this->ProcessPCB.AccessLogicAddr = 0;
    this->TLB_Obj = TLB_Input;
    this->Memory_Obj = Memory_Input;
}

void ProcessSim::SetProcessPCB(PCB NewPCB) //设置新的PCB信息
{
    this->ProcessPCB = NewPCB;
}

void ProcessSim::SetTLB(TLB *NewTLB) //设置新的TLB信息
{
    this->TLB_Obj = NewTLB;
}

void ProcessSim::SetMemory(Memory *NewMemory) //设置新的内存信息
{
    this->Memory_Obj = NewMemory;
}

int ProcessSim::AccessMemory() //访问内存
{
    int PhysicsAddr;
    PhysicsAddr = this->TLB_Obj->GetPhysicsAddr(this->ProcessPCB.AccessLogicAddr);
    this->Result->ProcessID = this->ProcessPCB.ProcessID;
    this->Result->AccessBeginTime = this->ProcessPCB.AccessBeginTime; //访问到来的时间
    this->Result->LogicAddr = this->ProcessPCB.AccessLogicAddr;
    this->Result->PhysicsAddr = PhysicsAddr;
    this->Result->MemoryData = this->Memory_Obj->GetData(PhysicsAddr);
    return this->Result->MemoryData;
}

void ProcessSim::RefreshResult(AccessResultInfo *Result) //更新结果指针
{
    this->Result = Result;
}
