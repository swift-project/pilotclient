#include "introwindow.h"
#include "mainwindow.h"
#include "guimodeenums.h"
#include "blackcore/context_runtime_config.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include <QtGlobal>
#include <QApplication>
#include <QMessageBox>
#include <QPushButton>

/*!
 * \brief Main
 * \param argc
 * \param argv
 * \return
 */
int main(int argc, char *argv[])
{
    // register
    Q_INIT_RESOURCE(blackgui);
    BlackMisc::initResources();
    BlackMisc::registerMetadata();
    // BlackMisc::displayAllUserMetatypesTypes();

    QFile file(":blackmisc/translations/blackmisc_i18n_de.qm");
    qDebug() << (file.exists() ? "Found translations in resources" : "No translations in resources");
    QTranslator translator;
    if (translator.load("blackmisc_i18n_de", ":blackmisc/translations/"))
    {
        qDebug() << "Translator loaded";
    }

    // app
    QApplication a(argc, argv);
    a.installTranslator(&translator);

    // modes
    GuiModes::WindowMode windowMode;
    GuiModes::CoreMode coreMode;

    // Dialog to decide external or internal core
    CIntroWindow intro;
    BlackCore::CRuntimeConfig runtimeConfig;
    if (intro.exec() == QDialog::Rejected)
    {
        return 0;
    }
    else
    {
        coreMode = intro.getCoreMode();
        windowMode = intro.getWindowMode();
        QString dbusAddress = BlackCore::CDBusServer::fixAddressToDBusAddress(intro.getDBusAddress());
        switch (windowMode)
        {
        case GuiModes::CoreExternal:
            runtimeConfig =  BlackCore::CRuntimeConfig::remote(dbusAddress);
            break;
        case GuiModes::CoreInGuiProcess:
            runtimeConfig =  BlackCore::CRuntimeConfig::local(dbusAddress);
            break;
        case GuiModes::CoreExternalAudioLocal:
            runtimeConfig =  BlackCore::CRuntimeConfig::remoteLocalAudio(dbusAddress);
            break;
        }
    }
    intro.close();

    // show window
    MainWindow w(windowMode);
    w.show();
    w.init(runtimeConfig); // object is complete by now
    int r = a.exec();
    return r;
}
