/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CLIENT_H
#define CLIENT_H

#include "blackcore/voiceclient.h"

#include <QObject>
#include <functional>
#include <QMap>

class Client : public QObject
{
    Q_OBJECT
public:
    Client(QObject *parent = 0);

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

    void printLinePrefix();

public slots:
    void onSquelchTestFinished();
    void onMicTestFinished();
private slots:

    void connectionStatusConnected();
    void connectionStatusDisconnected();

private:
    QMap<QString, std::function<void(QTextStream &)>> m_commands;
    BlackCore::IVoiceClient *m_voiceClient;

};

#endif // CLIENT_H
