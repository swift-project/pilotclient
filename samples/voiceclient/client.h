/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SAMPLE_VOICECLIENT
#define SAMPLE_VOICECLIENT

#include "blackcore/voice.h"

#include <QObject>
#include <functional>
#include <QMap>
#include <QThread>

class Client : public QObject
{
    Q_OBJECT

public:
    Client(QObject *parent = nullptr);
    ~Client() { if(m_voice) m_voice->deleteLater(); }

signals:
    void quit();

public slots:
    void command(QString line);

private: //commands
    void help(QTextStream &args);
    void echo(QTextStream &args);
    void exit(QTextStream &args);
    void squelchTestCmd(QTextStream &args);
    void micTestCmd(QTextStream &args);
    void setCallsignCmd(QTextStream &args);
    void initiateConnectionCmd(QTextStream &args);
    void terminateConnectionCmd(QTextStream &args);
    void inputDevicesCmd(QTextStream &args);
    void outputDevicesCmd(QTextStream &args);
    void listCallsignsCmd(QTextStream &args);
    void enableLoopbackCmd(QTextStream &args);
    void disableLoopbackCmd(QTextStream &args);

    void printLinePrefix();

public slots:
    void onSquelchTestFinished();
    void onMicTestFinished();
private slots:

    void connectionStatusChanged(BlackCore::IVoice::ComUnit comUnit,
                                   BlackCore::IVoice::ConnectionStatus oldStatus,
                                   BlackCore::IVoice::ConnectionStatus newStatus);
    void audioStartedStream(const BlackCore::IVoice::ComUnit comUnit);
    void audioStoppedStream(const BlackCore::IVoice::ComUnit comUnit);
    void userJoinedRoom(const BlackMisc::Aviation::CCallsign &callsign);
    void userLeftRoom(const BlackMisc::Aviation::CCallsign &callsign);

private:
    QMap<QString, std::function<void(QTextStream &)>> m_commands;
    BlackCore::IVoice *m_voice;
    QThread m_threadVoice;

};

#endif // SAMPLE_VOICECLIENT
