#pragma once 

#include <QMainWindow>
#include <QLabel.h>
#include "DebugDialog.h"
#include "MemoryDialog.h"
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
    void SaveConfig();
    void LoadConfig();

public slots:
   void Update();

protected:
   // Menu action
   

private:
    Ui::MainWindow *ui;
    AmigaEmulation* emu_handler_;;

    DebugDialog debug_;
    MemoryDialog memory_;

    // Ressources
    QPixmap* led_on_;
    QPixmap* led_off_;
    QLabel* iconled_;
};

