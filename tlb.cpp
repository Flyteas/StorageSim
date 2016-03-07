#include "tlb.h"

TLB::TLB(PageTable *PageTable_Input, int TLBSize_Input, int TLBTime_Input, int ReplaceAlgorOption, int ReplaceStratOption) //构造函数
{
    this->TLBSize = TLBSize_Input;
    this->PageTable_Obj = PageTable_Input;
    this->TLBTime = TLBTime_Input;
    this->FIFOPos = 0;
    this->ReplaceAlgorOption = ReplaceAlgorOption;
    this->ReplaceStratOption = ReplaceStratOption;
}

int TLB::GetPhysicsAddr(int LogicAddr) //响应进程的访问请求
{
    int PhysicsAddr;
    PhysicsAddr = this->GetPhysicsAddrByTLB(LogicAddr);
    this->Result->AccessTime += this->TLBTime;
    if(PhysicsAddr != -1) //查找TLB表成功
    {
        this->Result->TLBHit = true;
        return PhysicsAddr;
    }
   this->Result->TLBHit = false;
    //查找失败，则查找页表并替换TLB
    PhysicsAddr = this->AccessPageTable(LogicAddr);
    AddrMapping NewAddrMapping; //换入TLB的 逻辑地址-物理地址 映射
    NewAddrMapping.LogicAddr = LogicAddr;
    NewAddrMapping.PhysicsAddr = PhysicsAddr;
    if(this->Table.size() >= this->TLBSize) //如果TLB满了则置换
    {
        if(this->ReplaceAlgorOption == 0) //如果是FIFO置换
        {
            this->TLBReplaceFIFO(NewAddrMapping); //FIFO替换
        }
        else //LRU置换
        {
            this->TLBTeplaceLRU(NewAddrMapping); //LRU置换
        }

    }
    else //没满则直接加入
    {
        this->Result->TLBReplaced.LogicAddr = -1; //-1表示被替换的位置为空，也就是直接加入空位置处
        this->Result->TLBReplaced.PhysicsAddr = PhysicsAddr; //加入的位置
        this->Table.push_back(NewAddrMapping);
    }
    return PhysicsAddr;
}

int TLB::GetPhysicsAddrByTLB(int LogicAddr) //查找TLB表
{
    int PhySicsAddr = -1;
    for(int i=0;i<this->Table.size();i++)
    {
        if(this->Table[i].LogicAddr == LogicAddr) //TLB命中
        {
            PhySicsAddr = this->Table[i].PhysicsAddr;
            if(this->ReplaceAlgorOption == 1) //如果是LRU置换算法，则更新访问记录
            {
                AddrMapping TmpAddrMapping = this->Table[i];
                this->Table.erase(this->Table.begin()+i);
                this->Table.push_back(TmpAddrMapping); //将当前记录放到最上
            }
            break;
        }
    }
    return PhySicsAddr;
}

int TLB::AccessPageTable(int LogicAddr) //访问页表
{
    return this->PageTable_Obj->GetPhysicsAddr(LogicAddr);
}

void TLB::TLBReplaceFIFO(AddrMapping NewMapping) //FIFO置换TLB
{
    this->Result->TLBReplaced = this->Table[this->FIFOPos]; //记录被替换的TLB条目
    this->Table[this->FIFOPos] = NewMapping; //替换
    this->FIFOPos = (this->FIFOPos+1)%this->Table.size(); //更新当前TLB最早换入的页位置
}

void TLB::TLBTeplaceLRU(AddrMapping NewMapping) //LRU置换TLB
{
    this->Result->TLBReplaced = this->Table[0]; //记录被替换的TLB条目
    this->Table.erase(this->Table.begin()); //换出最近最少使用的条目
    this->Table.push_back(NewMapping); //换入并放到顶端
}

void TLB::SetPageTable(PageTable *NewPageTable) //设置新的页表指针
{
    this->PageTable_Obj = NewPageTable;
}

void TLB::SetTLBSize(int NewTLBSize) //设置TLB大小
{
    this->TLBSize = NewTLBSize;
}

void TLB::RefreshResult(AccessResultInfo *Result) //更新访问结果指针
{
    this->Result = Result;
}
