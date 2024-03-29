#include "DebugDialog.h"
#include "ui_DebugDialog.h"

#include <sstream>
#include <iostream>
#include <iomanip>

#include <QDir>
#include <QMenuBar>
#include <QDebug>

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

void DebugDialog::on_set_top_address_clicked()
{
   // get top 
   QString text = ui->dasm_address->text();
   if (text.length() > 0)
   {
      unsigned int addr = (unsigned int)strtoul(text.toUtf8().constData(), NULL, 16);
      if (addr >= 0 && addr < 0xFFFFFF)
      {
         // Set disassembly
         UpdateDisassembly(addr);

         // Write it on the log window
         std::string out_txt;
         disassembler_.DisassembleArrayOfcode(emu_handler_->GetMotherboard(), addr, 512, out_txt);
         // Log it !
         qDebug() << out_txt.c_str();

      }
   }
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

void DebugDialog::on_bpAddress_returnPressed()
{
   on_add_bp_clicked();
}

void DebugDialog::on_add_bp_clicked()
{
   QString text = ui->bpAddress->text();
   if (text.length() > 0)
   {
      BreakPointHandler* pb_handler = emu_handler_->GetBreakpointHandler();
      pb_handler->CreateBreakpoint(text.toUtf8().constData());
      UpdateDebug();
   }
}

void DebugDialog::on_remove_bp_clicked()
{
   QList<QListWidgetItem *> list_of_items = ui->list_breakpoints->selectedItems();
   if (list_of_items.size() > 0)
   {
      BreakPointHandler* pb_handler = emu_handler_->GetBreakpointHandler();
      // todo : handle multiple selection
      emu_handler_->RemoveBreakpoint(pb_handler->GetBreakpoint(list_of_items[0]->data(Qt::UserRole).toUInt()));
      UpdateDebug();
   }
}

void DebugDialog::on_clear_bp_clicked()
{
   BreakPointHandler* pb_handler = emu_handler_->GetBreakpointHandler();
   pb_handler->Clear();
   UpdateDebug();
}

void DebugDialog::UpdateDebug()
{
   unsigned int offset, offset_old;

   // Update parent window
   this->parentWidget()->repaint();

   // Get CPU
   M68k* m68k = emu_handler_->GetCpu();

   // Registers
   QString str = QString("%1").arg(m68k->GetPc(), 6, 16);
   ui->registers_list_->item(0, 1)->setText( str );

   unsigned int stack_pointer = (m68k->GetDataSr() & 0x2000) ? m68k->GetDataSsp() : m68k->GetDataUsp();
   for (int i = 0; i < 8; i++)
   {
      if (i != 7)
      {
         str = QString("%1").arg(m68k->GetAddressRegister(i), 6, 16);
      }
      else
      {
         
         // A7 is the stackpointer. Depends on either usp/ssp
         str = QString("%1").arg(stack_pointer, 6, 16);
      }

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

   // CallStack
   unsigned char* mem = emu_handler_->GetMotherboard()->GetBus()->GetRam();
   str = QString("%1").arg(stack_pointer, 6, 16);
   offset = stack_pointer;
   ui->callStack->clear();
   if (stack_pointer < 512 * 1024)
   {
      for (int i = 0; i < 16; i++)
      {
         std::stringstream sstream;
         QListWidgetItem* stackitem = new QListWidgetItem;
         stackitem->setData(Qt::UserRole, offset);

         unsigned int dword_value = mem[offset+3] |
            (mem[offset + 2] << 8) |
            (mem[offset + 1] << 16) |
            (mem[offset] << 24);
                                    
         sstream << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << offset;
         sstream << "  ";
         sstream << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << dword_value;

         stackitem->setText(sstream.str().c_str());

         ui->callStack->addItem(stackitem);
         offset += 4;
      }
   }

   offset = offset_old = emu_handler_->GetMotherboard()->GetCpu()->GetPc() - 4; // -2 because of prefetch
   UpdateDisassembly(offset);

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

void DebugDialog::UpdateDisassembly(unsigned int offset)
{
   char addr[16];
   unsigned int offset_old = offset;

   // Disassemble the next lines
   std::string str_asm;
   ui->listWidget->clear();
   for (int i = 0; i < 32; i++)
   {
#define ASM_SIZE 26
#define ADD_SIZE 10

      sprintf(addr, "%8.8X: ", offset);
      QListWidgetItem* item = new QListWidgetItem;
      item->setData(Qt::UserRole, offset);

      offset = disassembler_.Disassemble(emu_handler_->GetMotherboard(), offset, str_asm);
      str_asm = addr + str_asm;
      int size_tab = (ADD_SIZE + ASM_SIZE) - str_asm.size();
      if (size_tab > 0)
      {
         str_asm.append(size_tab, ' ');
      }
      for (unsigned int j = offset_old; j < offset; j++)
      {
         char b[4];
         sprintf(b, "%2.2X ", emu_handler_->GetMotherboard()->Read8(j));
         str_asm += b;
      }
      item->setText(str_asm.c_str());
      ui->listWidget->addItem(item);

      //
      offset_old = offset;
   }
}
