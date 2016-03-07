#include "disk.h"
#include <QFile>

Disk::Disk(int SeekTimeInput,int RotaTimeInput,int TransferTimeInput) //构造函数
{
    this->SeekTime = SeekTimeInput;
    this->RotaTime = RotaTimeInput;
    this->TransferTime = TransferTimeInput;
    this->SectorPos = 0;
    this->TrackPos = 0;
}

int Disk::GetData(int LogicAddr) //读取硬盘数据
{
    int TargetTrackPos; //目标磁道号
    int TargetSectorPos; //目标扇区号
    int DiskAddr = -1;
    this->Result->DiskAccess = true;
    for(int i=0;i<this->DiskStorageData.size();i++)
    {
        if(this->DiskStorageData[i].LogicAddr == LogicAddr)
        {
            DiskAddr = this->DiskStorageData[i].DiskAddr;
            break;
        }
    }
    TargetSectorPos = DiskAddr%256;
    TargetTrackPos = DiskAddr/256;
    this->Result->AccessTime += qAbs(TargetTrackPos - this->TrackPos)*this->SeekTime; //寻道时间
    this->Result->AccessTime += qAbs(TargetSectorPos - this->SectorPos)*this->RotaTime; //旋转时间
    this->Result->AccessTime += this->TransferTime;
    this->TrackPos = TargetTrackPos; //更新磁道位置
    this->SectorPos = TargetSectorPos; //更新扇区位置
    return DiskAddr;
}

bool Disk::LoadDataFile(QString MappingFilePath) //载入外部数据文件
{
    QFile MappingFile(MappingFilePath);
    if(!MappingFile.open(QIODevice::ReadOnly | QIODevice::Text)) //打开失败
    {
        return false;
    }
    vector<DiskData> ClearTmpVec;
    this->DiskStorageData.swap(ClearTmpVec); //清空元素
    while(!MappingFile.atEnd())
    {
        QByteArray LineQBA = MappingFile.readLine();
        QString LineStr(LineQBA);
        DiskData DataAdd;
        DataAdd.LogicAddr = LineStr.mid(0,LineStr.indexOf(" ")).toInt();
        DataAdd.DiskAddr = LineStr.mid(LineStr.indexOf(" ")+1).toInt();
        this->DiskStorageData.push_back(DataAdd);
    }
    MappingFile.close();
    return true;
}

void Disk::RefreshResult(AccessResultInfo *Result) //更新结果信息指针
{
    this->Result = Result;
}
