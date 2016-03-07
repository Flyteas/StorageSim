#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "storagesim.h"
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    /* 默认参数 */
    this->MemorySize = 20;
    this->TLBSize = 10;
    this->TLBTime = 5;
    this->MemoryTime = 100;
    this->DiskSeekTime = 100;
    this->DiskRotaTime = 50;
    this->DiskTransTime = 30000;
    this->ReplaceAlgorOption = 0;
    this->ReplaceStratOption = 0;
    this->DiskMappingFilePath = "mapping.txt";
    this->TraceDisplayModel = new QStandardItemModel;
    this->ui->TraceTableView->setModel(this->TraceDisplayModel);
    this->ui->TraceTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->ResultDisplayModel = new QStandardItemModel;
    this->ui->ResultTableView->setModel(this->ResultDisplayModel);
    this->ui->ResultTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->StorageSim_Obj = new StorageSim(this->MemorySize,this->TLBSize,this->TLBTime,this->MemoryTime,this->DiskSeekTime,this->DiskRotaTime,this->DiskTransTime,this->ReplaceAlgorOption,this->ReplaceStratOption,this->DiskMappingFilePath);
    this->LoadPara("parameter.txt");
}

MainWindow::~MainWindow()
{
    delete this->StorageSim_Obj;
    this->StorageSim_Obj = NULL;
    delete this->TraceDisplayModel;
    delete this->ResultDisplayModel;
    for(int i=0;i<this->AccessResults.size();i++)
    {
        delete this->AccessResults[i];
    }
    delete ui;
}


bool MainWindow::LoadTrace(int ProcessID,QString TraceFilePath) //载入Trace数据
{
    QFile TraceFile(TraceFilePath);
    if(!TraceFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return false;
    }
    while(!TraceFile.atEnd())
    {
        int i = 0;
        QByteArray LineQBA = TraceFile.readLine();
        QString LineStr(LineQBA);
        Trace TraceAdd;
        TraceAdd.ProcessID = ProcessID;
        TraceAdd.AccessTime = LineStr.mid(0,LineStr.indexOf(" ")).toDouble();
        TraceAdd.LogicAddr = LineStr.mid(LineStr.indexOf(" ")+1).toInt();
        for(;i<this->TraceVec.size();i++) //插入排序
        {
            if(this->TraceVec[i].AccessTime > TraceAdd.AccessTime)
            {
                break;
            }
        }
        this->TraceVec.insert(this->TraceVec.begin()+i,TraceAdd);
    }
    TraceFile.close();
    return true;
}

bool MainWindow::LoadPara(QString ParaFilePath) //载入配置参数
{
    QFile ParaFile(ParaFilePath);
    if(!ParaFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return false;
    }
    while(!ParaFile.atEnd())
    {
        QByteArray LineQBA = ParaFile.readLine();
        QString LineStr(LineQBA);
        if(LineStr.indexOf("frame number") != -1)
        {
            this->MemorySize = LineStr.mid(LineStr.indexOf('=')+2,LineStr.indexOf(';')-LineStr.indexOf('=')-2).toInt();
        }
        else if(LineStr.indexOf("TLB entry") != -1)
        {
            this->TLBSize = LineStr.mid(LineStr.indexOf('=')+2,LineStr.indexOf(';')-LineStr.indexOf('=')-2).toInt();
        }
        else if(LineStr.indexOf("TLB time") != -1)
        {
            this->TLBTime = LineStr.mid(LineStr.indexOf('=')+2,LineStr.indexOf(';')-LineStr.indexOf('=')-2).toInt();
        }
        else if(LineStr.indexOf("memory time") != -1)
        {
            this->MemoryTime = LineStr.mid(LineStr.indexOf('=')+2,LineStr.indexOf(';')-LineStr.indexOf('=')-2).toInt();
        }
        else if(LineStr.indexOf("seek time") != -1)
        {
            this->DiskSeekTime = LineStr.mid(LineStr.indexOf('=')+2,LineStr.indexOf(';')-LineStr.indexOf('=')-2).toInt();
        }
        else if(LineStr.indexOf("rotational time") != -1)
        {
            this->DiskRotaTime = LineStr.mid(LineStr.indexOf('=')+2,LineStr.indexOf(';')-LineStr.indexOf('=')-2).toInt();
        }
        else if(LineStr.indexOf("transfer time") != -1)
        {
            this->DiskTransTime = LineStr.mid(LineStr.indexOf('=')+2,LineStr.indexOf(';')-LineStr.indexOf('=')-2).toInt();
        }
        else if(LineStr.indexOf("replacement algorithm") != -1)
        {
            this->ReplaceAlgorOption = LineStr.mid(LineStr.indexOf('=')+2,LineStr.indexOf(';')-LineStr.indexOf('=')-2).toInt();
        }
        else if(LineStr.indexOf("replacement strategy") != -1)
        {
            this->ReplaceStratOption = LineStr.mid(LineStr.indexOf('=')+2,LineStr.indexOf(';')-LineStr.indexOf('=')-2).toInt();
        }
    }
    return true;
}

