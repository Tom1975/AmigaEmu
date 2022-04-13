#pragma once

#include <QDialog>
#include "AmigaEmulation.h"

namespace Ui {
class DiskDialog;
}

class DiskDialog : public QDialog, IUpdate
{
    Q_OBJECT

public:
    explicit DiskDialog(QWidget *parent = 0);
    ~DiskDialog();

    // Init dialog
    void SetEmulator(AmigaEmulation* emu_handler);
    virtual bool event(QEvent *event);

    void UpdateDebug();

public slots:
    // Update the view
    virtual void Update();



protected:
   // Menu action

private:
   Ui::DiskDialog *ui;
   AmigaEmulation* emu_handler_;
};
