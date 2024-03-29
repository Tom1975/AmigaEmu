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

void ExecDialog::UpdateTask(unsigned long task_adress, QTreeWidgetItem* base_item, std::vector<QTreeWidgetItem*> *list_items)
{
   QTreeWidgetItem* item = new QTreeWidgetItem;
   // Add Signals

   base_item->addChild(item);
   list_items->push_back(item);
}

void ExecDialog::AddTask(unsigned long task_adress_ptr, QTreeWidgetItem * root_item, void(*list_handler)(unsigned long, QTreeWidgetItem*, std::vector<QTreeWidgetItem*> *))
{
   // Address
   unsigned char * ram = emu_handler_->GetMotherboard()->GetBus()->GetRam();
   unsigned char * rom = emu_handler_->GetMotherboard()->GetRom();

   unsigned long task_adress = EXTRACT_LONG((&ram[task_adress_ptr]));

   if (task_adress != 0 && task_adress < 0x7FFFF)
   {
      QTreeWidgetItem* item = new QTreeWidgetItem;
      // Add a device
      unsigned long name_address = EXTRACT_LONG((&ram[(task_adress + 10) & 0x7FFFF]));
      unsigned long current_sp = EXTRACT_LONG((&ram[(task_adress + 54) & 0x7FFFF]));
      char* name = (name_address >= 0xFC0000) ? (char*)&rom[name_address & 0x3FFFF] : (char*)&ram[(name_address) & 0x7FFFF];

      if (strlen(name) > 0)
      {
         item->setText(0, QString(name));
         // Add base address
         QTreeWidgetItem* base_address_item = new QTreeWidgetItem;
         base_address_item->setText(0, QString("Base address : %1").arg(task_adress & 0x7FFFF, 6, 16));
         if (list_handler != nullptr)
         {
            list_handler(task_adress & 0x7FFFF, item, &list_items_);
         }
         item->addChild(base_address_item);

         QTreeWidgetItem* sp_address_item = new QTreeWidgetItem;
         sp_address_item->setText(0, QString("Stack sp : %1").arg(current_sp, 6, 16));
         item->addChild(sp_address_item);

         unsigned long sig_wait = EXTRACT_LONG((&ram[(task_adress + 22) & 0x7FFFF]));
         QTreeWidgetItem* sp_signal_awaited = new QTreeWidgetItem;
         sp_signal_awaited->setText(0, QString("SIGNAL_WAIT : %1").arg(sig_wait, 6, 16));
         item->addChild(sp_signal_awaited);

         unsigned long sig_recv = EXTRACT_LONG((&ram[(task_adress + 30) & 0x7FFFF]));
         QTreeWidgetItem* sp_signal_received = new QTreeWidgetItem;
         sp_signal_received->setText(0, QString("SIGNAL_RECV : %1").arg(sig_recv, 6, 16));
         item->addChild(sp_signal_received);

         list_items_.push_back(base_address_item);
         root_item->addChild(item);
         list_items_.push_back(item);
         // Add specific informations

      }
   }
}

