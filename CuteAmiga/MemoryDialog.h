#pragma once


#include <QDialog>
#include "AmigaEmulation.h"
#include "Disassembler68k.h"

namespace Ui {
class MemoryDialog;
}

class MemoryDialog : public QDialog, IUpdate
{
    Q_OBJECT

public:
    explicit MemoryDialog(QWidget *parent = 0);
    ~MemoryDialog();

    // Init dialog
    void SetEmulator(AmigaEmulation* emu_handler);
    virtual bool event(QEvent *event);
    void Break();

   public slots:

    // Update the view
    virtual void Update();
    void on_setAddress_clicked();
    void on_address_returnPressed();

    void on_searchAddress_clicked();
    void on_pattern_returnPressed();

protected:
   // Menu action
   
   // Disassembly
   void UpdateDebug();


private:

   unsigned int start_addr_;
   Ui::MemoryDialog *ui;
   AmigaEmulation* emu_handler_;
};
