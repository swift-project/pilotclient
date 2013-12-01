/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "voiceclient_vatlib.h"

#include <QDebug>
#include <QTimer>

namespace BlackCore
{
    CVoiceClientVatlib::CVoiceClientVatlib(QObject *parent) :
        IVoiceClient(parent),
        m_voice(Create_Cvatlib_Voice_Simple()),
        m_inputSquelch(-1),
        m_queryUserRoomIndex(-1)
    {
        try
        {
            m_voice->Setup(true, 3290, 2, 1, onRoomStatusUpdate, this);
            m_voice->GetInputDevices(onInputHardwareDeviceReceived, this);
            m_voice->GetOutputDevices(onOutputHardwareDeviceReceived, this);

            // TODO: read audio device settings here and init with the same devices
            // If not settings are there or it is the first run, use the default one
            setInputDevice(defaultAudioInputDevice());
            setOutputDevice(defaultAudioOutputDevice());

            connect(this, &CVoiceClientVatlib::userJoinedLeft, this, &CVoiceClientVatlib::onUserJoinedLeft, Qt::QueuedConnection);
            connect(this, &CVoiceClientVatlib::connected, this, &CVoiceClientVatlib::onUserJoinedLeft, Qt::QueuedConnection);

            startTimer(100);
        }
        catch (...) { this->exceptionDispatcher(Q_FUNC_INFO); }
    }

    CVoiceClientVatlib::~CVoiceClientVatlib()
    {
    }

    void CVoiceClientVatlib::setCallsign(const BlackMisc::Aviation::CCallsign &callsign)
    {
        m_callsign = callsign;
    }

    void CVoiceClientVatlib::joinVoiceRoom(const int32_t comUnit, const BlackMisc::Voice::CVoiceRoom &voiceRoom)
    {
        if (!voiceRoom.isValid())
        {
            qDebug() << "Error: voiceRoom is invalid.";
        }

        Q_ASSERT_X (m_voice->IsValid() && m_voice->IsSetup(), "CVoiceClientVatlib", "Cvatlib_Voice_Simple invalid or not setup!");

        try
        {
            if (!m_voice->IsRoomValid(comUnit))
            {
                qDebug() << "Error: room index out of bounds";
                return;
            }

            QString serverSpec = voiceRoom.hostName() + "/" + voiceRoom.channel();

            m_voice->JoinRoom(comUnit, m_callsign.toQString().toLatin1().constData(), serverSpec.toLatin1().constData());
        }
        catch (...) { this->exceptionDispatcher(Q_FUNC_INFO); }
    }

    void CVoiceClientVatlib::leaveVoiceRoom(const int32_t comUnit)
    {
        try
        {
            if (!m_voice->IsRoomValid(comUnit))
            {
                qDebug() << "Error: room index out of bounds";
                return;
            }
            Q_ASSERT_X (m_voice->IsValid() && m_voice->IsSetup(), "CVoiceClientVatlib", "Cvatlib_Voice_Simple invalid or not setup!");
            m_voice->LeaveRoom(comUnit);
        }
        catch (...) { this->exceptionDispatcher(Q_FUNC_INFO); }
    }

    void CVoiceClientVatlib::setVolume(const int32_t comUnit, const uint32_t volumne)
    {

    }

    void CVoiceClientVatlib::startTransmitting(const int32_t comUnit)
    {
        try
        {
            if (!m_voice->IsRoomValid(comUnit))
            {
                qDebug() << "Error: room index out of bounds";
                return;
            }
            Q_ASSERT_X (m_voice->IsValid() && m_voice->IsSetup(), "CVoiceClientVatlib", "Cvatlib_Voice_Simple invalid or not setup!");
            m_voice->SetMicState(comUnit, true);
        }
        catch (...) { this->exceptionDispatcher(Q_FUNC_INFO); }
    }

    void CVoiceClientVatlib::stopTransmitting(const int32_t comUnit)
    {
        try
        {
            if (!m_voice->IsRoomValid(comUnit))
            {
                qDebug() << "Error: room index out of bounds";
                return;
            }
            Q_ASSERT_X (m_voice->IsValid() && m_voice->IsSetup(), "CVoiceClientVatlib", "Cvatlib_Voice_Simple invalid or not setup!");
            m_voice->SetMicState(comUnit, false);
        }
        catch (...) { this->exceptionDispatcher(Q_FUNC_INFO); }
    }

