#include "blackcore/pluginmgr.h"
#include "blackmisc/plugins.h"
#include "blackmisc/context.h"
#include "blackmisc/debug.h"
#include <QCoreApplication>
#include <iostream>
#include <vector>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    BlackMisc::CApplicationContext myApplicationContext;

    BlackMisc::IContext::getInstance().setSingleton(new BlackMisc::CDebug());

    {
        const QString pluginPath = "../../build/bin";

        std::cout << "Loading plugins from " << pluginPath.toStdString() << std::endl;

        BlackCore::CPluginManager pluginmgr;
        pluginmgr.loadAllPlugins(pluginPath);
        size_t count = pluginmgr.getPluginCount();

        std::cout << count << " plugins loaded" << std::endl << std::endl;

        for (size_t i = 0; i < count; ++i)
        {
            std::cout << "Plugin " << i << ": " << pluginmgr.getName(i).toStdString()
                                        << ": " << pluginmgr.getDescription(i).toStdString() << std::endl;
        }
        std::cout << std::endl;

        std::vector<BlackMisc::IPlugin*> plugins;

        for (size_t i = 0; i < count; ++i)
        {
            std::cout << "Constructing plugin " << i << std::endl;

            plugins.push_back(pluginmgr.constructPlugin(i));
        }

        size_t i = 0;
        for (std::vector<BlackMisc::IPlugin*>::iterator it = plugins.begin(); it != plugins.end(); ++it, ++i)
        {
            std::cout << "Destroying plugin " << i << std::endl;

            (*it)->getFactory().destroy(*it);
        }
    }

    //return a.exec();
}
