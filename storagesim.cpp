#include "storagesim.h"
StorageSim::StorageSim(int MemorySizeInput, int TLBSizeInput, int TLBTimeInput, int MemoryTimeInput, int DiskSeekTimeInput, int DiskRotaTimeInput, int DiskTransTimeInput, int ReplaceAlgorOptionInput, int ReplaceStratOptionInput, QString DiskMappingFilePathInput) //构造函数
{
    this->MemorySize = MemorySizeInput;
    this->TLBSize = TLBSizeInput;
    this->TLBTime = TLBTimeInput;
    this->MemoryTime = MemoryTimeInput;
    this->DiskSeekTime = DiskSeekTimeInput;
    this->DiskRotaTime = DiskRotaTimeInput;
    this->DiskTransTime = DiskTransTimeInput;
    this->ReplaceAlgorOption = ReplaceAlgorOptionInput;
    this->ReplaceStratOption = ReplaceStratOptionInput;
    this->DiskMappingFilePath = DiskMappingFilePathInput;

    Disk_Obj = new Disk(this->DiskSeekTime,this->DiskRotaTime,this->DiskTransTime);
    if(!Disk_Obj->LoadDataFile(this->DiskMappingFilePath))
    {
        return;
    }
    Memory_Obj = new Memory(this->MemoryTime);
    PageTable_Obj = new PageTable(Memory_Obj,Disk_Obj,this->MemorySize,this->MemoryTime,this->ReplaceAlgorOption,this->ReplaceStratOption);
    TLB_Obj = new TLB(PageTable_Obj,this->TLBSize,this->TLBTime,this->ReplaceAlgorOption,this->ReplaceStratOption);
    ProcessSim_Obj = new ProcessSim(TLB_Obj,Memory_Obj);
}

void StorageSim::Access(int ProcessID,int AccessBeginTime, int LogicAddr, AccessResultInfo *Result) //处理进程访问请求
{
    PCB PCB_Obj;
    PCB_Obj.ProcessID = ProcessID;
    PCB_Obj.AccessBeginTime = AccessBeginTime;
    PCB_Obj.AccessLogicAddr = LogicAddr;
    ProcessSim_Obj->SetProcessPCB(PCB_Obj);
    this->ProcessSim_Obj->RefreshResult(Result); //更新结果指针
    this->TLB_Obj->RefreshResult(Result); //更新结果指针
    this->PageTable_Obj->RefreshResult(Result); //更新结果指针
    this->Memory_Obj->RefreshResult(Result); //更新结果指针
    this->Disk_Obj->RefreshResult(Result); //更新结果指针
    ProcessSim_Obj->AccessMemory();
}
