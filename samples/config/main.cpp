#include <QCoreApplication>
#include "blackmisc/debug.h"
#include "blackmisc/config_manager.h"
#include "blackmisc/config.h"
#include <limits>
#include <iostream>

using namespace BlackMisc;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
   
    BlackMisc::CApplicationContext ctx;
    BlackMisc::IContext::setInstance(ctx);
    BlackMisc::CDebug debug;
    ctx.setObject(debug);

    CConfigManager::getInstance().setConfigPath(QString("config"));
	if (!CConfigManager::getInstance().readConfig())
	{
		bAppWarning << "To run this sample, there must be a config folder";
		bAppWarning << "in the same directory, containing *.cfg files.";
	}

	CConfig *myConfig = CConfigManager::getInstance().getConfig("position");
	Q_ASSERT(myConfig);
	myConfig->display();
    
    return a.exec();
}
