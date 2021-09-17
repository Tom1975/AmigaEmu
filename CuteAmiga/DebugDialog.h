#pragma once


#include <QDialog>
#include "AmigaEmulation.h"
#include "Disassembler68k.h"

namespace Ui {
class DebugDialog;
}

class DebugDialog : public QDialog, IUpdate
{
    Q_OBJECT

public:
    explicit DebugDialog(QWidget *parent = 0);
    ~DebugDialog();

    // Init dialog
    void SetEmulator(AmigaEmulation* emu_handler);
    virtual bool event(QEvent *event);
    void Break();

   public slots:
      void on_dbg_pause_clicked();
      void on_dbg_step__clicked();
      void on_dbg_run_clicked();
      void DasmShowContextMenu(const QPoint &pos);
      void AddBreakpoint();
      void RemoveBreakpoint();
      void on_add_bp_clicked();
      void on_remove_bp_clicked();
      void on_clear_bp_clicked();

    // Update the view
    virtual void Update();

protected:
   // Menu action
   
   // Disassembly
   unsigned int Disassemble(M68k* m68k, unsigned int offset, std::string& str_asm);
   void UpdateDebug();


private:

    Ui::DebugDialog *ui;
    AmigaEmulation* emu_handler_;

    Disassembler68k disassembler_;
};
