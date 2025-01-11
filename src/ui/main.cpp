#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QSettings>

#include "PageController.h"
#include "QmlContextNetwork.h"
#include "QmlContextSimulator.h"
#include "SimSelection.h"

#include "core/application.h"
#include "core/context/context.h"
#include "core/context/contextnetwork.h"
#include "core/context/contextsimulator.h"
#include "core/pluginmanagersimulator.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("swift project");
    QCoreApplication::setApplicationName("pilotclient");
    QGuiApplication app(argc, argv);

    // Init swift core
    swift::core::CApplication swiftApp(swift::misc::CApplicationInfo::PilotClient);
    // swiftApp.addDBusAddressOption() // TODO Core mode
    swiftApp.addAudioOptions();
    swiftApp.addNetworkOptions();
    swiftApp.parseCommandLineArgsAndLoadSetup();

    swift::core::CCoreFacadeConfig config(swift::core::CCoreFacadeConfig::Local);
    swiftApp.initContextsAndStartCoreFacade(config);

    PageController controller;
    QmlContextNetwork net(swiftApp.getIContextNetwork());
    QmlContextSimulator sim(swiftApp.getIContextSimulator());

    QQmlApplicationEngine engine;

    // Setup context for QML
    engine.rootContext()->setContextProperty("network", &net);
    engine.rootContext()->setContextProperty("simulator", &sim);
    engine.rootContext()->setContextProperty("controller", &controller);

    engine.load(QUrl(QStringLiteral("qrc:/main/main.qml")));

    if (engine.rootObjects().isEmpty()) { return -1; }
    return app.exec();
}