    bool CVoiceClientVatlib::isReceiving(const int32_t comUnit)
    {
        try
        {
            if (!m_voice->IsRoomValid(comUnit))
            {
                qDebug() << "Error: room index out of bounds";
                return false;
            }
            Q_ASSERT_X (m_voice->IsValid() && m_voice->IsSetup(), "CVoiceClientVatlib", "Cvatlib_Voice_Simple invalid or not setup!");
            m_voice->IsAudioPlaying(comUnit);
        }
        catch (...) { this->exceptionDispatcher(Q_FUNC_INFO); }

    }

    bool CVoiceClientVatlib::isConnected(const int32_t comUnit)
    {
        try
        {
            Q_ASSERT_X (m_voice->IsValid() && m_voice->IsSetup(), "CVoiceClientVatlib", "Cvatlib_Voice_Simple invalid or not setup!");
            m_voice->IsRoomConnected(comUnit);
        }
        catch (...) { this->exceptionDispatcher(Q_FUNC_INFO); }
    }

    const QStringList CVoiceClientVatlib::roomUserList(const int32_t comUnit)
    {
        /**/
        return m_voiceRoomsUsers.value(comUnit);
    }

    const QList<BlackMisc::Voice::CInputAudioDevice> &CVoiceClientVatlib::audioInputDevices() const
    {
        return m_inputDevices;
    }

    const QList<BlackMisc::Voice::COutputAudioDevice> &CVoiceClientVatlib::audioOutputDevices() const
    {
        return m_outputDevices;
    }

    const BlackMisc::Voice::CInputAudioDevice & CVoiceClientVatlib::defaultAudioInputDevice() const
    {
        foreach(BlackMisc::Voice::CInputAudioDevice inputDevice, m_inputDevices)
        {
            if(inputDevice.name().contains("[default]", Qt::CaseInsensitive))
                return inputDevice;
        }
        return BlackMisc::Voice::CInputAudioDevice();
    }

    const BlackMisc::Voice::COutputAudioDevice & CVoiceClientVatlib::defaultAudioOutputDevice() const
    {
        foreach(BlackMisc::Voice::COutputAudioDevice outputDevice, m_outputDevices)
        {
            if(outputDevice.name().contains("[default]", Qt::CaseInsensitive))
                return outputDevice;
        }
        return BlackMisc::Voice::COutputAudioDevice();
    }

    void CVoiceClientVatlib::setInputDevice(const BlackMisc::Voice::CInputAudioDevice &device)
    {
        if (!device.isValid())
            return;

        try
        {
            if( !m_voice->SetInputDevice(device.index()))
            {
                qWarning() << "SetInputDevice() failed";
            }
            if (!m_voice->IsInputDeviceAlive())
            {
                qWarning() << "Input device hit a fatal error";
            }
        }
        catch (...) { exceptionDispatcher(Q_FUNC_INFO); }
    }

    void CVoiceClientVatlib::enableAudio(const int32_t comUnit)
    {
        m_voice->SetOutoutState(comUnit, 0, true);
    }

    void CVoiceClientVatlib::setOutputDevice(const BlackMisc::Voice::COutputAudioDevice &device)
    {
        if (!device.isValid())
            return;

        try
        {
            m_voice->SetOutputDevice(0, device.index());

            if (!m_voice->IsOutputDeviceAlive(0))
            {
                qWarning() << "Input device hit a fatal error";
            }
        }
        catch (...) { exceptionDispatcher(Q_FUNC_INFO); }
    }

    void CVoiceClientVatlib::runSquelchTest()
    {
        try
        {
            Q_ASSERT_X (m_voice->IsValid() && m_voice->IsSetup(), "CVoiceClientVatlib", "Cvatlib_Voice_Simple invalid or not setup!");
            m_voice->BeginFindSquelch();
        }
        catch (...) { this->exceptionDispatcher(Q_FUNC_INFO); }

        // Start the timer only if no exception was thrown before
        QTimer::singleShot(5000, this, SLOT(onEndFindSquelch()));
    }

    void CVoiceClientVatlib::runMicTest()
    {
        try
        {
            Q_ASSERT_X (m_voice->IsValid() && m_voice->IsSetup(), "CVoiceClientVatlib", "Cvatlib_Voice_Simple invalid or not setup!");
            m_voice->BeginMicTest();
        }
        catch (...) { this->exceptionDispatcher(Q_FUNC_INFO); }

        // Start the timer only if no exception was thrown before
        QTimer::singleShot(5000, this, SLOT(onEndMicTest()));
    }

