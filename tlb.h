#ifndef TLB_H
#define TLB_H
#include "pagetable.h"
#include "common.h"
#include <vector>
/* TLB类 */
using namespace std;
class TLB
{
public:
    TLB(PageTable *PageTable_Input,int TLBSize_Input,int TLBTime_Input,int ReplaceAlgorOption,int ReplaceStratOption); //构造函数
    int GetPhysicsAddr(int LogicAddr); //通过逻辑地址获取物理地址，响应进程的请求，也就是供进程调用
    void SetTLBSize(int NewTLBSize); //设置TLB大小
    void SetPageTable(PageTable* NewPageTable); //设置页表指针
    void RefreshResult(AccessResultInfo *Result); //更新结果信息指针
private:
    int ReplaceAlgorOption; //置换算法，0表示FIFO，1表示LRU
    int ReplaceStratOption; //置换策略，0表示全局置换，表示局部置换
    int TLBTime; //访问TLB所需时间
    vector<AddrMapping> Table; //TLB表
    int GetPhysicsAddrByTLB(int LogicAddr); //查找TLB表,查找失败返回-1
    int AccessPageTable(int LogicAddr); //访问页表获取物理地址
    void TLBReplaceFIFO(AddrMapping NewMapping); //使用FIFO替换TLB
    void TLBTeplaceLRU(AddrMapping NewMapping); //使用LRU替换TLB
    int TLBSize; //TLB大小
    PageTable *PageTable_Obj; //内存指针
    int FIFOPos; //当前TLB里最早换入的位置
    AccessResultInfo *Result; //访问结果信息
};

#endif // TLB_H
