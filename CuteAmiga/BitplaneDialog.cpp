#include "BitplaneDialog.h"
#include "ui_BitplaneDialog.h"

#include <QDir>
#include <QMenuBar>

#define EXTRACT_LONG(c) \
   ((c[0]<<24)|(c[1]<<16)|(c[2]<<8)|(c[3]))

#define EXTRACT_WORD(c) \
   ((c[0]<<8)|(c[1]))

BitplaneDialog::BitplaneDialog(QWidget *parent) :
   QDialog(parent),
   ui(new Ui::BitplaneDialog)
{
   ui->setupUi(this);
}

BitplaneDialog::~BitplaneDialog()
{
   // Empty list
   for (auto it : list_items_)
   {
      //delete (bitplane_struct*)it->data(Qt::UserRole).toUInt();
      delete it;
   }
}

bool BitplaneDialog::event(QEvent *event)
{
   if ( event->type() == QEvent::User )
   {
      UpdateDebug();
      return true;
   }
   return QWidget::event(event);
}

void BitplaneDialog::SetEmulator(AmigaEmulation* emu_handler)
{
   emu_handler_ = emu_handler;
   emu_handler_->AddUpdateListener(this);

}

// Ajout de rasterport
void BitplaneDialog::on_add_rasterport__returnPressed()
{
   on_add_rasterport__clicked();
}

void BitplaneDialog::on_add_rasterport__clicked()
{
   // Add a bitmap 
   QString text = ui->rasterport_address_->text();
   if (text.length() > 0)
   {
      // Get address, width, height
      QString text = ui->rasterport_address_->text();
      if (text.length() > 0)
      {
         bool ok;
         unsigned int addr = text.toInt(&ok, 16);

         if (ok)
         {
            QListWidgetItem *item = new QListWidgetItem;
            item->setText(QString("Bitmap #%1").arg(addr, 6, 16));
            item->setData(Qt::UserRole, addr);
            list_items_.push_back(item);
            ui->bitmap_list_->addItem(item);

            // Select this item, and update the bitplane list
            ui->bitmap_list_->setCurrentItem(item, QItemSelectionModel::Current);
            UpdateBitplaneList();
         }
      }
   }
}

void BitplaneDialog::UpdateBitmapStructure()
{
   QList<QListWidgetItem*> item_list = ui->bitmap_list_->selectedItems();
   if (item_list.size() > 0)
   {
      unsigned int addr = item_list[0]->data(Qt::UserRole).toUInt();
      unsigned char * ram = emu_handler_->GetMotherboard()->GetBus()->GetRam();

      // get address from selected bitmap
      current_bitmap_.byte_per_row = EXTRACT_WORD((&ram[addr]));
      current_bitmap_.rows = EXTRACT_WORD((&ram[addr + 2]));
      current_bitmap_.depth = ram[addr + 5];
      for (int i = 0; i < current_bitmap_.depth && i < 8; i++)
      {
         // Get address
         current_bitmap_.bitplanes[i] = EXTRACT_LONG((&ram[addr + 8 + i * 4]));
      }
   }
   else
   {
      memset(&current_bitmap_, 0, sizeof(current_bitmap_));
   }
}

// Select a rasterport
void BitplaneDialog::on_bitmap_list__itemSelectionChanged()
{
   UpdateBitplaneList();
}
// Select a bitplane

void BitplaneDialog::on_bitplanes_list__itemSelectionChanged()
{
   UpdateBitplaneOverview();
}

void BitplaneDialog::Update()
{
   // Send "update" command
   QEvent* event = new QEvent(QEvent::User);
   QCoreApplication::postEvent( this, event);

}

// Update
void BitplaneDialog::UpdateBitplaneList()
{
   ui->bitplanes_list_->clear();

   UpdateBitmapStructure();
   for (int i = 0; i < current_bitmap_.depth && i < 8; i++)
   {
      QString label = QString("Bitplane #%1").arg(i);
      ui->bitplanes_list_->addItem(label);
   }
   UpdateBitplaneOverview();
}

void BitplaneDialog::UpdateBitplaneOverview()
{
   // Get current bitplane from bitplane list
   QList<QListWidgetItem*> item_list = ui->bitplanes_list_->selectedItems();
   
   if (item_list.size() == 0 )
      return;

   unsigned int index = ui->bitplanes_list_->currentRow();
   bitplane_struct* bp_struct = &current_bitmap_;

   unsigned int addr_base = bp_struct->bitplanes[index];
   unsigned char * ram = emu_handler_->GetMotherboard()->GetBus()->GetRam();
   for (int i = 0; i < bp_struct->rows && i < 300; i++)
   {
      unsigned int * ptr = ui->display_->GetFrameBuffer(i);
      for (int x = 0; x < bp_struct->byte_per_row && x < 100; x+=2)
      {
         unsigned short current_word = (ram[addr_base]<<8) | ram[addr_base+1];
         if (current_word != 0)
         {
            int dbg = 1;
         }
         addr_base += 2;
         // add pixel
         for (int b = 0; b < 16; b++)
         {
            ptr[x * 8 + b] = ((current_word >> (15-b)) & 0x1) ? 0: 0xFFFFFF;
         }
      }
   }  
}

void BitplaneDialog::UpdateDebug()
{
   // update display
   UpdateBitplaneList();
}