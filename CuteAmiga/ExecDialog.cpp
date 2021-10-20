#include "ExecDialog.h"
#include "ui_ExecDialog.h"

#include <QDir>
#include <QMenuBar>

#define EXTRACT_LONG(c) \
   ((c[0]<<24)|(c[1]<<16)|(c[2]<<8)|(c[3]))

#define EXTRACT_WORD(c) \
   ((c[0]<<8)|(c[1]))

ExecDialog::ExecDialog(QWidget *parent) :
   QDialog(parent),
   ui(new Ui::ExecDialog)
{
   ui->setupUi(this);
   
   exec_base_item_ = new QTreeWidgetItem;
   ui->ExecWidget->addTopLevelItem(exec_base_item_);

}

ExecDialog::~ExecDialog()
{
   delete exec_base_item_;
}

bool ExecDialog::event(QEvent *event)
{
   if ( event->type() == QEvent::User )
   {
      UpdateDebug();
      return true;
   }
   return QWidget::event(event);
}

void ExecDialog::SetEmulator(AmigaEmulation* emu_handler)
{
   emu_handler_ = emu_handler;
   emu_handler_->AddUpdateListener(this);

}

void ExecDialog::Break()
{
   // wait for the emulator to be in a stable state
}


void ExecDialog::Update()
{
   // Send "update" command
   QEvent* event = new QEvent(QEvent::User);
   QCoreApplication::postEvent( this, event);

}

void ExecDialog::UpdateDebug()
{
   // Update parent window
   this->parentWidget()->repaint();

   unsigned char * ram = emu_handler_->GetMotherboard()->GetBus()->GetRam();
   unsigned char * rom = emu_handler_->GetMotherboard()->GetRom();

   // Check execbase
   unsigned int exec_base = EXTRACT_LONG ( (&ram[4]) );

   // Exec base address
   unsigned int kickstart_ver, kickstart_subver;

   exec_base_item_->setText(0, QString("ExecBase : %3").arg(exec_base, 6, 16));

   list_items_.clear();

   if (exec_base < 1024 * 512) // TODO : handle this with real memory size...
   {
      // Add Devices
      QTreeWidgetItem item_device_list;
      item_device_list.setText(0, QString("DEVICES"));
      unsigned long device_list = exec_base + 0x15E;

      unsigned long current_device_node = EXTRACT_LONG((&ram[device_list]));

      // From head to tail...
      while (current_device_node != 0)
      {
         QTreeWidgetItem item_device;
         // Add a device
         unsigned long device_name_address = EXTRACT_LONG((&ram[current_device_node + 10]));
         char* device_name = (char*)&rom[device_name_address &0x3FFFF];
         item_device.setText(1, QString(device_name));
         item_device_list.addChild(&item_device);
         list_items_.push_back(item_device);


         current_device_node = EXTRACT_LONG((&ram[current_device_node]));

      }

      ui->ExecWidget->addTopLevelItem(&item_device_list);


      list_items_.push_back(item_device_list);
   }

}
