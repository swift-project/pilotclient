/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "client.h"
#include "blackcore/voice_vatlib.h"
#include "blackmisc/audiodeviceinfolist.h"
#include "blackmisc/aviation/callsignlist.h"
#include <QDateTime>

using namespace BlackMisc::Audio;
using namespace BlackMisc::Aviation;
using namespace BlackCore;

/*
 * Client
 */
Client::Client(QObject *parent) :
    QObject(parent),
    m_voice(new BlackCore::CVoiceVatlib()),
    m_stdout(stdout)
{
    m_channelCom1 = m_voice->createVoiceChannel();
    m_inputDevice = m_voice->createInputDevice();
    m_outputDevice = m_voice->createOutputDevice();
    m_audioMixer = m_voice->createAudioMixer();

    m_voice->connectVoice(m_inputDevice.get(), m_audioMixer.get(), IAudioMixer::InputMicrophone);
    m_voice->connectVoice(m_channelCom1.data(), m_audioMixer.get(), IAudioMixer::InputVoiceChannel1);
    m_voice->connectVoice(m_audioMixer.get(), IAudioMixer::OutputOutputDevice1, m_outputDevice.get());
    m_audioMixer->makeMixerConnection(IAudioMixer::InputVoiceChannel1, IAudioMixer::OutputOutputDevice1);

    using namespace BlackCore;
    connect(m_channelCom1.data(), &IVoiceChannel::connectionStatusChanged,              this, &Client::connectionStatusChanged);
    connect(m_channelCom1.data(), &IVoiceChannel::audioStarted,                         this, &Client::audioStartedStream);
    connect(m_channelCom1.data(), &IVoiceChannel::audioStopped,                         this, &Client::audioStoppedStream);
    connect(m_channelCom1.data(), &IVoiceChannel::userJoinedRoom,                       this, &Client::userJoinedRoom);
    connect(m_channelCom1.data(), &IVoiceChannel::userLeftRoom,                         this, &Client::userLeftRoom);

    using namespace std::placeholders;
    m_commands["help"]              = std::bind(&Client::help, this, _1);
    m_commands["echo"]              = std::bind(&Client::echo, this, _1);
    m_commands["exit"]              = std::bind(&Client::exit, this, _1);
    m_commands["setcallsign"]       = std::bind(&Client::setCallsignCmd, this, _1);
    m_commands["initconnect"]       = std::bind(&Client::initiateConnectionCmd, this, _1);
    m_commands["termconnect"]       = std::bind(&Client::terminateConnectionCmd, this, _1);
    m_commands["inputdevices"]      = std::bind(&Client::inputDevicesCmd, this, _1);
    m_commands["outputdevices"]     = std::bind(&Client::outputDevicesCmd, this, _1);
    m_commands["users"]             = std::bind(&Client::listCallsignsCmd, this, _1);
    m_commands["enableloopback"]    = std::bind(&Client::enableLoopbackCmd, this, _1);
    m_commands["disableloopback"]   = std::bind(&Client::disableLoopbackCmd, this, _1);
}

Client::~Client()
{
    if(m_voice) m_voice->deleteLater();
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
        m_stdout << "No such command" << endl;
        printLinePrefix();
    }
    else
    {
        (*found)(stream);
    }
}

void Client::printLinePrefix()
{
    QTextStream(stdout) << "voice> ";
}

/****************************************************************************/
/************                      Commands                     *************/
/****************************************************************************/

void Client::help(QTextStream &)
{
    m_stdout << "Commands:" << endl;
    auto keys = m_commands.keys();
    for (auto &key : keys)
    {
        m_stdout << " " << key << endl;
    }
    printLinePrefix();
}

void Client::echo(QTextStream &line)
{
    m_stdout << "echo: " << line.readAll() << endl;
    printLinePrefix();
}

void Client::exit(QTextStream &)
{
    m_stdout << "Shutting down...";
    emit quit();
}

void Client::setCallsignCmd(QTextStream &args)
{
    QString callsign;
    args >> callsign;
    m_channelCom1->setMyAircraftCallsign(BlackMisc::Aviation::CCallsign(callsign));
}

void Client::initiateConnectionCmd(QTextStream &args)
{
    QString hostname;
    QString channel;
    args >> hostname >> channel;
    m_stdout << getCurrentTimeStamp() << "Joining voice room: " << hostname << "/" << channel << endl;
    m_channelCom1->joinVoiceRoom(BlackMisc::Audio::CVoiceRoom(hostname, channel));
    printLinePrefix();
}

