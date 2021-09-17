#include "DebugDialog.h"
#include "ui_DebugDialog.h"

#include <QDir>
#include <QMenuBar>

DebugDialog::DebugDialog(QWidget *parent) :
   QDialog(parent),
   ui(new Ui::DebugDialog)
{
   ui->setupUi(this);
   ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
   connect(ui->listWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(DasmShowContextMenu(QPoint)));

   ui->registers_list_->clear();
   ui->registers_list_->setColumnCount(2);
   ui->registers_list_->setRowCount(20);

   QTableWidgetItem  * item = new QTableWidgetItem("pc");
   ui->registers_list_->setItem(0, 0, item);

   item = new QTableWidgetItem("D0");
   ui->registers_list_->setItem(1, 0, item);
   ui->registers_list_->setItem(2, 0, new QTableWidgetItem("D1"));
   ui->registers_list_->setItem(3, 0, new QTableWidgetItem("D2"));
   ui->registers_list_->setItem(4, 0, new QTableWidgetItem("D3"));
   ui->registers_list_->setItem(5, 0, new QTableWidgetItem("D4"));
   ui->registers_list_->setItem(6, 0, new QTableWidgetItem("D5"));
   ui->registers_list_->setItem(7, 0, new QTableWidgetItem("D6"));
   ui->registers_list_->setItem(8, 0, new QTableWidgetItem("D7"));

   ui->registers_list_->setItem(9, 0, new QTableWidgetItem("A0"));
   ui->registers_list_->setItem(10, 0, new QTableWidgetItem("A1"));
   ui->registers_list_->setItem(11, 0, new QTableWidgetItem("A2"));
   ui->registers_list_->setItem(12, 0, new QTableWidgetItem("A3"));
   ui->registers_list_->setItem(13, 0, new QTableWidgetItem("A4"));
   ui->registers_list_->setItem(14, 0, new QTableWidgetItem("A5"));
   ui->registers_list_->setItem(15, 0, new QTableWidgetItem("A6"));
   ui->registers_list_->setItem(16, 0, new QTableWidgetItem("A7"));

   ui->registers_list_->setItem(17, 0, new QTableWidgetItem("SSP"));
   ui->registers_list_->setItem(18, 0, new QTableWidgetItem("USP"));

   ui->registers_list_->setItem(19, 0, new QTableWidgetItem("SR"));

   for (int i = 0; i < 20; i++)
   {
      ui->registers_list_->setItem(i, 1, new QTableWidgetItem("--"));
   }

}

DebugDialog::~DebugDialog()
{
}

bool DebugDialog::event(QEvent *event)
{
   if ( event->type() == QEvent::User )
   {
      UpdateDebug();
      return true;
   }
   return QWidget::event(event);
}

void DebugDialog::SetEmulator(AmigaEmulation* emu_handler)
{
   emu_handler_ = emu_handler;
   emu_handler_->AddUpdateListener(this);
}

void DebugDialog::Break()
{
   // wait for the emulator to be in a stable state
}

void DebugDialog::on_dbg_step__clicked()
{
   emu_handler_->Step();
}

void DebugDialog::on_dbg_run_clicked()
{
   emu_handler_->Run();
}

void DebugDialog::on_dbg_pause_clicked()
{
   emu_handler_->Break();
}

void DebugDialog::Update()
{
   // Send "update" command
   QEvent* event = new QEvent(QEvent::User);
   QCoreApplication::postEvent( this, event);

}

void DebugDialog::DasmShowContextMenu(const QPoint &pos)
{
   // Handle global position
   QPoint globalPos = ui->listWidget->mapToGlobal(pos);

   // Create menu and insert some actions
   QMenu myMenu;
   myMenu.addAction("Insert breakpoint", this, SLOT(AddBreakpoint()));
   myMenu.addAction("Erase", this, SLOT(RemoveBreakpoint()));

   // Show context menu at handling position
   myMenu.exec(globalPos);
}

void DebugDialog::RemoveBreakpoint()
{
   QList<QListWidgetItem *> list_of_items = ui->listWidget->selectedItems();
   // todo : handle multiple selection
   emu_handler_->RemoveBreakpoint(list_of_items[0]->data(Qt::UserRole).toUInt());
   UpdateDebug();
}


