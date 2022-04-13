#pragma once 

#include <QMainWindow>
#include <QLabel.h>
#include "DebugDialog.h"
#include "MemoryDialog.h"
#include "CopperDialog.h"
#include "ExecDialog.h"
#include "BitplaneDialog.h"
#include "DiskDialog.h"
#include "AmigaEmulation.h"
#include "IDragNDropTarget.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow, public IDragNDropTarget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    virtual void OpenFiles(const QStringList& pathList);
    virtual void closeEvent(QCloseEvent *event);
    void InitEmulation();
    void Break();
    void Memory();
    void Copper();
    void Exec();
    void Bitplane();
    void DiskDebug();
    void SaveConfig();
    void LoadConfig();

    void InsertDisk();

public slots:
   void Update();

protected:
   // Menu action
   

private:
    Ui::MainWindow *ui;
    AmigaEmulation* emu_handler_;;

    DebugDialog debug_;
    MemoryDialog memory_;
    CopperDialog copper_;
    ExecDialog exec_;
    BitplaneDialog bitplane_;
    DiskDialog disk_debug_;

    // Configuration
    QString df0_path_;

    // Ressources
    QPixmap* led_on_;
    QPixmap* led_off_;
    QPixmap* drive_led_on_;
    QPixmap* drive_led_off_;


    QLabel* iconled_;

    QLabel icondrive_[4];
};

