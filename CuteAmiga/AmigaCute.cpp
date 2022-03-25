/****************************************************************
**
** Qt tutorial 1
**
****************************************************************/

#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
   Q_INIT_RESOURCE(resources);


   QApplication app(argc, argv);
   QCoreApplication::setOrganizationName("Amiga");
   QCoreApplication::setApplicationName("Amiga");
   QCoreApplication::setApplicationVersion(QT_VERSION_STR);
   QCommandLineParser parser;
   parser.setApplicationDescription(QCoreApplication::applicationName());
   parser.addHelpOption();
   parser.addVersionOption();

   
   QCommandLineOption df0("df0", "Disk image to insert in df0", "directory");
   parser.addOption(df0);
   parser.process(app);

   QString filepath_df0;
   if (parser.isSet(df0))
   {
      filepath_df0 = parser.value(df0);
   }

   MainWindow mainWin;
   mainWin.show();
   mainWin.InitEmulation();

   // Init what can be initialised

   return app.exec();
}
