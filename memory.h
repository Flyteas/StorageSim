#ifndef MEMORY_H
#define MEMORY_H
#include <vector>
#include <QString>
#include "common.h"
/* 内存类 */
using namespace std;
struct MemoryData //内存数据结构体
{
    int PhysicsAddr;  //物理地址
    int Data; //数据
};
class Memory
{
public:
    Memory(int MemoryTimeInput); //构造函数
    int GetData(int PhysicsAddr); //根据物理地址返回内存数据
    void AddData(int PhysicsAddr,int Data); //根据物理地址和数据添加内存数据
    void ReplaceData(int PhysicsAddr,int Data); //根据物理地址和数据替换内存数据
    void RefreshResult(AccessResultInfo *Result); //更新访问结果指针
    int GetUsedSize(); //获取当前内存已用空间大小
private:
    int MemoryTime; //访问一次内存耗时
    vector<MemoryData> StorageData; //内存数据
    AccessResultInfo *Result; //访问结果
};

#endif // MEMORY_H