void Client::terminateConnectionCmd(QTextStream & /** args **/)
{
    m_stdout << getCurrentTimeStamp() << "Leaving room." << endl;
    m_channelCom1->leaveVoiceRoom();
    printLinePrefix();
}

void Client::inputDevicesCmd(QTextStream & /** args **/)
{
    for(const auto &device : m_inputDevice->getInputDevices())
    {
        m_stdout << device.getName() << endl;
    }
    printLinePrefix();
}

/*
 * Output devices
 */
void Client::outputDevicesCmd(QTextStream & /** args **/)
{
    for(const auto &device : m_outputDevice->getOutputDevices())
    {
        m_stdout << device.getName() << endl;
    }
    printLinePrefix();
}

/*
 * Input devices
 */
void Client::listCallsignsCmd(QTextStream &args)
{
    Q_UNUSED(args)
    CCallsignList callsigns = m_channelCom1->getVoiceRoomCallsigns();
    foreach(CCallsign callsign, callsigns)
    {
        m_stdout << " " << callsign << endl;
    }
    printLinePrefix();
}

void Client::enableLoopbackCmd(QTextStream &/*args*/)
{
    m_stdout << endl;
    m_stdout << "Enabling audio loopback." << endl;
    m_audioMixer->makeMixerConnection(IAudioMixer::InputMicrophone, IAudioMixer::OutputOutputDevice1);
    printLinePrefix();
}

void Client::disableLoopbackCmd(QTextStream &/*args*/)
{
    m_stdout << endl;
    m_stdout << "Disabling audio loopback." << endl;
    m_audioMixer->removeMixerConnection(IAudioMixer::InputMicrophone, IAudioMixer::OutputOutputDevice1);
    printLinePrefix();
}

void Client::connectionStatusChanged( BlackCore::IVoiceChannel::ConnectionStatus /** oldStatus **/,
                                       BlackCore::IVoiceChannel::ConnectionStatus newStatus)
{
    switch (newStatus)
    {
    case BlackCore::IVoiceChannel::Disconnected:
        m_stdout << endl;
        m_stdout << getCurrentTimeStamp() << "CONN_STATUS_DISCONNECTED" << endl;
        break;
    case BlackCore::IVoiceChannel::Disconnecting:
        m_stdout << endl;
        m_stdout << getCurrentTimeStamp() << "CONN_STATUS_DISCONNECTING" << endl;
        break;
    case BlackCore::IVoiceChannel::DisconnectedError:
        m_stdout << endl;
        m_stdout << getCurrentTimeStamp() << "CONN_STATUS_DISCONNECTED_ERROR" << endl;
        break;
    case BlackCore::IVoiceChannel::Connecting:
        m_stdout << endl;
        m_stdout << getCurrentTimeStamp() << "CONN_STATUS_CONNECTING" << endl;
        break;
    case BlackCore::IVoiceChannel::Connected:
        m_stdout << endl;
        m_stdout << getCurrentTimeStamp() << "CONN_STATUS_CONNECTED" << endl;
        break;
    case BlackCore::IVoiceChannel::ConnectingFailed:
        m_stdout << endl;
        m_stdout << getCurrentTimeStamp() << "CONN_STATUS_CONNECTING_FAILED" << endl;
        break;
    }
    printLinePrefix();
}

void Client::audioStartedStream()
{
    m_stdout << endl;
    m_stdout << getCurrentTimeStamp() << "Started stream in room index " << endl;
    printLinePrefix();
}

void Client::audioStoppedStream()
{
    m_stdout << endl;
    m_stdout << getCurrentTimeStamp() << "Stopped stream in room index " << endl;
    printLinePrefix();
}

void Client::userJoinedRoom(const CCallsign &callsign)
{
    m_stdout << endl;
    m_stdout << getCurrentTimeStamp() << callsign << " joined the voice room." << endl;
    printLinePrefix();
}

void Client::userLeftRoom(const CCallsign &callsign)
{
    m_stdout << endl;
    m_stdout << getCurrentTimeStamp() << callsign << " left the voice room." << endl;
    printLinePrefix();
}

QString Client::getCurrentTimeStamp() const
{
    QString timeStamp = QDateTime::currentDateTime().toString("hh:mm:ss") + " : ";
    return timeStamp;
}
