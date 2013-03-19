#include <QCoreApplication>

#include "blackmisc/context.h"
#include "client.h"

using namespace BlackMisc;

int main(int argc, char *argv[])
{
    BlackMisc::CApplicationContext myApplicationContext;
    QCoreApplication a(argc, argv);

	Client client;
    
    return a.exec();
}
