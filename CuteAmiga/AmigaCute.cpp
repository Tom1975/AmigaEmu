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
   parser.addPositionalArgument("file", "The file to open.");
   parser.process(app);

   MainWindow mainWin;
   mainWin.show();
   mainWin.InitEmulation();


   return app.exec();
}
