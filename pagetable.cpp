#include "pagetable.h"
#include "common.h"
PageTable::PageTable(Memory* Memory_Input, Disk* Disk_Input, int PageTableSize_Input, int PageTableTimeInput, int ReplaceAlgorOption, int ReplaceStratOption) //构造函数
{
    this->Memory_Obj = Memory_Input;
    this->Disk_Obj = Disk_Input;
    this->FIFOPos = 0;
    this->PageTableSize = PageTableSize_Input;
    this->PageTableTime = PageTableTimeInput;
    this->ReplaceAlgorOption = ReplaceAlgorOption;
    this->ReplaceStratOption = ReplaceStratOption;
}

int PageTable::GetPhysicsAddr(int LogicAddr) //响应查页表请求
{
    int PhysicsAddr;
    PhysicsAddr = this->GetPhysicsAddrByTable(LogicAddr);
    this->Result->AccessTime += this->PageTableTime;
    if(PhysicsAddr != -1) //查找页表成功
    {
        this->Result->PageTableHit = true;
        return PhysicsAddr;
    }
    //查找失败，则从外存调入数据
    this->Result->PageTableHit = false;
    PhysicsAddr = this->AccessDisk(LogicAddr);
    return PhysicsAddr;
}

int PageTable::GetPhysicsAddrByTable(int LogicAddr) //查询页表
{
    int PhysicsAddr = -1;
    for(int i=0;(unsigned)i<this->Table.size();i++)
    {
        if(this->Table[i].LogicAddr == LogicAddr) //页表命中
        {
            PhysicsAddr = this->Table[i].PhysicsAddr;
            if(this->ReplaceAlgorOption == 1) //如果是LRU置换算法，则更新访问记录
            {
                AddrMapping TmpAddrMapping = this->Table[i];
                this->Table.erase(this->Table.begin()+i);
                this->Table.push_back(TmpAddrMapping); //将当前记录放到最上
            }
            break;
        }
    }
    return PhysicsAddr;
}

int PageTable::AccessDisk(int LogicAddr) //访问外存
{
    int PhysicsAddr;
    int DiskData;
    int DiskAddr;
    //读取硬盘数据
    DiskAddr = this->Disk_Obj->GetData(LogicAddr);
    DiskData = DiskAddr;
    if(this->Table.size() >= (unsigned int)this->PageTableSize) //如果页表满了则置换
    {
        if(this->ReplaceAlgorOption == 0) //FIFO置换
        {
            PhysicsAddr = this->TableReplaceFIFO(LogicAddr);
        }
        else //LRU置换
        {
            PhysicsAddr = this->TableReplaceLRU(LogicAddr);
        }
        this->Memory_Obj->ReplaceData(PhysicsAddr,DiskData); //替换内存数据


        if((this->ReplaceStratOption==1)&&(!this->Result->PageTableHit)&&(!this->Result->TLBHit))
        {
            this->Result->AccessTime += 20;
        }
    }
    else //没满直接加入
    {
        AddrMapping NewAddrMapping;
        PhysicsAddr = this->Memory_Obj->GetUsedSize();//this->Table.size();
        NewAddrMapping.LogicAddr = LogicAddr;
        NewAddrMapping.PhysicsAddr = PhysicsAddr;
        this->Table.push_back(NewAddrMapping);
        this->Memory_Obj->AddData(PhysicsAddr,DiskData); //添加内存数据
        this->Result->PageTableReplaced.LogicAddr = -1; //-1表示是直接添加到空位置
        this->Result->PageTableReplaced.PhysicsAddr =  PhysicsAddr;
    }
    return PhysicsAddr;

}

void PageTable::SetDisk(Disk *NewDisk) //设置外存指针
{
    this->Disk_Obj = NewDisk;
}

void PageTable::SetMemory(Memory *NewMemory) //设置内存指针
{
    this->Memory_Obj = NewMemory;
}

int PageTable::TableReplaceFIFO(int LogicAddr) //FIFO置换
{
    AddrMapping NewMapping;
    NewMapping.LogicAddr = LogicAddr;
    NewMapping.PhysicsAddr = this->Table[this->FIFOPos].PhysicsAddr;
    this->Result->PageTableReplaced = this->Table[this->FIFOPos]; //记录被替换的条目
    this->Table[this->FIFOPos] = NewMapping; //替换
    this->FIFOPos = (this->FIFOPos+1)%this->Table.size(); //更新当前TLB最早换入的页位置
    return NewMapping.PhysicsAddr; //返回换入的物理地址
}

int PageTable::TableReplaceLRU(int LogicAddr) //LRU置换
{
    AddrMapping NewMapping;
    NewMapping.LogicAddr = LogicAddr;
    NewMapping.PhysicsAddr = this->Table[0].PhysicsAddr;
    this->Result->PageTableReplaced = this->Table[0]; //记录被替换的条目
    this->Table.erase(this->Table.begin()); //换出最近最少使用的条目
    this->Table.push_back(NewMapping); //换入并放到顶端
    return NewMapping.PhysicsAddr;
}

void PageTable::SetPageTableSize(int NewSize) //设置页表大小
{
    this->PageTableSize = NewSize;
}

void PageTable::RefreshResult(AccessResultInfo *Result) //更新结果指针
{
    this->Result = Result;
}
