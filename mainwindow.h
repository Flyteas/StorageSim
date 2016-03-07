#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QStandardItemModel>
#include <vector>
#include "common.h"
#include "storagesim.h"
/* 主窗口类 */
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void on_HandleNextBtn_clicked(); //处理下一条按钮点击事件
    void on_HandleAllBtn_clicked(); //处理全部按钮点击事件
    void on_LoadTraceBtn_clicked(); //载入数据按钮点击事件
    void on_LoadMappingBtn_clicked();//载入映射按钮点击事件

private:
    Ui::MainWindow *ui;
    StorageSim *StorageSim_Obj; //存储管理模拟类实例
    std::vector<Trace> TraceVec; //所有进程的访问数据，按时间排序
    std::vector<AccessResultInfo*> AccessResults; //访问结果集
    QStandardItemModel *TraceDisplayModel; //Trace数据显示模型
    QStandardItemModel *ResultDisplayModel; //Result数据显示模型
    bool LoadTrace(int ProcessID, QString TraceFilePath); //载入进程Trace数据
    bool LoadPara(QString ParaFilePath); //载入配置参数
    void HandleProcessAccess(); //处理进程访问
    void HandleAllProcessAccess(); //处理全部进程访问，并计算每个进程的平均等待时间和执行时间
    void RefreshTraceTableView(); //刷新Trace数据显示表

    int MemorySize; //内存可容纳的页框数
    int TLBSize; //TLB大小
    int TLBTime; //TLB访问时间
    int MemoryTime; //内存访问时间
    int DiskSeekTime; //外存寻道时间
    int DiskRotaTime; //外存旋转时间
    int DiskTransTime; //外存传输数据时间
    int ReplaceAlgorOption; //置换算法，0表示FIFO，1表示LRU
    int ReplaceStratOption; //置换策略，0表示全局置换，表示局部置换
    QString DiskMappingFilePath; //硬盘映射数据文件路径
};

#endif // MAINWINDOW_H
