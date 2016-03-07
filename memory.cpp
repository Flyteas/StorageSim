#include "memory.h"

Memory::Memory(int MemoryTimeInput) //构造函数
{
    this->MemoryTime = MemoryTimeInput;
}

int Memory::GetData(int PhysicsAddr) //根据物理地址获取内存数据
{
    int Data = -1;
    for(int i=0;i<this->StorageData.size();i++)
    {
        if(this->StorageData[i].PhysicsAddr == PhysicsAddr) //找到
        {
            Data = this->StorageData[i].Data;
        }
    }
    this->Result->AccessTime += this->MemoryTime; //访问内存时间
    return Data;
}

void Memory::AddData(int PhysicsAddr, int Data) //添加数据
{
    MemoryData MemoryDataAdd;
    MemoryDataAdd.PhysicsAddr = PhysicsAddr;
    MemoryDataAdd.Data = Data;
    this->StorageData.push_back(MemoryDataAdd);
    this->Result->MemoryDataReplaced = -1; //-1表示是直接添加到内存
    this->Result->AccessTime += this->MemoryTime; //访问内存时间
}

void Memory::ReplaceData(int PhysicsAddr, int Data) //替换数据
{
    for(int i=0;i<this->StorageData.size();i++)
    {
        if(this->StorageData[i].PhysicsAddr == PhysicsAddr) //找到
        {
            this->Result->MemoryDataReplaced = this->StorageData[i].Data; //记录被替换的数据
            this->StorageData[i].Data = Data;
            break;
        }
    }
    this->Result->AccessTime += this->MemoryTime; //访问内存时间
}

void Memory::RefreshResult(AccessResultInfo *Result) //更新结果指针
{
    this->Result = Result;
}

int Memory::GetUsedSize() //获取当前内存已用空间大小
{
    return this->StorageData.size();
}