void ExecDialog::UpdateList(unsigned long list_adress, QTreeWidgetItem * root_item, void(*list_handler)(unsigned long, QTreeWidgetItem*, std::vector<QTreeWidgetItem*> *))
{
   unsigned char * ram = emu_handler_->GetMotherboard()->GetBus()->GetRam();
   unsigned char * rom = emu_handler_->GetMotherboard()->GetRom();

   unsigned long current_list_node = EXTRACT_LONG((&ram[list_adress]));
   // From head to tail...
   while (current_list_node != 0  && current_list_node < 0x7FFFF)
   {
      QTreeWidgetItem* item = new QTreeWidgetItem;
      // Add a device
      unsigned long name_address = EXTRACT_LONG((&ram[current_list_node + 10]));
      unsigned long current_sp = EXTRACT_LONG((&ram[current_list_node + 54]));
      char* name = (name_address >= 0xFC0000) ? (char*)&rom[name_address & 0x3FFFF] : (char*)&ram[name_address];
      if (strlen(name) > 0)
      {
         item->setText(0, QString(name));
         // Add base address
         QTreeWidgetItem* base_address_item = new QTreeWidgetItem;
         base_address_item->setText(0, QString("Base address : %1").arg(current_list_node, 6, 16));
         if (list_handler != nullptr)
         {
            list_handler(current_list_node, item, &list_items_);
         }
         
         item->addChild(base_address_item);

         /*QTreeWidgetItem* pc_address_item = new QTreeWidgetItem;
         pc_address_item->setText(0, QString("Current PC : %1").arg(current_pc, 6, 16));
         item->addChild(pc_address_item);
         */
         QTreeWidgetItem* sp_address_item = new QTreeWidgetItem;
         sp_address_item->setText(0, QString("Stack sp : %1").arg(current_sp, 6, 16));
         item->addChild(sp_address_item);

         list_items_.push_back(base_address_item);
         root_item->addChild(item);
         list_items_.push_back(item);
         
         // Add specific informations
         unsigned long sig_wait = EXTRACT_LONG((&ram[current_list_node + 22]));
         QTreeWidgetItem* sp_signal_awaited = new QTreeWidgetItem;
         sp_signal_awaited->setText(0, QString("SIGNAL_WAIT : %1").arg(sig_wait, 6, 16));
         item->addChild(sp_signal_awaited);

         unsigned long sig_recv = EXTRACT_LONG((&ram[current_list_node + 32]));
         QTreeWidgetItem* sp_signal_received = new QTreeWidgetItem;
         sp_signal_received->setText(0, QString("SIGNAL_RECV : %1").arg(sig_recv, 6, 16));
         item->addChild(sp_signal_received);
      }
      current_list_node = EXTRACT_LONG((&ram[current_list_node]));

   }

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

   for (auto it : list_items_)
   {
      delete it;
   }
   list_items_.clear();

   if (exec_base < 1024 * 512) // TODO : handle this with real memory size...
   {
      // Add Devices
      QTreeWidgetItem *item_memory_list = new QTreeWidgetItem;
      item_memory_list = new QTreeWidgetItem;
      item_memory_list->setText(0, QString("MEMORY LIST"));
      UpdateList(exec_base + 0x142, item_memory_list);
      ui->ExecWidget->addTopLevelItem(item_memory_list);
      list_items_.push_back(item_memory_list);

      QTreeWidgetItem *item_resource_list = new QTreeWidgetItem;
      item_resource_list->setText(0, QString("RESOURCE"));
      UpdateList(exec_base + 0x150, item_resource_list);
      ui->ExecWidget->addTopLevelItem(item_resource_list);
      list_items_.push_back(item_resource_list);

      QTreeWidgetItem *item_device_list = new QTreeWidgetItem;
      item_device_list->setText(0, QString("DEVICES"));
      UpdateList(exec_base + 0x15E, item_device_list);
      ui->ExecWidget->addTopLevelItem(item_device_list);
      list_items_.push_back(item_device_list);

      // Current task
      QTreeWidgetItem *item_task_list = new QTreeWidgetItem;
      item_task_list->setText(0, QString("CURRENT_TASK"));
      AddTask(exec_base + 0x114, item_task_list, UpdateTask);
      ui->ExecWidget->addTopLevelItem(item_task_list);
      list_items_.push_back(item_task_list);

      item_task_list = new QTreeWidgetItem;
      item_task_list->setText(0, QString("TASK_READY"));
      UpdateList(exec_base + 0x196, item_task_list, UpdateTask);
      ui->ExecWidget->addTopLevelItem(item_task_list);
      list_items_.push_back(item_task_list);

      item_task_list = new QTreeWidgetItem;
      item_task_list->setText(0, QString("TASK_WAIT"));
      UpdateList(exec_base + 0x1A4, item_task_list, UpdateTask);
      ui->ExecWidget->addTopLevelItem(item_task_list);
      list_items_.push_back(item_task_list);

   }

}
