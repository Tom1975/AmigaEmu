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

void ExecDialog::UpdateList(unsigned long list_adress, QTreeWidgetItem * root_item, void(*list_handler)(unsigned long, QTreeWidgetItem*, std::vector<QTreeWidgetItem*> *))
{
   unsigned char * ram = emu_handler_->GetMotherboard()->GetBus()->GetRam();
   unsigned char * rom = emu_handler_->GetMotherboard()->GetRom();

   unsigned long current_list_node = EXTRACT_LONG((&ram[list_adress]));
   // From head to tail...
   while (current_list_node != 0  )
   {
      QTreeWidgetItem* item = new QTreeWidgetItem;
      // Add a device
      unsigned long name_address = EXTRACT_LONG((&ram[current_list_node + 10]));
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
         list_items_.push_back(base_address_item);
         root_item->addChild(item);
         list_items_.push_back(item);
         // Add specific informations

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

      QTreeWidgetItem *item_task_list = new QTreeWidgetItem;
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
