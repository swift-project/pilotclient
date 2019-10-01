// #include "voiceclientui.h"

#include "blackcore/afv/model/atcstationmodel.h"
#include "blackcore/afv/model/afvmapreader.h"
#include "blackcore/afv/clients/afvclient.h"

#include "blackcore/application.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QPointer>
#include <QThread>

using namespace BlackMisc;
using namespace BlackCore::Afv::Clients;
using namespace BlackCore::Afv::Model;

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication qa(argc, argv);

    BlackCore::CApplication a("sampleafvclient", CApplicationInfo::Sample);

    CAfvMapReader *afvMapReader = new CAfvMapReader(&a);
    afvMapReader->updateFromMap();

    CAfvClient voiceClient("https://voice1.vatsim.uk");

    QQmlApplicationEngine engine;
    QQmlContext *ctxt = engine.rootContext();
    ctxt->setContextProperty("afvMapReader", afvMapReader);
    ctxt->setContextProperty("voiceClient", &voiceClient);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return a.exec();
}
