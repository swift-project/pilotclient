/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "client.h"
#include "blackcore/voice_vatlib.h"
#include "blackmisc/audiodevicelist.h"
#include "blackmisc/avcallsignlist.h"

using namespace BlackMisc::Audio;
using namespace BlackMisc::Aviation;


/*
 * Client
 */
Client::Client(QObject *parent) :
    QObject(parent),
    m_voice(new BlackCore::CVoiceVatlib())
{
    using namespace BlackCore;
    connect(m_voice, &IVoice::squelchTestFinished,                  this, &Client::onSquelchTestFinished);
    connect(m_voice, &IVoice::micTestFinished,                      this, &Client::onMicTestFinished);
    connect(m_voice, &IVoice::connectionStatusChanged,              this, &Client::connectionStatusChanged);
    connect(m_voice, &IVoice::audioStarted,                         this, &Client::audioStartedStream);
    connect(m_voice, &IVoice::audioStopped,                         this, &Client::audioStoppedStream);
    connect(m_voice, &IVoice::userJoinedRoom,                       this, &Client::userJoinedRoom);
    connect(m_voice, &IVoice::userLeftRoom,                         this, &Client::userLeftRoom);

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
    m_voice->runSquelchTest();
}

void Client::micTestCmd(QTextStream & /** args **/)
{
    std::cout << "Running mic test. Speak normally for 5 seconds..." << std::endl;
    printLinePrefix();
    m_voice->runMicrophoneTest();
}

void Client::setCallsignCmd(QTextStream &args)
{
    QString callsign;
    args >> callsign;
    m_voice->setMyAircraftCallsign(BlackMisc::Aviation::CCallsign(callsign));
}

void Client::initiateConnectionCmd(QTextStream &args)
{
    QString hostname;
    QString channel;
    args >> hostname >> channel;
    std::cout << "Joining voice room: " << hostname.toStdString() << "/" << channel.toStdString() << std::endl;
    m_voice->joinVoiceRoom(BlackCore::IVoice::COM1, BlackMisc::Audio::CVoiceRoom(hostname, channel));
    printLinePrefix();
}

void Client::terminateConnectionCmd(QTextStream & /** args **/)
{
    std::cout << "Leaving room." << std::endl;
    m_voice->leaveVoiceRoom(BlackCore::IVoice::COM1);
    printLinePrefix();
}

void Client::inputDevicesCmd(QTextStream & /** args **/)
{
    foreach(BlackMisc::Audio::CAudioDevice device, this->m_voice->audioDevices().getInputDevices())
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
    foreach(BlackMisc::Audio::CAudioDevice device, this->m_voice->audioDevices().getOutputDevices())
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
    CCallsignList callsigns = m_voice->getVoiceRoomCallsigns(BlackCore::IVoice::COM1);
    foreach(CCallsign callsign, callsigns)
    {
        std::cout << " " << callsign.toStdString() << std::endl;
    }
    printLinePrefix();
}

void Client::onSquelchTestFinished()
{
    std::cout << "Input squelch: " << m_voice->inputSquelch() << std::endl;
    printLinePrefix();
}

void Client::onMicTestFinished()
{
    std::cout << "Mic test result: " << (int)m_voice->micTestResult() << std::endl;
    printLinePrefix();
}

void Client::connectionStatusChanged(BlackCore::IVoice::ComUnit /** comUnit **/,
                                       BlackCore::IVoice::ConnectionStatus /** oldStatus **/,
                                       BlackCore::IVoice::ConnectionStatus newStatus)
{
    switch (newStatus)
    {
    case BlackCore::IVoice::Disconnected:
        std::cout << "CONN_STATUS_DISCONNECTED" << std::endl;
        break;
    case BlackCore::IVoice::Disconnecting:
        std::cout << "CONN_STATUS_DISCONNECTING" << std::endl;
        break;
    case BlackCore::IVoice::DisconnectedError:
        std::cout << "CONN_STATUS_DISCONNECTED_ERROR" << std::endl;
        break;
    case BlackCore::IVoice::Connecting:
        std::cout << "CONN_STATUS_CONNECTING" << std::endl;
        break;
    case BlackCore::IVoice::Connected:
        std::cout << "CONN_STATUS_CONNECTED" << std::endl;
        break;
    case BlackCore::IVoice::ConnectingFailed:
        std::cout << "CONN_STATUS_CONNECTING_FAILED" << std::endl;
        break;
    }
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


