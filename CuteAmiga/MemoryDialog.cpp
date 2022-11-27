#include "MemoryDialog.h"
#include "ui_MemoryDialog.h"

#include <QDir>
#include <QMenuBar>
#include <QDebug>

MemoryDialog::MemoryDialog(QWidget *parent) :
   QDialog(parent),
   ui(new Ui::MemoryDialog),
   start_addr_(0)
{
   ui->setupUi(this);
   ui->memoryWidget->setContextMenuPolicy(Qt::CustomContextMenu);
   
}

MemoryDialog::~MemoryDialog()
{
}

bool MemoryDialog::event(QEvent *event)
{
   if ( event->type() == QEvent::User )
   {
      UpdateDebug();
      return true;
   }
   return QWidget::event(event);
}

void MemoryDialog::SetEmulator(AmigaEmulation* emu_handler)
{
   emu_handler_ = emu_handler;
   emu_handler_->AddUpdateListener(this);
}

void MemoryDialog::Break()
{
   // wait for the emulator to be in a stable state
}

void MemoryDialog::Update()
{
   // Send "update" command
   QEvent* event = new QEvent(QEvent::User);
   QCoreApplication::postEvent( this, event);

}

void MemoryDialog::on_address_returnPressed()
{
   on_setAddress_clicked();
}

void MemoryDialog::on_setAddress_clicked()
{
   QString text = ui->address->text();
   start_addr_ = (unsigned int)strtoul(text.toUtf8().constData(), NULL, 16);
   UpdateDebug();
}

void MemoryDialog::on_pattern_returnPressed()
{
   on_searchAddress_clicked();
}

void MemoryDialog::on_searchAddress_clicked()
{
   // Get CPU, memory
   M68k* m68k = emu_handler_->GetCpu();
   unsigned char* mem = emu_handler_->GetMotherboard()->GetBus()->GetRam();

   QString text = ui->pattern->text();
   text = text.toUpper();

   // Remove anything but 0-9 A-F
   QByteArray data_pattern = QByteArray::fromHex(text.toUtf8());
   const char* search_buffer = data_pattern.data();
   size_t size = data_pattern.size();

   // Look into memory from current address + 1
   bool finished = false;
   bool roll = false;
   unsigned int current_address = start_addr_ + 1;
   while (!finished)
   {
      finished = (memcmp(&mem[current_address], search_buffer, size) == 0);
      if (!finished)
      {
         current_address++;
         if (roll)
         {
            if (current_address >= start_addr_)
            {
               finished = true;
            }
         }
         else
         {
            if (current_address >= 512 * 1024)
            {
               roll = true;
               current_address = 0;
            }
         }
            
      }
      else
      {
         start_addr_ = current_address;
      }
   }
   // until found or all memory looked up
   // found ? display
   // 
   UpdateDebug();
}


void MemoryDialog::UpdateDebug()
{
   // Update parent window
   this->parentWidget()->repaint();

   // Get CPU
   M68k* m68k = emu_handler_->GetCpu();

   // Get Memory
   unsigned char* mem = emu_handler_->GetMotherboard()->GetBus()->GetRam();

   std::string str_asm;
   ui->memoryWidget->clear();

   char addr[16];
   char hexa_line[16 * 3 + 1];
   char ascii_line[16 + 1];

   unsigned int current_address = start_addr_;
   for (int i = 0; i < 32; i++)
   {
      sprintf(addr, "%8.8X: ", current_address);
      str_asm = addr;

      hexa_line[0] = '\0';
      ascii_line[0] = '\0';
      if (current_address < 512 * 1024)
      {
         for (int j = 0; j < 16; j++)
         {
            // Hex line
            sprintf(addr, "%2.2X ", mem[current_address]);
            strcat(hexa_line, addr);
            
            // ASCII line
            unsigned char b = mem[current_address];
            if (b >= 0x20 && b <= 126)
            {
               sprintf(addr,"%c", b);
            }
            else
            {
               sprintf (addr, ".");
            }
            strcat(ascii_line, addr);

            current_address++;

         }
      }
      
      str_asm += hexa_line;
      str_asm += "   ";
      str_asm += ascii_line;

      qDebug() << str_asm.c_str();

      ui->memoryWidget->addItem(str_asm.c_str());
   }


}