void MainWindow::RefreshTraceTableView() //刷新Trace数据窗口显示
{
    while(this->TraceDisplayModel->rowCount()>0) //清空原来的显示
    {
        this->TraceDisplayModel->removeRow(0);
    }
    for(int i=0;i<this->TraceVec.size();i++)
    {
        QList<QStandardItem *> TraceData;
        TraceData<<new QStandardItem(QString::number(this->TraceVec[i].ProcessID));
        TraceData<<new QStandardItem(QString::number(this->TraceVec[i].AccessTime));
        TraceData<<new QStandardItem(QString::number(this->TraceVec[i].LogicAddr));
        this->TraceDisplayModel->appendRow(TraceData);
    }
    this->TraceDisplayModel->setHorizontalHeaderItem(0,new QStandardItem("进程号"));
    this->TraceDisplayModel->setHorizontalHeaderItem(1,new QStandardItem("访问时间"));
    this->TraceDisplayModel->setHorizontalHeaderItem(2,new QStandardItem("逻辑地址"));
}

void MainWindow::HandleProcessAccess() //处理进程访问
{
    if(!this->TraceVec.size()>0) //待处理请求为空
    {
        return;
    }
    int ProcessID = this->TraceVec[0].ProcessID;
    int LogicAddr = this->TraceVec[0].LogicAddr;
    int AccessBeginTime = this->TraceVec[0].AccessTime; //访问到来的时间
    AccessResultInfo *Result = new AccessResultInfo; //初始化结果信息
    Result->ProcessID = -1;
    Result->LogicAddr = -1;
    Result->PhysicsAddr = -1;
    Result->AccessTime = 0;
    Result->WaittingTime = 0;
    Result->MemoryData = -1;
    Result->TLBHit = false;
    Result->TLBReplaced.LogicAddr = -1;
    Result->TLBReplaced.PhysicsAddr = -1;
    Result->PageTableHit = false;
    Result->PageTableReplaced.LogicAddr = -1;
    Result->PageTableReplaced.PhysicsAddr = -1;
    Result->MemoryDataReplaced = false;
    Result->DiskAccess = false;
    this->StorageSim_Obj->Access(ProcessID,AccessBeginTime,LogicAddr,Result);
    if(this->AccessResults.size()>0)
    {
        Result->WaittingTime = this->AccessResults[this->AccessResults.size()-1]->AccessBeginTime*1000000 + this->AccessResults[this->AccessResults.size()-1]->WaittingTime + this->AccessResults[this->AccessResults.size()-1]->AccessTime - Result->AccessBeginTime*1000000;
        if(Result->WaittingTime<0)
        {
            Result->WaittingTime = 0;
        }
    }
    this->TraceVec.erase(this->TraceVec.begin());
    this->RefreshTraceTableView();
    this->AccessResults.push_back(Result);
    /* 更新结果显示 */
    this->ResultDisplayModel->clear();
    QList<QStandardItem *> Result_ProcessID; //进程号
    QList<QStandardItem *> Result_AccessTime;//访问耗时
    QList<QStandardItem *> Result_WaittingTime;//等待时间
    QList<QStandardItem *> Result_LogicAddr; //访问的逻辑地址
    QList<QStandardItem *> Result_PhysicsAddr; //获取的物理地址
    QList<QStandardItem *> Result_MemoryData; //读取到的内存数据
    QList<QStandardItem *> Result_TLBHit; //TLB是否命中
    QList<QStandardItem *> Result_PageTableHit; //页表是否命中
    QList<QStandardItem *> Result_DiskAccess; //是否访问外存
    QList<QStandardItem *> Result_MemoryDataReplaced; //内存被置换的数据
    QList<QStandardItem *> Result_TLBReplaced; //TLB被置换的条目
    QList<QStandardItem *> Result_PageTableReplaced; //页表被置换的条目

    Result_ProcessID<<new QStandardItem("进程号");
    Result_ProcessID<<new QStandardItem(QString::number(Result->ProcessID));
    this->ResultDisplayModel->appendRow(Result_ProcessID);

    Result_WaittingTime<<new QStandardItem("等待时间(ns)");
    Result_WaittingTime<<new QStandardItem(QString::number((Result->WaittingTime)));
    this->ResultDisplayModel->appendRow(Result_WaittingTime);

    Result_AccessTime<<new QStandardItem("执行时间(ns)");
    Result_AccessTime<<new QStandardItem(QString::number((Result->AccessTime)));
    this->ResultDisplayModel->appendRow(Result_AccessTime);

    Result_LogicAddr<<new QStandardItem("逻辑地址");
    Result_LogicAddr<<new QStandardItem(QString::number(Result->LogicAddr));
    this->ResultDisplayModel->appendRow(Result_LogicAddr);

    Result_PhysicsAddr<<new QStandardItem("物理地址");
    Result_PhysicsAddr<<new QStandardItem(QString::number(Result->PhysicsAddr));
    this->ResultDisplayModel->appendRow(Result_PhysicsAddr);

    Result_MemoryData<<new QStandardItem("内存数据");
    Result_MemoryData<<new QStandardItem(QString::number(Result->MemoryData));
    this->ResultDisplayModel->appendRow(Result_MemoryData);

    Result_TLBHit<<new QStandardItem("TLB命中");
    if(Result->TLBHit)
    {
        Result_TLBHit<<new QStandardItem("是");
    }
    else
    {
        Result_TLBHit<<new QStandardItem("否");
    }
    this->ResultDisplayModel->appendRow(Result_TLBHit);

    Result_TLBReplaced<<new QStandardItem("TLB置换");
    if(!Result->TLBHit)
    {
        if(Result->TLBReplaced.LogicAddr == -1) //TLB还有空间，直接加入数据
        {
            Result_TLBReplaced<<new QStandardItem("加入TLB空位置"+QString::number(Result->TLBReplaced.PhysicsAddr));
        }
        else //置换信息
        {
            Result_TLBReplaced<<new QStandardItem("逻辑地址"+QString::number(Result->TLBReplaced.LogicAddr)+"--物理地址"+QString::number(Result->TLBReplaced.PhysicsAddr)+"被置换");
        }
    }
    this->ResultDisplayModel->appendRow(Result_TLBReplaced);

    Result_PageTableHit<<new QStandardItem("页表命中");
    if(!Result->TLBHit)
    {
        if(Result->PageTableHit)
        {
            Result_PageTableHit<<new QStandardItem("是");
        }
        else
        {
            Result_PageTableHit<<new QStandardItem("否");
        }
    }
    this->ResultDisplayModel->appendRow(Result_PageTableHit);

    Result_PageTableReplaced<<new QStandardItem("页表置换");
    if((!Result->PageTableHit)&&(!Result->TLBHit))
    {
        if(Result->PageTableReplaced.LogicAddr == -1) //还有空间，直接加入
        {
            Result_PageTableReplaced<<new QStandardItem("加入页表空位置"+QString::number(Result->PageTableReplaced.PhysicsAddr));
        }
        else
        {
            Result_PageTableReplaced<<new QStandardItem("逻辑地址"+QString::number(Result->PageTableReplaced.LogicAddr)+"--物理地址"+QString::number(Result->PageTableReplaced.PhysicsAddr)+"被置换");
        }
    }
    this->ResultDisplayModel->appendRow(Result_PageTableReplaced);

    Result_MemoryDataReplaced<<new QStandardItem("内存置换");
    if((!Result->PageTableHit)&&(!Result->TLBHit))
    {
        if(Result->MemoryDataReplaced != -1) //被置换，而不是直接加入空位置
        {
            Result_MemoryDataReplaced<<new QStandardItem("数据"+QString::number(Result->MemoryDataReplaced)+"被置换");
        }
    }
    this->ResultDisplayModel->appendRow(Result_MemoryDataReplaced);

    Result_DiskAccess<<new QStandardItem("访问外存");
    if(Result->DiskAccess)
    {
        Result_DiskAccess<<new QStandardItem("是");
    }
    else
    {
        Result_DiskAccess<<new QStandardItem("否");
    }
    this->ResultDisplayModel->appendRow(Result_DiskAccess);
}

