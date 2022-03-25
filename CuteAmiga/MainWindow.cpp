#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDir>
#include <QSettings>
#include <QFile>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
   QMainWindow(parent),
   debug_(this),
   memory_(this),
   copper_(this),
   exec_(this),
   bitplane_(this),
   ui(new Ui::MainWindow)
{
   ui->setupUi(this);

   ui->display_->SetDragnDropTarget(this);
   emu_handler_ = new AmigaEmulation(ui->display_);

   // Menu connection
   connect(ui->actionReset, &QAction::triggered, emu_handler_, &AmigaEmulation::Reset);
   connect(ui->actionCPU, &QAction::triggered, this, &MainWindow::Break);
   connect(ui->actionMemory, &QAction::triggered, this, &MainWindow::Memory);
   connect(ui->actionCopper, &QAction::triggered, this, &MainWindow::Copper);
   connect(ui->actionExec, &QAction::triggered, this, &MainWindow::Exec);
   connect(ui->actionBitplane, &QAction::triggered, this, &MainWindow::Bitplane);
   connect(ui->actionInsert_disk_df0, &QAction::triggered, this, &MainWindow::InsertDisk);

   connect(ui->display_, SIGNAL(Update()),
      this, SLOT(Update()), Qt::QueuedConnection);

   // Create debug window
   debug_.SetEmulator(emu_handler_);
   memory_.SetEmulator(emu_handler_);
   copper_.SetEmulator(emu_handler_);
   exec_.SetEmulator(emu_handler_);
   bitplane_.SetEmulator(emu_handler_);

   // status bar
   led_on_ = new QPixmap(":/Images/led_on.png");
   led_off_ = new QPixmap(":/Images/led_off.png");
   drive_led_on_ = new QPixmap(":/Images/drive_led_on.png");
   drive_led_off_ = new QPixmap(":/Images/drive_led_off.png");
   iconled_ = new QLabel;
   iconled_->setPixmap(*led_off_);

   statusBar()->addWidget(iconled_);

   for (int i = 0; i < 4; i++)
   {
      icondrive_[i].setText("DFx");
      icondrive_[i].setPixmap(*drive_led_off_);
      statusBar()->addWidget(&icondrive_[i]);
   }


}

MainWindow::~MainWindow()
{
   delete emu_handler_;
   delete ui;
}


/////////////////////////////////////////////////////////////////////////////
// initialisation
/////////////////////////////////////////////////////////////////////////////

void MainWindow::InitEmulation()
{   // Load configuration
   LoadConfig();

   // Link various windows to emulator parts

   // Start the emulator thread
   std::string path = QDir::currentPath().toStdString();
   emu_handler_->Start();
}

/////////////////////////////////////////////////////////////////////////////
// Configuration load / save
/////////////////////////////////////////////////////////////////////////////
void MainWindow::SaveConfig()
{
   // Save current config
   QSettings settings(QString("config/config.ini"), QSettings::IniFormat);

   // Breakpoints
   auto pb_handler = emu_handler_->GetBreakpointHandler();
   unsigned int nb_bp = pb_handler->GetBreakpointNumber();
   
   unsigned int nb_breapkoints_saved = 0;
   for (unsigned int i = 0; i < nb_bp; i++)
   {
      IBreakpointItem* bp = pb_handler->GetBreakpoint(i);
      const char* str_def = bp->GetString();
      if (str_def != nullptr)
      {
         QString key = "breakpoints/" + QString::number(i);
         settings.setValue(key, str_def);
         nb_breapkoints_saved++;
      }
   }
   settings.setValue("breakpoints/number", nb_breapkoints_saved);

   // Disk status (df0-df3)
   if (df0_path_.length() > 0)
   {
      settings.setValue("disk/df0_path_file", df0_path_);
   }
}

void MainWindow::LoadConfig()
{
   // Save current config
   auto pb_handler = emu_handler_->GetBreakpointHandler();
   QSettings settings(QString("config/config.ini"), QSettings::IniFormat);
   
   int nb_breapkoints = settings.value("breakpoints/number", 0).toInt();
   
   // Breakpoints
   for (int i = 0; i < nb_breapkoints; i++)
   {
      QString key = "breakpoints/" + QString::number(i);
      std::string str = settings.value(key, 0).toString().toStdString();

      pb_handler->CreateBreakpoint(str.c_str());
   }
   
   // Disk status (df0-df3)
   Motherboard* mb = emu_handler_->GetMotherboard();
   if (df0_path_.length() > 0)
   {
      df0_path_ = settings.value("disk/df0_path_file", 0).toString();
      Disk* disk = new Disk(df0_path_.toStdString());
      if (disk->IsValid())
      {
         mb->GetDiskController()->GetDiskDrive(0)->InsertDisk(disk);
      }
   }
}

/////////////////////////////////////////////////////////////////////////////
// Action handlers
/////////////////////////////////////////////////////////////////////////////
void MainWindow::closeEvent(QCloseEvent *event)
{
   SaveConfig();
}

void MainWindow::Break()
{
   emu_handler_->Break();
   debug_.Update();
   debug_.show();
   memory_.Update();
   copper_.Update();
   exec_.Update();
   bitplane_.Update();
}

void MainWindow::Copper ()
{
   copper_.Update();
   copper_.show();
}

void MainWindow::Memory()
{
   memory_.Update();
   memory_.show();
}

void MainWindow::Exec()
{
   exec_.Update();
   exec_.show();
}

void MainWindow::Bitplane()
{
   bitplane_.Update();
   bitplane_.show();
}

void MainWindow::InsertDisk()
{
   // Select file to open
   QString filename = QFileDialog::getOpenFileName(this, "Choose File");

   if (filename.isEmpty())
      return;

   // Open it.
   Motherboard* mb = emu_handler_->GetMotherboard();

   Disk* disk = new Disk (filename.toStdString());
   if (disk->IsValid())
   {
      mb->GetDiskController()->GetDiskDrive(0)->InsertDisk(disk);
      // Update df0 config
      df0_path_ = filename;
   }
   else
   {
      df0_path_ = "";
      delete disk;
   }
}

/////////////////////////////////////////////////////////////////////////////
// Draw & update
/////////////////////////////////////////////////////////////////////////////
void MainWindow::Update()
{
   ui->display_->update();
   
   Motherboard* mb = emu_handler_->GetMotherboard();

   // Main power Led
   // Read CIA-A, PA1
   iconled_->setPixmap(mb->GetLed()?*led_on_ :*led_off_);

   // Disk drive status
   for (int i = 0; i < 4; i++)
   {
      icondrive_[i].setPixmap( mb->GetDriveLed(i)?*drive_led_on_:*drive_led_off_);
   }

}

/////////////////////////////////////////////////////////////////////////////
// Drag'n'Drop
/////////////////////////////////////////////////////////////////////////////
void MainWindow::OpenFiles(const QStringList& pathList)
{
   for (int i = 0; i < pathList.size() && i < 4; ++i)
   {
      // Load first 4 files ( df0 to df3 )
      Motherboard* mb = emu_handler_->GetMotherboard();
      Disk* disk = new Disk(pathList[i].toStdString());
      if (disk->IsValid())
      {
         mb->GetDiskController()->GetDiskDrive(0)->InsertDisk(disk);
         // Update df0 config
         df0_path_ = pathList[i];
      }
      else
      {
         df0_path_ = "";
         delete disk;
      }
   }
}
