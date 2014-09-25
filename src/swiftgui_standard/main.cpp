#include "introwindow.h"
#include "mainwindow.h"
#include "guimodeenums.h"
#include "blackgui/stylesheetutility.h"
#include "blackcore/blackcorefreefunctions.h"
#include "blackcore/context_runtime_config.h"
#include "blacksim/blacksimfreefunctions.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/icons.h"
#include "blackmisc/loghandler.h"

#include <QtGlobal>
#include <QApplication>
#include <QMessageBox>
#include <QPushButton>

using namespace BlackGui;

/*!
 * \brief Main
 * \param argc
 * \param argv
 * \return
 */
int main(int argc, char *argv[])
{
    // register
    BlackMisc::initResources();
    BlackMisc::registerMetadata();
    BlackSim::registerMetadata();
    BlackCore::registerMetadata();
    // BlackMisc::displayAllUserMetatypesTypes();

    // Translations
    QFile file(":blackmisc/translations/blackmisc_i18n_de.qm");
    qDebug() << (file.exists() ? "Found translations in resources" : "No translations in resources");
    QTranslator translator;
    if (translator.load("blackmisc_i18n_de", ":blackmisc/translations/"))
    {
        qDebug() << "Translator loaded";
    }

    // application
    QApplication a(argc, argv);
    CLogHandler::instance()->install();
    QIcon icon(BlackMisc::CIcons::swift24());
    QApplication::setWindowIcon(icon);
    const QString s = CStyleSheetUtility::instance().styles(
    {
        CStyleSheetUtility::fileNameFonts(),
        CStyleSheetUtility::fileNameMainWindow()
    }
    );
    a.installTranslator(&translator);
    a.setStyleSheet(s);

    // modes
    GuiModes::WindowMode windowMode;
    GuiModes::CoreMode coreMode;

    // Dialog to decide external or internal core
    CIntroWindow intro;
    intro.setWindowIcon(icon);
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
        switch (coreMode)
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
