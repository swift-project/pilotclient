#include <QApplication>

#include <blackmisc/context.h>
#include "server.h"

using namespace BlackMisc;

int main(int argc, char *argv[])
{
    BlackMisc::CApplicationContext myApplicationContext;
    QApplication a(argc, argv);

	Server server;
    
    return a.exec();
}
