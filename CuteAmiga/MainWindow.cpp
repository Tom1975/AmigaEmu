#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDir>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent) :
   QMainWindow(parent),
   debug_(this),
   memory_(this),
   copper_(this),
   exec_(this),
   ui(new Ui::MainWindow)
{
   ui->setupUi(this);

   emu_handler_ = new AmigaEmulation(ui->display_);

   // Menu connection
   connect(ui->actionReset, &QAction::triggered, emu_handler_, &AmigaEmulation::Reset);
   connect(ui->actionCPU, &QAction::triggered, this, &MainWindow::Break);
   connect(ui->actionMemory, &QAction::triggered, this, &MainWindow::Memory);
   connect(ui->actionCopper, &QAction::triggered, this, &MainWindow::Copper);
   connect(ui->actionExec, &QAction::triggered, this, &MainWindow::Exec);
   
   connect(ui->display_, SIGNAL(Update()),
      this, SLOT(Update()), Qt::QueuedConnection);

   // Create debug window
   debug_.SetEmulator(emu_handler_);
   memory_.SetEmulator(emu_handler_);
   copper_.SetEmulator(emu_handler_);
   exec_.SetEmulator(emu_handler_);

   led_on_ = new QPixmap(":/Images/led_on.png");
   led_off_ = new QPixmap(":/Images/led_off.png");
   iconled_ = new QLabel;
   iconled_->setPixmap(*led_off_);
   statusBar()->addWidget(iconled_);


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

/////////////////////////////////////////////////////////////////////////////
// Draw & update
/////////////////////////////////////////////////////////////////////////////
void MainWindow::Update()
{
   ui->display_->update();
   // Set led on/off - todo
   emu_handler_->GetMotherboard();
   iconled_->setPixmap(*led_off_);
}