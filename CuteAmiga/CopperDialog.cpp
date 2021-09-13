#include "CopperDialog.h"
#include "ui_CopperDialog.h"

#include <QDir>
#include <QMenuBar>

CopperDialog::CopperDialog(QWidget *parent) :
   QDialog(parent),
   ui(new Ui::CopperDialog)
{
   ui->setupUi(this);
   ui->registers_list_->clear();
   ui->registers_list_->setColumnCount(2);
   ui->registers_list_->setRowCount(17);

   ui->registers_list_->setItem(0, 0, new QTableWidgetItem("State"));
   ui->registers_list_->setItem(1, 0, new QTableWidgetItem("Counter"));
   ui->registers_list_->setItem(2, 0, new QTableWidgetItem("Con"));
   ui->registers_list_->setItem(3, 0, new QTableWidgetItem("Address1"));
   ui->registers_list_->setItem(4, 0, new QTableWidgetItem("Address2"));
   ui->registers_list_->setItem(5, 0, new QTableWidgetItem("Jmp1"));
   ui->registers_list_->setItem(6, 0, new QTableWidgetItem("Jmp2"));
   ui->registers_list_->setItem(7, 0, new QTableWidgetItem("Instruction"));
   ui->registers_list_->setItem(8, 0, new QTableWidgetItem("instr_1"));
   ui->registers_list_->setItem(9, 0, new QTableWidgetItem("instr_2"));
   ui->registers_list_->setItem(10, 0, new QTableWidgetItem("Dest. Address"));
   ui->registers_list_->setItem(11, 0, new QTableWidgetItem("RAM Data"));
   ui->registers_list_->setItem(12, 0, new QTableWidgetItem("VP"));
   ui->registers_list_->setItem(13, 0, new QTableWidgetItem("HP"));
   ui->registers_list_->setItem(14, 0, new QTableWidgetItem("VE"));
   ui->registers_list_->setItem(15, 0, new QTableWidgetItem("HE"));
   ui->registers_list_->setItem(16, 0, new QTableWidgetItem("BFD"));

   for (int i = 0; i < 17; i++)
   {
      ui->registers_list_->setItem(i, 1, new QTableWidgetItem("--"));
   }

}

CopperDialog::~CopperDialog()
{
}

bool CopperDialog::event(QEvent *event)
{
   if ( event->type() == QEvent::User )
   {
      UpdateDebug();
      return true;
   }
   return QWidget::event(event);
}

void CopperDialog::SetEmulator(AmigaEmulation* emu_handler)
{
   emu_handler_ = emu_handler;
   emu_handler_->AddUpdateListener(this);
}

void CopperDialog::Break()
{
   // wait for the emulator to be in a stable state
}


void CopperDialog::Update()
{
   // Send "update" command
   QEvent* event = new QEvent(QEvent::User);
   QCoreApplication::postEvent( this, event);

}

void CopperDialog::UpdateDebug()
{
   // Update parent window
   this->parentWidget()->repaint();

   // Get CPU
   Copper* copper = emu_handler_->GetCopper();

   // Registers
   ui->registers_list_->item(0, 1)->setText(QString("%1").arg(copper->GetState(), 6, 16));
   ui->registers_list_->item(1, 1)->setText(QString("%1").arg(copper->GetCounter(), 6, 16));
   ui->registers_list_->item(2, 1)->setText(QString("%1").arg(copper->GetCon(), 6, 16));
   ui->registers_list_->item(3, 1)->setText(QString("%1").arg(copper->GetAddress1(), 6, 16));
   ui->registers_list_->item(4, 1)->setText(QString("%1").arg(copper->GetAddress2(), 6, 16));
   ui->registers_list_->item(5, 1)->setText(QString("%1").arg(copper->GetJmp1(), 6, 16));
   ui->registers_list_->item(6, 1)->setText(QString("%1").arg(copper->GetJmp2(), 6, 16));
   ui->registers_list_->item(7, 1)->setText(QString("%1").arg(copper->GetInstruction(), 6, 16));
   ui->registers_list_->item(8, 1)->setText(QString("%1").arg(copper->GetInstruction_1(), 6, 16));
   ui->registers_list_->item(9, 1)->setText(QString("%1").arg(copper->GetInstruction_2(), 6, 16));
   ui->registers_list_->item(10, 1)->setText(QString("%1").arg(copper->GetDestinationAddress(), 6, 16));
   ui->registers_list_->item(11, 1)->setText(QString("%1").arg(copper->GetRAMData(), 6, 16));
   ui->registers_list_->item(12, 1)->setText(QString("%1").arg(copper->GetVP(), 6, 16));
   ui->registers_list_->item(13, 1)->setText(QString("%1").arg(copper->GetVE(), 6, 16));
   ui->registers_list_->item(14, 1)->setText(QString("%1").arg(copper->GetHP(), 6, 16));
   ui->registers_list_->item(15, 1)->setText(QString("%1").arg(copper->GetHE(), 6, 16));
   ui->registers_list_->item(16, 1)->setText(QString("%1").arg(copper->GetBFD(), 6, 16));

   // Decode copperlist 1
   ui->copperlist_1->clear();
   // Decode copperlist 2
   ui->copperlist_2->clear();

   unsigned int counter_1 = copper->GetAddress1();
   unsigned int counter_2 = copper->GetAddress2();

   for (int i = 0; i < 32; i++)
   {
      unsigned short instr1 = emu_handler_->GetMotherboard()->Read16(counter_1);
      counter_1 += 2;
      unsigned short instr2 = emu_handler_->GetMotherboard()->Read16(counter_1);
      counter_1 += 2;

      ui->copperlist_1->addItem(Decode(instr1, instr2));

      instr1 = emu_handler_->GetMotherboard()->Read16(counter_2);
      counter_2 += 2;
      instr2 = emu_handler_->GetMotherboard()->Read16(counter_2);
      counter_2 += 2;

      ui->copperlist_2->addItem(Decode(instr1, instr2 ));
   }
}

QString CopperDialog::Decode(unsigned short instr1, unsigned short instr2)
{
   QString str = QString("%1").arg(instr1, 4, 16, QLatin1Char('0')).toUpper();
   str += " ";
   str += QString("%1").arg(instr2, 4, 16, QLatin1Char('0')).toUpper();
   if (instr1 & 1)
   {
      if (instr2 & 0x1)
      {
         // Skip
         str += " SKIP ";
      }
      else
      {
         // Wait
         str += " WAIT (";
      }
      //str += QString("#%1").arg((instr1 >> 1) & 0xFF, 2, 16, QLatin1Char('0')).toUpper();

   }
   else
   {
      // Move
      str += " MOVE ";
      str += QString("#%1").arg(instr2, 0, 16).toUpper();
      str += QString(", $%1").arg((instr1) & 0x1FF, 3, 16, QLatin1Char('0')).toUpper();

   }
   return str;
}
