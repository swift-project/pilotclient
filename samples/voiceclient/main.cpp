/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackcore/voice_vatlib.h"
#include "blackmisc/blackmiscfreefunctions.h"

#include "client.h"
#include "reader.h"

#include <QCoreApplication>
#include <QDebug>
#include <QThread>

using namespace BlackMisc::Audio;

int main(int argc, char *argv[])
{
    QCoreApplication app (argc, argv);
    Client client(&app);
    BlackMisc::registerMetadata();
    BlackCore::IVoice *m_voice = new BlackCore::CVoiceVatlib();
    QThread m_voiceThread;
    m_voice->moveToThread(&m_voiceThread);
    m_voiceThread.start();
    LineReader reader;
    QObject::connect(&reader, SIGNAL(command(const QString &)), &client, SLOT(command(const QString &)));
    QObject::connect(&client, &Client::quit, [&] ()
    {
        qDebug() << "Shutting down...";
        reader.terminate();
        m_voiceThread.quit();
        m_voiceThread.wait(5000);
        qApp->quit();
    });

    reader.start();
    app.exec();
}