    float CVoiceClientVatlib::inputSquelch() const
    {
        return m_inputSquelch;
    }

    Cvatlib_Voice_Simple::agc CVoiceClientVatlib::micTestResult() const
    {
        return m_micTestResult;
    }

    const BlackMisc::Voice::CVoiceRoom &CVoiceClientVatlib::voiceRoom(const uint32_t comUnit)
    {

    }

    void CVoiceClientVatlib::timerEvent(QTimerEvent *)
    {
        try
        {
            Q_ASSERT_X (m_voice->IsValid() && m_voice->IsSetup(), "CVoiceClientVatlib", "Cvatlib_Voice_Simple invalid or not setup!");
            m_voice->DoProcessing();
        }
        catch (...) { this->exceptionDispatcher(Q_FUNC_INFO); }
    }

    void CVoiceClientVatlib::onEndFindSquelch()
    {
        try
        {
            Q_ASSERT_X (m_voice->IsValid() && m_voice->IsSetup(), "CVoiceClientVatlib", "Cvatlib_Voice_Simple invalid or not setup!");

            m_voice->EndFindSquelch();

            //TODO: store captured squelch
            m_inputSquelch = m_voice->GetInputSquelch();
            emit squelchTestFinished();
        }
        catch (...) { this->exceptionDispatcher(Q_FUNC_INFO); }
    }

    void CVoiceClientVatlib::onEndMicTest()
    {
        try
        {
           Q_ASSERT_X (m_voice->IsValid() && m_voice->IsSetup(), "CVoiceClientVatlib", "Cvatlib_Voice_Simple invalid or not setup!");

            m_micTestResult = m_voice->EndMicTest();
            emit micTestFinished();
        }
        catch (...) { this->exceptionDispatcher(Q_FUNC_INFO); }
    }

    void CVoiceClientVatlib::onUserJoinedLeft(const int32_t roomIndex)
    {
        try
        {
            Q_ASSERT_X (m_voice->IsValid() && m_voice->IsSetup(), "CVoiceClientVatlib", "Cvatlib_Voice_Simple invalid or not setup!");
            Q_ASSERT_X (m_queryUserRoomIndex == -1, "CVoiceClientVatlib::onUserJoinedLeft", "Cannot list users for two rooms in parallel!");

            if (!m_voice->IsRoomValid(roomIndex))
            {
                qDebug() << "Error: room index out of bounds";
                return;
            }
            if (!m_voice->IsRoomConnected(roomIndex))
                return;

            // Store the room index for the slot.
            m_queryUserRoomIndex = roomIndex;
            m_voice->GetRoomUserList(roomIndex, onRoomUserReceived, this);
            m_queryUserRoomIndex = -1;
        }
        catch (...) { this->exceptionDispatcher(Q_FUNC_INFO); }
    }

    /********************************** * * * * * * * * * * * * * * * * * * * ************************************/
    /**********************************           shimlib callbacks           ************************************/
    /********************************** * * * * * * * * * * * * * * * * * * * ************************************/

    CVoiceClientVatlib *cbvar_cast(void *cbvar)
    {
        return static_cast<CVoiceClientVatlib *>(cbvar);
    }

    void CVoiceClientVatlib::onRoomStatusUpdate(Cvatlib_Voice_Simple *obj, Cvatlib_Voice_Simple::roomStatusUpdate upd, int32_t roomIndex, void *cbVar)
    {
        cbvar_cast(cbVar)->m_voiceRoomsStatus[roomIndex] = upd;
        switch (upd)
        {
        case Cvatlib_Voice_Simple::roomStatusUpdate_JoinSuccess:
            cbvar_cast(cbVar)->enableAudio(roomIndex);
            emit cbvar_cast(cbVar)->connected(roomIndex);
            break;
        case Cvatlib_Voice_Simple::roomStatusUpdate_JoinFail:
            emit cbvar_cast(cbVar)->connectionFailed(roomIndex);
            break;
        case Cvatlib_Voice_Simple::roomStatusUpdate_UnexpectedDisconnectOrKicked:
            emit cbvar_cast(cbVar)->kicked(roomIndex);
            break;
        case Cvatlib_Voice_Simple::roomStatusUpdate_LeaveComplete:
            emit cbvar_cast(cbVar)->disconnected(roomIndex);
            break;
        case Cvatlib_Voice_Simple::roomStatusUpdate_UserJoinsLeaves:
            // FIXME: We cannot call GetRoomUserList because vatlib is reentrent safe.
            emit cbvar_cast(cbVar)->userJoinedLeft (roomIndex);
            break;
        case Cvatlib_Voice_Simple::roomStatusUpdate_RoomAudioStarted:
            emit cbvar_cast(cbVar)->audioStarted (roomIndex);
            break;
        case Cvatlib_Voice_Simple::roomStatusUpdate_RoomAudioStopped:
            emit cbvar_cast(cbVar)->audioStopped (roomIndex);
            break;
        case Cvatlib_Voice_Simple::roomStatusUpdate_AudioStarted:
            emit cbvar_cast(cbVar)->audioStarted();
            break;
        case Cvatlib_Voice_Simple::roomStatusUpdate_AudioStopped:
            emit cbvar_cast(cbVar)->audioStopped();
            break;
        default:
            break;
        }
    }

