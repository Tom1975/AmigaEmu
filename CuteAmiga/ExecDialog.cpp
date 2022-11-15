#include "ExecDialog.h"
#include "ui_ExecDialog.h"

#include <QDir>
#include <QMenuBar>

#define EXTRACT_LONG(c) \
   ((c[0]<<24)|(c[1]<<16)|(c[2]<<8)|(c[3]))

#define EXTRACT_WORD(c) \
   ((c[0]<<8)|(c[1]))

ExecDialog::ExecDialog(QWidget* parent) :
   QDialog(parent),
   ui(new Ui::ExecDialog)
{
   ui->setupUi(this);

   exec_base_item_ = new QTreeWidgetItem;
   ui->ExecWidget->addTopLevelItem(exec_base_item_);

   // Helper function population
   InitHelper();
}

void ExecDialog::InitHelper()
{
   // Exec
   library_helper_["exec.library"][-480] = "AbortIO";
   library_helper_["exec.library"][-432] = "AddDevice";
   library_helper_["exec.library"][-240] = "AddHead";
   library_helper_["exec.library"][-168] = "AddIntServer";
   library_helper_["exec.library"][-396] = "AddLibrary";
   library_helper_["exec.library"][-618] = "AddMemList";
   library_helper_["exec.library"][-354] = "AddPort";
   library_helper_["exec.library"][-486] = "AddResouree";
   library_helper_["exec.library"][-600] = "AddSemaphore";
   library_helper_["exec.library"][-246] = "AddTail";
   library_helper_["exec.library"][-282] = "AddTask";
   library_helper_["exec.library"][-108] = "Alert";
   library_helper_["exec.library"][-204] = "AllocAbs";
   library_helper_["exec.library"][-186] = "Allocate";
   library_helper_["exec.library"][-222] = "AllocEntry";
   library_helper_["exec.library"][-198] = "AllocMem";
   library_helper_["exec.library"][-330] = "AllocSignal";
   library_helper_["exec.library"][-342] = "AllocTrap";
   library_helper_["exec.library"][-576] = "AttemptSemaphore";
   library_helper_["exec.library"][-216] = "AvailMem";
   library_helper_["exec.library"][-180] = "Cause";
   library_helper_["exec.library"][-366] = "CBump";
   library_helper_["exec.library"][-468] = "CheckIO";
   library_helper_["exec.library"][-450] = "CloseDevice";
   library_helper_["exec.library"][-414] = "CloseLibrary";
   library_helper_["exec.library"][-624] = "CopyMem";
   library_helper_["exec.library"][-630] = "CopyMemQuick";
   library_helper_["exec.library"][-192] = "Deallocate";
   library_helper_["exec.library"][-114] = "Debug";
   library_helper_["exec.library"][-120] = "Disable";
   library_helper_["exec.library"][-465] = "DoIO";
   library_helper_["exec.library"][-126] = "Enable";
   library_helper_["exec.library"][-270] = "Enqueue";
   library_helper_["exec.library"][-276] = "FindName";
   library_helper_["exec.library"][-390] = "FindPort";
   library_helper_["exec.library"][-96]  = "FindResident";
   library_helper_["exec.library"][-612] = "FindSemaphore";
   library_helper_["exec.library"][-294] = "FindTask";
   library_helper_["exec.library"][-132] = "Forbid";
   library_helper_["exec.library"][-228] = "FreeEntry";
   library_helper_["exec.library"][-210] = "FreeMem";
   library_helper_["exec.library"][-336] = "FreeSignal";
   library_helper_["exec.library"][-348] = "FreeTrap";
   library_helper_["exec.library"][-528] = "GetCC";
   library_helper_["exec.library"][-372] = "GetMsg";
   library_helper_["exec.library"][-72]  = "InitCode";
   library_helper_["exec.library"][-102] = "InitResident";
   library_helper_["exec.library"][-78] = "InitStruct";
   library_helper_["exec.library"][-234] = "Insert";
   library_helper_["exec.library"][-90] = "MakeFunctions";
   library_helper_["exec.library"][-84] = "MakeLibrary";
   library_helper_["exec.library"][-564] = "ObtainSemaphore";
   library_helper_["exec.library"][-582] = "ObtainSemaphoreList";
   library_helper_["exec.library"][-408] = "OldOpenLibrary";
   library_helper_["exec.library"][-444] = "OpenDevice";
   library_helper_["exec.library"][-552] = "OpenLibrary";
   library_helper_["exec.library"][-498] = "OpenResource";
   library_helper_["exec.library"][-138] = "Permit";
   library_helper_["exec.library"][-540] = "Procure";
   library_helper_["exec.library"][-366] = "PutMsg";
   library_helper_["exec.library"][-522] = "RawDoFmt";
   library_helper_["exec.library"][-570] = "ReleaseSemaphore";
   library_helper_["exec.library"][-588] = "ReleaseSemaphoreList";
   library_helper_["exec.library"][-438] = "RemDevice";
   library_helper_["exec.library"][-258] = "RemHead";
   library_helper_["exec.library"][-174] = "RemIntServer";
   library_helper_["exec.library"][-402] = "RemLibrary";
   library_helper_["exec.library"][-252] = "Remove";
   library_helper_["exec.library"][-360] = "RemPort";
   library_helper_["exec.library"][-492] = "RemResource";
   library_helper_["exec.library"][-606] = "RemSemaphore";
   library_helper_["exec.library"][-264] = "RemTail";
   library_helper_["exec.library"][-288] = "RemTask";
   library_helper_["exec.library"][-378] = "ReplyMsg";
   library_helper_["exec.library"][-462] = "SendIO";
   library_helper_["exec.library"][-312] = "SetExcept";
   library_helper_["exec.library"][-420] = "SetFunction";
   library_helper_["exec.library"][-162] = "SetIntVector";
   library_helper_["exec.library"][-306] = "SetSignal";
   library_helper_["exec.library"][-144] = "SetSR";
   library_helper_["exec.library"][-300] = "SetTaskPri";
   library_helper_["exec.library"][-324] = "Signal";
   library_helper_["exec.library"][-612] = "SumKickData";
   library_helper_["exec.library"][-426] = "SumLibrary";
   library_helper_["exec.library"][-150] = "SuperState";
   library_helper_["exec.library"][-534] = "TypeOfMem";
   library_helper_["exec.library"][-156] = "UserState";
   library_helper_["exec.library"][-546] = "Vacate";
   library_helper_["exec.library"][-318] = "Wait";
   library_helper_["exec.library"][-474] = "WaitIO";
   library_helper_["exec.library"][-384] = "WaitPort";


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

      item_task_list = new QTreeWidgetItem;
      item_task_list->setText(0, QString("LIBRARIES"));

      // Check memory : From top node, 
      unsigned long node = EXTRACT_LONG((&ram[0x676 + 0x17A]));
      while (node != 0)
      {
         // 
         // offset 0x0A : name
         QTreeWidgetItem* item = new QTreeWidgetItem;
         unsigned long lib_name_ptr = EXTRACT_LONG((&ram[node + 0x0A]));
         char* lib_name = (lib_name_ptr >= 0xFC0000) ? (char*)&rom[lib_name_ptr & 0x3FFFF] : (char*)&ram[(lib_name_ptr) & 0x7FFFF];
         item->setText(0, QString(lib_name));
         QTreeWidgetItem* base_address_item = new QTreeWidgetItem;
         base_address_item->setText(0, QString("Base address : %1").arg(node, 6, 16));

         // Add all functions 
         unsigned long vector_address = node - 6;
         unsigned short jmp = EXTRACT_WORD((&ram[vector_address]));
         while (jmp == 0x4EF9 && vector_address > 6)
         {
            QTreeWidgetItem* func = new QTreeWidgetItem;
            unsigned long func_addr = EXTRACT_LONG((&ram[vector_address +2]));

            short index = vector_address - node;
            if ( (library_helper_.find(lib_name) != library_helper_ .end()) && (library_helper_[lib_name].find(index) != library_helper_[lib_name].end()))
            {
               std::string func_name = library_helper_[lib_name][index];
               func->setText(0, QString("-$%1 (%3) : %2").arg((short)(node - vector_address), 4, 16).arg(func_addr, 6, 16).arg(func_name.c_str()));
            }
            else
            {
               func->setText(0, QString("-$%1 : %2").arg((short)(node - vector_address), 4, 16).arg(func_addr, 6, 16));
            }
            

            base_address_item->addChild(func);

            vector_address -= 6;
            jmp = EXTRACT_WORD((&ram[vector_address]));

         }
         

         // Add Signals
         item->addChild(base_address_item);
         node = EXTRACT_LONG((&ram[node]));

         item_task_list->addChild(item);
      }
      ui->ExecWidget->addTopLevelItem(item_task_list);
      list_items_.push_back(item_task_list);

      // Add Interrupt vectors
      item_task_list = new QTreeWidgetItem;
      item_task_list->setText(0, QString("Interrupt Vectors"));

      // Check memory : From top node, 
      for (int i = 1; i < 8; i++)
      {
         unsigned long intVector= EXTRACT_LONG((&ram[(24 + i) * 4]));

         QTreeWidgetItem* item = new QTreeWidgetItem;
         item->setText(0, QString("Int %1").arg(i));
         QTreeWidgetItem* base_address_item = new QTreeWidgetItem;
         base_address_item->setText(0, QString("Base address : %1").arg(intVector, 6, 16));
         // Add Signals
         item->addChild(base_address_item);
         item_task_list->addChild(item);
      }

      ui->ExecWidget->addTopLevelItem(item_task_list);
      list_items_.push_back(item_task_list);

   }

}
