#include <QApplication>

#include <blackmisc/context.h>
#include "client.h"

using namespace BlackMisc;

int main(int argc, char *argv[])
{
    BlackMisc::CApplicationContext myApplicationContext;
    QApplication a(argc, argv);

	Client client;
    
    return a.exec();
}
