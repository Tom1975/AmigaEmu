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
      bitplane_struct *bp_struct = new bitplane_struct;
      unsigned char * ram = emu_handler_->GetMotherboard()->GetBus()->GetRam();

      // Get address, width, height
      QString text = ui->rasterport_address_->text();
      if (text.length() > 0)
      {
         bool ok;
         unsigned int addr = text.toInt(&ok, 16);

         if (ok)
         {
            QListWidgetItem *item = new QListWidgetItem;
            item->setText(QString("Bitmap #%1").arg(text.toUInt() + 4, 6, 16));

            bp_struct->byte_per_row = EXTRACT_WORD((&ram[addr]));
            bp_struct->rows = EXTRACT_WORD((&ram[addr + 2]));
            bp_struct->depth = ram[addr + 5];
            for (int i = 0; i < bp_struct->depth; i++)
            {
               // Get address
               bp_struct->bitplanes[i] = EXTRACT_LONG((&ram[addr + 8 + i * 4]));
            }

            //      item->setData( Qt::UserRole, bp_struct);
            item->setData(Qt::UserRole, (int)bp_struct);
            list_items_.push_back(item);
            ui->bitmap_list_->addItem(item);
         }
      }
   }
}

void BitplaneDialog::Update()
{
   // Send "update" command
   QEvent* event = new QEvent(QEvent::User);
   QCoreApplication::postEvent( this, event);

}

void BitplaneDialog::UpdateBitplaneOverview()
{
   // Depending on the biplane selected, display it.
   /*QList<QListWidgetItem *> list_item = ui->bitmap_list_->selectedItems();
   if (list_item.isEmpty())
      return;

   bitplane_struct bp_struct = list_item[0]->data(Qt::UserRole).value<bitplane_struct >();
   */
   if (list_items_.empty())
      return;
   
   QListWidgetItem* item = list_items_[0];
   bitplane_struct* bp_struct = (bitplane_struct*)item->data(Qt::UserRole).toUInt();

   unsigned int addr_base = bp_struct->bitplanes[0];
   unsigned char * ram = emu_handler_->GetMotherboard()->GetBus()->GetRam();
   for (int i = 0; i < bp_struct->rows; i++)
   {
      unsigned int * ptr = ui->display_->GetFrameBuffer(i);
      for (int x = 0; x < bp_struct->byte_per_row; x++)
      {
         unsigned char current_byte = ram[addr_base++];
         // add pixel
         for (int b = 0; b < 8; b++)
         {
            ptr[x * 8 + b] = ((current_byte >> b) & 0x1) ? 0: 0xFFFFFF;
         }
      }
   }
   
}

void BitplaneDialog::UpdateDebug()
{

   // update ui->display_
   // Get biplane address
   /*Bitplanes* bp = emu_handler_->GetMotherboard()->GetBitplanes();
   unsigned int nb_bitplanes_ = ((bp->bplcon0_ & 0x7000) >> 12);

   for (auto it : list_items_)
   {
      //delete it;
   }
   list_items_.clear();

   ui->bitmap_list_->clear();

   bitplane_struct bp_struct;
   QListWidgetItem *item = new QListWidgetItem;

   item->setText( QString("CURRENT BITPLANE"));
   bp_struct.depth = nb_bitplanes_;

   // byte per row ?
   // rows ?

   for (int i = 0; i < nb_bitplanes_; i++)
   {
      // Get address
      bp_struct.bitplanes[i] = bp->bplxpt_[i];
   }
   */
   // Get width / stride / height
   // update display
   UpdateBitplaneOverview();
}