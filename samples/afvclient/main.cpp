// #include "voiceclientui.h"

#include "models/atcstationmodel.h"
#include "clients/afvclient.h"
#include "afvmapreader.h"
#include "blackcore/application.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QPointer>
#include <QThread>

using namespace BlackCore::Afv::Clients;

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication qa(argc, argv);

    BlackCore::CApplication a("sampleafvclient", BlackMisc::CApplicationInfo::Sample);

    AFVMapReader *afvMapReader = new AFVMapReader(&a);
    afvMapReader->updateFromMap();

    CAfvClient voiceClient("https://voice1.vatsim.uk");

    QQmlApplicationEngine engine;
    QQmlContext *ctxt = engine.rootContext();
    ctxt->setContextProperty("afvMapReader", afvMapReader);
    ctxt->setContextProperty("voiceClient", &voiceClient);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return a.exec();
}
