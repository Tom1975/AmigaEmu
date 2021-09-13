#pragma once


#include <QDialog>
#include "AmigaEmulation.h"

namespace Ui {
class CopperDialog;
}

class CopperDialog : public QDialog, IUpdate
{
    Q_OBJECT

public:
    explicit CopperDialog(QWidget *parent = 0);
    ~CopperDialog();

    // Init dialog
    void SetEmulator(AmigaEmulation* emu_handler);
    virtual bool event(QEvent *event);
    void Break();

   public slots:

    // Update the view
    virtual void Update();

protected:
   // Menu action
   void UpdateDebug();

   QString Decode(unsigned short instr1, unsigned short instr2);

private:

    Ui::CopperDialog *ui;
    AmigaEmulation* emu_handler_;
};
