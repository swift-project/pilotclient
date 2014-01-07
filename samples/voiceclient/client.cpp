/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "client.h"

Client::Client(QObject *parent) :
    QObject(parent),
    m_voiceClient(&BlackMisc::IContext::getInstance().getObject<BlackCore::IVoiceClient>())
{
    using namespace BlackCore;
    connect(m_voiceClient, &IVoiceClient::squelchTestFinished,                  this, &Client::onSquelchTestFinished);
    connect(m_voiceClient, &IVoiceClient::micTestFinished,                      this, &Client::onMicTestFinished);
    connect(m_voiceClient, &IVoiceClient::connected,                            this, &Client::connectionStatusConnected);
    connect(m_voiceClient, &IVoiceClient::disconnected,                         this, &Client::connectionStatusDisconnected);
    connect(m_voiceClient, &IVoiceClient::audioStarted,                         this, &Client::audioStartedStream);
    connect(m_voiceClient, &IVoiceClient::audioStopped,                         this, &Client::audioStoppedStream);
    connect(m_voiceClient, &IVoiceClient::userJoinedRoom,                       this, &Client::userJoinedRoom);
    connect(m_voiceClient, &IVoiceClient::userLeftRoom,                         this, &Client::userLeftRoom);

    using namespace std::placeholders;
    m_commands["help"]              = std::bind(&Client::help, this, _1);
    m_commands["echo"]              = std::bind(&Client::echo, this, _1);
    m_commands["exit"]              = std::bind(&Client::exit, this, _1);
    m_commands["squelchtest"]       = std::bind(&Client::squelchTestCmd, this, _1);
    m_commands["mictest"]           = std::bind(&Client::micTestCmd, this, _1);
    m_commands["setcallsign"]       = std::bind(&Client::setCallsignCmd, this, _1);
    m_commands["initconnect"]       = std::bind(&Client::initiateConnectionCmd, this, _1);
    m_commands["termconnect"]       = std::bind(&Client::terminateConnectionCmd, this, _1);
    m_commands["inputdevices"]      = std::bind(&Client::inputDevicesCmd, this, _1);
    m_commands["outputdevices"]     = std::bind(&Client::outputDevicesCmd, this, _1);
    m_commands["users"]             = std::bind(&Client::listCallsignsCmd, this, _1);

}

void Client::command(QString line)
{
    QTextStream stream(&line, QIODevice::ReadOnly);
    QString cmd;
    stream >> cmd;
    stream.skipWhiteSpace();

    auto found = m_commands.find(cmd);
    if (found == m_commands.end())
    {
        std::cout << "No such command" << std::endl;
        printLinePrefix();
    }
    else
    {
        (*found)(stream);
    }
}

void Client::printLinePrefix()
{
    std::cout << "voice> ";
    std::cout.flush();
}

/****************************************************************************/
/************                      Commands                     *************/
/****************************************************************************/

void Client::help(QTextStream &)
{
    std::cout << "Commands:" << std::endl;
    auto keys = m_commands.keys();
    for (auto i = keys.begin(); i != keys.end(); ++i)
    {
        std::cout << " " << i->toStdString() << std::endl;
    }
    printLinePrefix();
}

void Client::echo(QTextStream &line)
{
    std::cout << "echo: " << line.readAll().toStdString() << std::endl;
    printLinePrefix();
}

void Client::exit(QTextStream &)
{
    emit quit();
}

void Client::squelchTestCmd(QTextStream & /** args **/)
{
    std::cout << "Running squelch test. Please be quiet for 5 seconds..." << std::endl;
    printLinePrefix();
    m_voiceClient->runSquelchTest();
}

void Client::micTestCmd(QTextStream & /** args **/)
{
    std::cout << "Running mic test. Speak normally for 5 seconds..." << std::endl;
    printLinePrefix();
    m_voiceClient->runMicTest();
}

void Client::setCallsignCmd(QTextStream &args)
{
    QString callsign;
    args >> callsign;
    m_voiceClient->setMyAircraftCallsign(BlackMisc::Aviation::CCallsign(callsign));
}

void Client::initiateConnectionCmd(QTextStream &args)
{
    QString hostname;
    QString channel;
    args >> hostname >> channel;
    std::cout << "Joining voice room: " << hostname.toStdString() << "/" << channel.toStdString() << std::endl;
    m_voiceClient->joinVoiceRoom(BlackCore::IVoiceClient::COM1, BlackMisc::Voice::CVoiceRoom(hostname, channel));
    printLinePrefix();
}

void Client::terminateConnectionCmd(QTextStream & /** args **/)
{
    std::cout << "Leaving room." << std::endl;
    m_voiceClient->leaveVoiceRoom(BlackCore::IVoiceClient::COM1);
    printLinePrefix();
}

void Client::inputDevicesCmd(QTextStream & /** args **/)
{
    QList<BlackMisc::Voice::CInputAudioDevice> devices = m_voiceClient->audioInputDevices();
    foreach(BlackMisc::Voice::CInputAudioDevice device, devices)
    {
        std::cout << device.name().toStdString() << std::endl;
    }
    printLinePrefix();
}

void Client::outputDevicesCmd(QTextStream & /** args **/)
{
    QList<BlackMisc::Voice::COutputAudioDevice> devices = m_voiceClient->audioOutputDevices();
    foreach(BlackMisc::Voice::COutputAudioDevice device, devices)
    {
        std::cout << " " << device.name().toStdString() << std::endl;
    }
    printLinePrefix();
}

/*
 * Input devices
 */
void Client::listCallsignsCmd(QTextStream &args)
{
    Q_UNUSED(args)
    QSet<QString> users = m_voiceClient->roomUserList(BlackCore::IVoiceClient::COM1);
    foreach(QString user, users)
    {
        std::cout << " " << user.toStdString() << std::endl;
    }
    printLinePrefix();
}

void Client::onSquelchTestFinished()
{
    std::cout << "Input squelch: " << m_voiceClient->inputSquelch() << std::endl;
    printLinePrefix();
}

void Client::onMicTestFinished()
{
    std::cout << "Mic test result: " << (int)m_voiceClient->micTestResult() << std::endl;
    printLinePrefix();
}

void Client::connectionStatusConnected(const BlackCore::IVoiceClient::ComUnit /** comUnit **/)
{
    std::cout << "CONN_STATUS_CONNECTED" << std::endl;
    printLinePrefix();
}

void Client::connectionStatusDisconnected()
{
    std::cout << "CONN_STATUS_DISCONNECTED" << std::endl;
    printLinePrefix();
}

void Client::audioStartedStream(const BlackCore::IVoiceClient::ComUnit comUnit)
{
    std::cout << "Started stream in room index " << static_cast<int32_t>(comUnit) << std::endl;
    printLinePrefix();
}

void Client::audioStoppedStream(const BlackCore::IVoiceClient::ComUnit comUnit)
{
    std::cout << "Stopped stream in room index " << static_cast<int32_t>(comUnit) << std::endl;
    printLinePrefix();
}

void Client::userJoinedRoom(const QString &callsign)
{
    std::cout << callsign.toStdString() << " joined the voice room." << std::endl;
    printLinePrefix();
}

void Client::userLeftRoom(const QString &callsign)
{
    std::cout << callsign.toStdString() << " left the voice room." << std::endl;
    printLinePrefix();
}


