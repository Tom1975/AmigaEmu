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
   QCommandLineOption t("t", "test option", "test");
   parser.addOption(t);
   parser.process(app);

   if (parser.isSet(t))
   {
      QString file = parser.value(t);
      const char* path = file.toStdString().c_str();

   }

   QString filepath_df0;
   if (parser.isSet(df0))
   {
      filepath_df0 = parser.value(df0);
      const char* path = filepath_df0.toStdString().c_str();
   }

   const QStringList args = parser.positionalArguments();

   MainWindow mainWin;
   mainWin.show();
   mainWin.InitEmulation();


   return app.exec();
}
