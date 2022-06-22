#include "DiskDialog.h"
#include "ui_DiskDialog.h"

DiskDialog::DiskDialog(QWidget *parent) :
   QDialog(parent),
   ui(new Ui::DiskDialog)
{
   ui->setupUi(this);
}

DiskDialog::~DiskDialog()
{
}

bool DiskDialog::event(QEvent *event)
{
   if ( event->type() == QEvent::User )
   {
      UpdateDebug();
      return true;
   }
   return QWidget::event(event);
}

void DiskDialog::SetEmulator(AmigaEmulation* emu_handler)
{
   emu_handler_ = emu_handler;
   emu_handler_->AddUpdateListener(this);

}

void DiskDialog::Update()
{
   // Send "update" command
   QEvent* event = new QEvent(QEvent::User);
   QCoreApplication::postEvent( this, event);
}

void DiskDialog::UpdateDebug()
{
   // update display - DF0 only !
   for (int i = 0; i < 1; i++)
   {
      DiskDrive* drive = emu_handler_->GetMotherboard()->GetDiskController()->GetDiskDrive(i);
      ui->track->display((int)drive->GetTrack());
      ui->side->display((int)drive->GetSide());
      ui->head->display( (int)drive->GetHeadPosition());
   }
   // Update parent window
   this->parentWidget()->repaint();
}
