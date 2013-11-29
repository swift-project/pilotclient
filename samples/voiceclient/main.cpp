/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackcore/voiceclient_vatlib.h"
#include <QCoreApplication>
#include <QDebug>

using namespace BlackMisc::Voice;

int main(int argc, char *argv[])
{
    QCoreApplication app (argc, argv);
    BlackMisc::IContext::getInstance().setObject(*new BlackMisc::CDebug());
    BlackMisc::IContext::getInstance().setObject<BlackCore::IVoiceClient>(*new BlackCore::CVoiceClientVatlib());

    BlackCore::IVoiceClient *voiceClient = BlackMisc::IContext::getInstance().singleton<BlackCore::IVoiceClient>();
    QList<COutputAudioDevice> outputDevices = voiceClient->audioOutputDevices(0);

    qDebug() << "Found " << outputDevices.size() << " output devices:";

    foreach (COutputAudioDevice device, outputDevices)
    {
        qDebug() << device.name();
    }

    app.exec();
}