void MainWindow::HandleAllProcessAccess() //处理所有进程访问请求
{
    if(!this->TraceVec.size()>0) //待处理列表为空
    {
        return;
    }
    while(this->TraceVec.size()>0)
    {
        int ProcessID = this->TraceVec[0].ProcessID;
        int LogicAddr = this->TraceVec[0].LogicAddr;
        int AccessBeginTime = this->TraceVec[0].AccessTime; //访问到来的时间
        AccessResultInfo *Result = new AccessResultInfo; //初始化结果信息
        Result->ProcessID = -1;
        Result->LogicAddr = -1;
        Result->PhysicsAddr = -1;
        Result->AccessTime = 0;
        Result->WaittingTime = 0;
        Result->MemoryData = -1;
        Result->TLBHit = false;
        Result->TLBReplaced.LogicAddr = -1;
        Result->TLBReplaced.PhysicsAddr = -1;
        Result->PageTableHit = false;
        Result->PageTableReplaced.LogicAddr = -1;
        Result->PageTableReplaced.PhysicsAddr = -1;
        Result->MemoryDataReplaced = false;
        Result->DiskAccess = false;
        this->StorageSim_Obj->Access(ProcessID,AccessBeginTime,LogicAddr,Result);
        if(this->AccessResults.size()>0)
        {
            Result->WaittingTime = this->AccessResults[this->AccessResults.size()-1]->AccessBeginTime*1000000 + this->AccessResults[this->AccessResults.size()-1]->WaittingTime + this->AccessResults[this->AccessResults.size()-1]->AccessTime - Result->AccessBeginTime*1000000;
            if(Result->WaittingTime<0)
            {
                Result->WaittingTime = 0;
            }
        }
        this->TraceVec.erase(this->TraceVec.begin());
        this->RefreshTraceTableView();
        this->AccessResults.push_back(Result);
    }
    /* 更新结果显示 */
    while(this->ResultDisplayModel->rowCount()>0) //清空原来的显示
    {
        this->ResultDisplayModel->removeRow(0);
    }
    for(int i=1;i<=5;i++) //计算所有进程的平均等待时间和平均执行时间
    {
        int AccessNums = 0; //此进程总共访问次数
        int WaittingTimeTotal = 0;//总计等待时间
        int AccessTimeTotal = 0; //总计执行时间
        int WaittingTimeAvg; //平均等待时间;
        int AccessTimeAvg; //平均执行时间
        for(int j=0;j<this->AccessResults.size();j++)
        {
            if(this->AccessResults[j]->ProcessID == i)
            {
                AccessNums++;
                WaittingTimeTotal += this->AccessResults[j]->WaittingTime;
                AccessTimeTotal += this->AccessResults[j]->AccessTime;
            }
        }
        WaittingTimeAvg = WaittingTimeTotal/AccessNums; //计算平均等待时间
        AccessTimeAvg = AccessTimeTotal/AccessNums; //计算平均执行时间
        QList<QStandardItem *> Result_Avg; //显示结果
        Result_Avg<<new QStandardItem(QString::number(i));
        Result_Avg<<new QStandardItem(QString::number(WaittingTimeAvg));
        Result_Avg<<new QStandardItem(QString::number(AccessTimeAvg));
        this->ResultDisplayModel->appendRow(Result_Avg);
    }

    this->ResultDisplayModel->setHorizontalHeaderItem(0,new QStandardItem("进程号"));
    this->ResultDisplayModel->setHorizontalHeaderItem(1,new QStandardItem("平均等待时间(ns)"));
    this->ResultDisplayModel->setHorizontalHeaderItem(2,new QStandardItem("平均执行时间(ns)"));
    QMessageBox::information(0,"处理完毕","所有访问请求处理完毕!");
}


