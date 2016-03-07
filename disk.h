#ifndef DISK_H
#define DISK_H
#include <vector>
#include <QString>
#include "common.h"
/* 外存类 */
using namespace std;
struct DiskData //外存数据结构体
{
    int LogicAddr; //逻辑地址
    int DiskAddr; //磁盘物理地址
};

class Disk
{
public:
    Disk(int SeekTimeInput, int RotaTimeInput, int TransferTimeInput); //构造函数
    bool LoadDataFile(QString MappingFilePath); //载入外部数据文件
    int GetData(int LogicAddr); //读取数据
    void RefreshResult(AccessResultInfo *Result); //更新结果信息指针
private:
    int SeekTime; //寻道时间
    int RotaTime; //旋转时间
    int TransferTime; //传输时间
    int TrackPos; //当前磁道位置
    int SectorPos; //当前扇区位置
    vector<DiskData> DiskStorageData; //外存数据
    AccessResultInfo *Result; //访问结果
};

#endif // DISK_H