void DebugDialog::AddBreakpoint()
{
   QList<QListWidgetItem *> list_of_items = ui->listWidget->selectedItems();
   // todo : handle multiple selection
   emu_handler_->AddBreakpoint(list_of_items[0]->data(Qt::UserRole).toUInt());
   UpdateDebug();
}

void DebugDialog::on_add_bp_clicked()
{
   QString text = ui->textEdit->toPlainText();
   BreakPointHandler* pb_handler = emu_handler_->GetBreakpointHandler();
   pb_handler->CreateBreakpoint(text.toUtf8().constData());
   UpdateDebug();
}

void DebugDialog::on_remove_bp_clicked()
{
   QList<QListWidgetItem *> list_of_items = ui->list_breakpoints->selectedItems();
   BreakPointHandler* pb_handler = emu_handler_->GetBreakpointHandler();
   // todo : handle multiple selection
   emu_handler_->RemoveBreakpoint(pb_handler->GetBreakpoint(list_of_items[0]->data(Qt::UserRole).toUInt()));
   UpdateDebug();
}

void DebugDialog::on_clear_bp_clicked()
{
   BreakPointHandler* pb_handler = emu_handler_->GetBreakpointHandler();
   pb_handler->Clear();
   UpdateDebug();
}

void DebugDialog::UpdateDebug()
{
   // Update parent window
   this->parentWidget()->repaint();

   // Get CPU
   M68k* m68k = emu_handler_->GetCpu();

   // Registers
   QString str = QString("%1").arg(m68k->GetPc(), 6, 16);
   ui->registers_list_->item(0, 1)->setText( str );
   for (int i = 0; i < 8; i++)
   {
      str = QString("%1").arg(m68k->GetAddressRegister(i), 6, 16);
      ui->registers_list_->item(9+i, 1)->setText(str);

      str = QString("%1").arg(m68k->GetDataRegister(i), 6, 16);
      ui->registers_list_->item(1+i, 1)->setText(str);
   }

   str = QString("%1").arg(m68k->GetDataSsp(), 6, 16);
   ui->registers_list_->item(17, 1)->setText(str);
   str = QString("%1").arg(m68k->GetDataUsp(), 6, 16);
   ui->registers_list_->item(18, 1)->setText(str);
   str = QString("%1").arg(m68k->GetDataSr(), 6, 16);
   ui->registers_list_->item(19, 1)->setText(str);

   // Disassemble the next lines
   unsigned int offset, offset_old;
   offset = offset_old = emu_handler_->GetMotherboard()->GetCpu()->GetPc() - 4; // -2 because of prefetch
   std::string str_asm;
   ui->listWidget->clear();
   char addr[16];
   for (int i = 0; i < 32; i++)
   {
#define ASM_SIZE 26
#define ADD_SIZE 10

      sprintf(addr, "%8.8X: ", offset);
      QListWidgetItem* item = new QListWidgetItem;
      item->setData(Qt::UserRole, offset);

      offset = disassembler_.Disassemble(emu_handler_->GetMotherboard(), offset, str_asm);
      str_asm = addr + str_asm;
      int size_tab = (ADD_SIZE+ASM_SIZE) - str_asm.size();
      if (size_tab > 0)
      {
         str_asm.append(size_tab, ' ' );
      }
      for (unsigned int i = offset_old; i < offset; i++)
      {
         char b[4];
         sprintf(b, "%2.2X ", emu_handler_->GetMotherboard()->Read8(i));
         str_asm += b;
      }
      item->setText(str_asm.c_str());
      ui->listWidget->addItem(item);

      //
      offset_old = offset;
   }

   // Breakpoints list
   BreakPointHandler* pb_handler = emu_handler_->GetBreakpointHandler();
   ui->list_breakpoints->clear();
   // Update breakpoints list
   for (int i = 0; i < pb_handler->GetBreakpointNumber(); i++)
   {
      QListWidgetItem* item = new QListWidgetItem;
      item->setText(pb_handler->GetBreakpoint(i)->GetLabel());
      item->setData(Qt::UserRole, i);
      
      ui->list_breakpoints->addItem(item);
   }
}