void MainWindow::on_LoadTraceBtn_clicked() //载入Trace数据按钮点击事件
{
    QStringList TraceFilePathList;
    delete this->StorageSim_Obj;
    this->StorageSim_Obj = new StorageSim(this->MemorySize,this->TLBSize,this->TLBTime,this->MemoryTime,this->DiskSeekTime,this->DiskRotaTime,this->DiskTransTime,this->ReplaceAlgorOption,this->ReplaceStratOption,this->DiskMappingFilePath);
    for(int i=0;i<this->AccessResults.size();i++) //清空结果集
    {
        delete this->AccessResults[i];
    }
    vector<AccessResultInfo*> ClearTmpVec;
    this->AccessResults.swap(ClearTmpVec); //清空元素

    TraceFilePathList = QFileDialog::getOpenFileNames();
    for(int i=0;i<TraceFilePathList.size();i++)
    {
        this->LoadTrace(TraceFilePathList[i].mid(TraceFilePathList[i].length()-5,1).toInt(),TraceFilePathList[i]);
    }
    this->RefreshTraceTableView(); //更新显示
}

void MainWindow::on_HandleNextBtn_clicked() //处理下一条按钮点击事件
{
    this->HandleProcessAccess();
}

void MainWindow::on_HandleAllBtn_clicked() //处理全部按钮点击事件
{
    this->HandleAllProcessAccess();
}

void MainWindow::on_LoadMappingBtn_clicked() //载入映射按钮点击事件
{
    this->DiskMappingFilePath = QFileDialog::getOpenFileName();
    delete this->StorageSim_Obj;
    this->StorageSim_Obj = new StorageSim(this->MemorySize,this->TLBSize,this->TLBTime,this->MemoryTime,this->DiskSeekTime,this->DiskRotaTime,this->DiskTransTime,this->ReplaceAlgorOption,this->ReplaceStratOption,this->DiskMappingFilePath);
    for(int i=0;i<this->AccessResults.size();i++) //清空结果集
    {
        delete this->AccessResults[i];
    }
    vector<AccessResultInfo*> ClearTmpVec;
    this->AccessResults.swap(ClearTmpVec); //清空元素
}
