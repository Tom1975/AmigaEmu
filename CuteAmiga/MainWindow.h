#pragma once 

#include <QMainWindow>
#include <QLabel.h>
#include "DebugDialog.h"
#include "MemoryDialog.h"
#include "CopperDialog.h"
#include "ExecDialog.h"
#include "BitplaneDialog.h"
#include "AmigaEmulation.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    virtual void closeEvent(QCloseEvent *event);
    void InitEmulation();
    void Break();
    void Memory();
    void Copper();
    void Exec();
    void Bitplane();
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

    // Ressources
    QPixmap* led_on_;
    QPixmap* led_off_;
    QLabel* iconled_;
};

