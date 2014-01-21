/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "client.h"
#include "blackmisc/vaudiodevicelist.h"
#include "blackmisc/avcallsignlist.h"

using namespace BlackMisc::Voice;
using namespace BlackMisc::Aviation;


/*
 * Client
 */
Client::Client(QObject *parent) :
    QObject(parent),
    m_voiceClient(&BlackMisc::IContext::getInstance().getObject<BlackCore::IVoice>())
{
    using namespace BlackCore;
    connect(m_voiceClient, &IVoice::squelchTestFinished,                  this, &Client::onSquelchTestFinished);
    connect(m_voiceClient, &IVoice::micTestFinished,                      this, &Client::onMicTestFinished);
    connect(m_voiceClient, &IVoice::connected,                            this, &Client::connectionStatusConnected);
    connect(m_voiceClient, &IVoice::disconnected,                         this, &Client::connectionStatusDisconnected);
    connect(m_voiceClient, &IVoice::audioStarted,                         this, &Client::audioStartedStream);
    connect(m_voiceClient, &IVoice::audioStopped,                         this, &Client::audioStoppedStream);
    connect(m_voiceClient, &IVoice::userJoinedRoom,                       this, &Client::userJoinedRoom);
    connect(m_voiceClient, &IVoice::userLeftRoom,                         this, &Client::userLeftRoom);

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
    m_voiceClient->joinVoiceRoom(BlackCore::IVoice::COM1, BlackMisc::Voice::CVoiceRoom(hostname, channel));
    printLinePrefix();
}

void Client::terminateConnectionCmd(QTextStream & /** args **/)
{
    std::cout << "Leaving room." << std::endl;
    m_voiceClient->leaveVoiceRoom(BlackCore::IVoice::COM1);
    printLinePrefix();
}

void Client::inputDevicesCmd(QTextStream & /** args **/)
{
    foreach(BlackMisc::Voice::CAudioDevice device, this->m_voiceClient->audioDevices().getInputDevices())
    {
        std::cout << device.getName().toStdString() << std::endl;
    }
    printLinePrefix();
}

/*
 * Output devices
 */
void Client::outputDevicesCmd(QTextStream & /** args **/)
{
    foreach(BlackMisc::Voice::CAudioDevice device, this->m_voiceClient->audioDevices().getOutputDevices())
    {
        std::cout << device.getName().toStdString() << std::endl;
    }
    printLinePrefix();
}

/*
 * Input devices
 */
void Client::listCallsignsCmd(QTextStream &args)
{
    Q_UNUSED(args)
    CCallsignList callsigns = m_voiceClient->getVoiceRoomCallsigns(BlackCore::IVoice::COM1);
    foreach(CCallsign callsign, callsigns)
    {
        std::cout << " " << callsign.toStdString() << std::endl;
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

void Client::connectionStatusConnected(const BlackCore::IVoice::ComUnit /** comUnit **/)
{
    std::cout << "CONN_STATUS_CONNECTED" << std::endl;
    printLinePrefix();
}

void Client::connectionStatusDisconnected()
{
    std::cout << "CONN_STATUS_DISCONNECTED" << std::endl;
    printLinePrefix();
}

void Client::audioStartedStream(const BlackCore::IVoice::ComUnit comUnit)
{
    std::cout << "Started stream in room index " << static_cast<qint32>(comUnit) << std::endl;
    printLinePrefix();
}

void Client::audioStoppedStream(const BlackCore::IVoice::ComUnit comUnit)
{
    std::cout << "Stopped stream in room index " << static_cast<qint32>(comUnit) << std::endl;
    printLinePrefix();
}

void Client::userJoinedRoom(const CCallsign &callsign)
{
    std::cout << callsign.toStdString() << " joined the voice room." << std::endl;
    printLinePrefix();
}

void Client::userLeftRoom(const CCallsign &callsign)
{
    std::cout << callsign.toStdString() << " left the voice room." << std::endl;
    printLinePrefix();
}


