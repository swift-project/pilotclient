#include "swiftcore.h"

#include "blackcore/context_runtime.h"
#include "blackcore/context_settings.h"
#include "blackcore/context_application.h"
#include "blackcore/context_application_impl.h"
#include "blackcore/dbus_server.h"
#include "blackmisc/icons.h"
#include "blackmisc/worker.h"
#include "blackmisc/networkutils.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/project.h"
#include "blackmisc/loghandler.h"
#include "blackgui/stylesheetutility.h"

#include <QApplication>
#include <QMessageBox>

using namespace BlackMisc;
using namespace BlackCore;
using namespace BlackGui;

enum CommandLineParseResult
{
    CommandLineOk,
    CommandLineError,
    CommandLineVersionRequested,
    CommandLineHelpRequested
};

CommandLineParseResult parseCommandLine(QCommandLineParser &parser, CSwiftCore::SetupInfo *setup, QString *errorMessage)
{
    parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
    parser.addOption({{"s", "session"}, QCoreApplication::translate("main", "Use session bus.")});
    parser.addOption({{"y", "system"}, QCoreApplication::translate("main", "Use system bus.")});
    parser.addOption({{"p", "p2p"}, QCoreApplication::translate("main", "Use P2P bus with <address>."),
        QCoreApplication::translate("main", "address")
    });
    parser.addOption({{"m", "minimized"}, QCoreApplication::translate("main", "Start minimized in system tray.")});

    if (!parser.parse(QCoreApplication::arguments()))
    {
        *errorMessage = parser.errorText();
        return CommandLineError;
    }

    if (parser.isSet("session"))
    {
        if (parser.isSet("system") || parser.isSet("p2p"))
        {
            *errorMessage = "Multiple DBus types set at the same time.";
            return CommandLineError;
        }
        setup->m_dbusAddress = CDBusServer::sessionDBusServer();
    }

    if (parser.isSet("system"))
    {
        if (parser.isSet("session") || parser.isSet("p2p"))
        {
            *errorMessage = "Multiple DBus types set at the same time.";
            return CommandLineError;
        }
        setup->m_dbusAddress = CDBusServer::systemDBusServer();
    }

    if (parser.isSet("p2p"))
    {
        const QString address = CDBusServer::fixAddressToDBusAddress(parser.value("p2p"));

        if (parser.isSet("session") || parser.isSet("system"))
        {
            *errorMessage = "Multiple DBus types set at the same time.";
            return CommandLineError;
        }
    }

    if (parser.isSet("minimized"))
    {
        setup->m_minimzed = true;
    }

    return CommandLineOk;
}

int main(int argc, char *argv[])
{
    CRuntime::registerMetadata(); // register metadata
    QApplication a(argc, argv);
    QApplication::setApplicationName("swiftcore");
    QApplication::setApplicationVersion(CProject::version());

    QCommandLineParser parser;
    parser.setApplicationDescription("swiftcore control");
    parser.addHelpOption();
    parser.addVersionOption();

    CSwiftCore::SetupInfo setup;
    QString errorMessage;
    switch (parseCommandLine(parser, &setup, &errorMessage))
    {
    case CommandLineOk:
        break;
    case CommandLineError:
#ifdef Q_OS_WIN
        QMessageBox::warning(0, QGuiApplication::applicationDisplayName(),
                             "<html><head/><body><h2>" + errorMessage + "</h2><pre>"
                             + parser.helpText() + "</pre></body></html>");
#else
        fputs(qPrintable(errorMessage), stderr);
        fputs("\n\n", stderr);
        fputs(qPrintable(parser.helpText()), stderr);
#endif
        return 1;
    case CommandLineVersionRequested:
#ifdef Q_OS_WIN
        QMessageBox::information(0, QGuiApplication::applicationDisplayName(),
                                 QGuiApplication::applicationDisplayName() + ' '+ QCoreApplication::applicationVersion());
#else
        printf("%s %s\n", qPrintable(QCoreApplication::applicationName()),
               qPrintable(QCoreApplication::applicationVersion()));
#endif
        return 0;
    case CommandLineHelpRequested:
#ifdef Q_OS_WIN
        QMessageBox::warning(0, QGuiApplication::applicationDisplayName(),
                             "<html><head/><body><pre>"
                             + parser.helpText() + "</pre></body></html>");
        return 0;
#else
        parser.showHelp();
        Q_UNREACHABLE();
#endif
    }

    if (!QSystemTrayIcon::isSystemTrayAvailable())
    {
        QMessageBox::critical(0, QObject::tr("Systray"),
                              QObject::tr("I couldn't detect any system tray on this system."));
        return 1;
    }

    // Translations
    QFile file(":blackmisc/translations/blackmisc_i18n_de.qm");
    CLogMessage("swift.standardgui.main").debug() << (file.exists() ? "Found translations in resources" : "No translations in resources");
    QTranslator translator;
    if (translator.load("blackmisc_i18n_de", ":blackmisc/translations/"))
    {
        CLogMessage("swift.standardgui.main").debug() << "Translator loaded";
    }

    QIcon icon(BlackMisc::CIcons::swift24());
    QApplication::setWindowIcon(icon);
    const QString s = CStyleSheetUtility::instance().styles(
    {
        CStyleSheetUtility::fileNameFonts(),
        CStyleSheetUtility::fileNameSwiftCore()
    }
    );
    a.installTranslator(&translator);
    a.setStyleSheet(s);

    CSwiftCore w(setup);
    if (!setup.m_minimzed) w.show();

    return a.exec();
}
