// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include <QGuiApplication>
#include <QPointer>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QThread>
#include <QTimer>

#include "afvclientbridge.h"

#include "config/buildconfig.h"
#include "core/afv/clients/afvclient.h"
#include "core/afv/model/afvmapreader.h"
#include "core/afv/model/atcstationmodel.h"
#include "core/application.h"
#include "core/registermetadata.h"
#include "misc/network/user.h"
#include "misc/obfuscation.h"

using namespace swift::config;
using namespace swift::misc;
using namespace swift::misc::network;
using namespace swift::core;
using namespace swift::core::afv::clients;
using namespace swift::core::afv::model;

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication qa(argc, argv);

    swift::core::registerMetadata();
    swift::core::CApplication a("sampleafvclient", CApplicationInfo::Sample);

    CAfvMapReader *afvMapReader = new CAfvMapReader(&a);
    afvMapReader->updateFromMap();

    CAfvClient *voiceClient = new CAfvClient("https://voice1.vatsim.net", &qa);
    voiceClient->start(QThread::TimeCriticalPriority); // background thread
    CAfvClientBridge *voiceClientBridge = new CAfvClientBridge(voiceClient, &qa);

    QObject::connect(&qa, &QCoreApplication::aboutToQuit, [voiceClient]() { voiceClient->quitAndWait(); });

    // default user name
    QString defaultUserName("1234567");
    if (CBuildConfig::isLocalDeveloperDebugBuild())
    {
        const CUser user("OBF:AwLZ7f9hUmpSZhm4=", "Joe Doe");
        defaultUserName = user.getId();
    }

    QQmlApplicationEngine engine;
    QQmlContext *ctxt = engine.rootContext();
    ctxt->setContextProperty("afvMapReader", afvMapReader);
    ctxt->setContextProperty("voiceClient", voiceClientBridge);
    ctxt->setContextProperty("userName", defaultUserName);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    return a.exec();
}