    void CVoiceClientVatlib::onRoomUserReceived(Cvatlib_Voice_Simple *obj, const char *name, void *cbVar)
    {
        CVoiceClientVatlib *voiceClientVatlib = cbvar_cast(cbVar);
        // This method retrieves first of all a list of the known users
        QStringList users = voiceClientVatlib->roomUserList(voiceClientVatlib->queryUserRoomIndex());
        QString callsign = QString(name);
        if (callsign.isEmpty())
            return;

        // if the user is already known, the user had left
        // else, the user joined.
        if (users.contains(callsign))
        {
            voiceClientVatlib->removeUserFromRoom(voiceClientVatlib->queryUserRoomIndex(), callsign);
        }
        else
        {
            voiceClientVatlib->addUserInRoom(voiceClientVatlib->queryUserRoomIndex(), callsign);
        }
    }

    void CVoiceClientVatlib::onInputHardwareDeviceReceived(Cvatlib_Voice_Simple *obj, const char *name, void *cbVar)
    {
        BlackMisc::Voice::CInputAudioDevice inputDevice(cbvar_cast(cbVar)->m_inputDevices.size(), QString(name));
        cbvar_cast(cbVar)->m_inputDevices.append(inputDevice);
    }

    void CVoiceClientVatlib::onOutputHardwareDeviceReceived(Cvatlib_Voice_Simple *obj, const char *name, void *cbVar)
    {
        BlackMisc::Voice::COutputAudioDevice outputDevice(cbvar_cast(cbVar)->m_outputDevices.size(), QString(name));
        cbvar_cast(cbVar)->m_outputDevices.append(outputDevice);
    }

    void CVoiceClientVatlib::addUserInRoom(const int32_t roomIndex, const QString &callsign)
    {
        m_voiceRoomsUsers[roomIndex].append(callsign);
        bool test = m_voice->IsAudioPlaying(roomIndex);
        emit userJoinedRoom(callsign);
    }

    void CVoiceClientVatlib::removeUserFromRoom(const int32_t roomIndex, const QString &callsign)
    {
        m_voiceRoomsUsers[roomIndex].removeAll(callsign);
        emit userLeftRoom(callsign);
    }

    void CVoiceClientVatlib::exceptionDispatcher(const char *caller)
    {
        QString msg("Caller: ");
        msg.append(caller).append(" ").append("Exception: ");
        try
        {
            throw;
        }
        catch (const NetworkNotConnectedException &e)
        {
            // this could be caused by a race condition during normal operation, so not an error
            msg.append("NetworkNotConnectedException").append(" ").append(e.what());
            emit this->exception(msg);
            qDebug() << "NetworkNotConnectedException caught in " << caller << "\n" << e.what();
        }
        catch (const VatlibException &e)
        {
            msg.append("VatlibException").append(" ").append(e.what());
            emit this->exception(msg, true);
            qFatal("VatlibException caught in %s\n%s", caller, e.what());
        }
        catch (const std::exception &e)
        {
            msg.append("std::exception").append(" ").append(e.what());
            emit this->exception(msg, true);
            qFatal("std::exception caught in %s\n%s", caller, e.what());
        }
        catch (...)
        {
            msg.append("unknown exception");
            emit this->exception(msg, true);
            qFatal("Unknown exception caught in %s", caller);
        }
    }

} // namespace BlackCore
