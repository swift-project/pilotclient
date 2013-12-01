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




    using namespace std::placeholders;
    m_commands["help"]              = std::bind(&Client::help, this, _1);
    m_commands["echo"]              = std::bind(&Client::echo, this, _1);
    m_commands["exit"]              = std::bind(&Client::exit, this, _1);
    m_commands["squelchtest"]       = std::bind(&Client::runSquelchTest, this, _1);
    m_commands["mictest"]           = std::bind(&Client::runMicTest, this, _1);
    m_commands["setcallsign"]       = std::bind(&Client::setCallsignCmd, this, _1);
    m_commands["initconnect"]       = std::bind(&Client::initiateConnectionCmd, this, _1);
    m_commands["termconnect"]       = std::bind(&Client::terminateConnectionCmd, this, _1);

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

void Client::runSquelchTest(QTextStream &args)
{
    std::cout << "Running squelch test. Please be quiet for 5 seconds..." << std::endl;
    printLinePrefix();
    m_voiceClient->runSquelchTest();
}

void Client::runMicTest(QTextStream &args)
{
    std::cout << "Running mic test. Speak normally for 5 seconds..." << std::endl;
    printLinePrefix();
    m_voiceClient->runMicTest();
}

void Client::setCallsignCmd(QTextStream &args)
{
    QString callsign;
    args >> callsign;
    m_voiceClient->setCallsign(BlackMisc::Aviation::CCallsign(callsign));
}

void Client::initiateConnectionCmd(QTextStream &args)
{
    QString hostname;
    QString channel;
    args >> hostname >> channel;
    std::cout << "Joining voice room: " << hostname.toStdString() << "/" << channel.toStdString() << std::endl;
    m_voiceClient->joinVoiceRoom(0, BlackMisc::Voice::CVoiceRoom(hostname, channel));
    printLinePrefix();
}

void Client::terminateConnectionCmd(QTextStream &args)
{
    std::cout << "Leaving room." << std::endl;
    m_voiceClient->leaveVoiceRoom(0);
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

void Client::connectionStatusConnected()
{
    std::cout << "CONN_STATUS_CONNECTED" << std::endl;
    printLinePrefix();
}

void Client::connectionStatusDisconnected()
{
    std::cout << "CONN_STATUS_DISCONNECTED" << std::endl;
    printLinePrefix();
}

