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
        m_voice(Cvatlib_Voice_Simple::Create()),
        m_inputSquelch(-1),
        m_micTestResult(Cvatlib_Voice_Simple::agc_Ok),
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

    const QSet<QString> CVoiceClientVatlib::roomUserList(const ComUnit comUnit)
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

    const BlackMisc::Voice::CInputAudioDevice CVoiceClientVatlib::defaultAudioInputDevice() const
    {
        // Constructor creates already a default device
        return BlackMisc::Voice::CInputAudioDevice(BlackMisc::Voice::CInputAudioDevice::defaultDevice(), "default");
    }

    const BlackMisc::Voice::COutputAudioDevice CVoiceClientVatlib::defaultAudioOutputDevice() const
    {
        // Constructor creates already a default device
        return BlackMisc::Voice::COutputAudioDevice(BlackMisc::Voice::COutputAudioDevice::defaultDevice(), "default");
    }

    void CVoiceClientVatlib::setInputDevice(const BlackMisc::Voice::CInputAudioDevice &device)
    {
        Q_ASSERT_X (m_voice->IsValid() && m_voice->IsSetup(), "CVoiceClientVatlib", "Cvatlib_Voice_Simple invalid or not setup!");
        if (!device.isValid())
        {
            qWarning() << "Cannot set invalid input device!";
            return;
        }

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

    void CVoiceClientVatlib::setOutputDevice(const BlackMisc::Voice::COutputAudioDevice &device)
    {
        Q_ASSERT_X (m_voice->IsValid() && m_voice->IsSetup(), "CVoiceClientVatlib", "Cvatlib_Voice_Simple invalid or not setup!");
        if (!device.isValid())
        {
            qWarning() << "Cannot set invalid output device!";
            return;
        }

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

    void CVoiceClientVatlib::enableAudio(const ComUnit comUnit)
    {
        Q_ASSERT_X (m_voice->IsValid() && m_voice->IsSetup(), "CVoiceClientVatlib", "Cvatlib_Voice_Simple invalid or not setup!");
        Q_ASSERT_X (m_voice->IsRoomValid(static_cast<int32_t>(comUnit)), "CVoiceClientVatlib", "Room index out of bounds!");
        try
        {
            m_voice->SetOutoutState(static_cast<int32_t>(comUnit), 0, true);
        }
        catch (...) { this->exceptionDispatcher(Q_FUNC_INFO); }
    }

    const BlackMisc::Voice::CVoiceRoom CVoiceClientVatlib::voiceRoom(const ComUnit comUnit)
    {
        return m_voiceRooms.value(comUnit);
    }

    bool CVoiceClientVatlib::isConnected(const ComUnit comUnit)
    {
        bool result;
        Q_ASSERT_X (m_voice->IsValid() && m_voice->IsSetup(), "CVoiceClientVatlib", "Cvatlib_Voice_Simple invalid or not setup!");
        Q_ASSERT_X (m_voice->IsRoomValid(static_cast<int32_t>(comUnit)), "CVoiceClientVatlib", "Room index out of bounds!");
        try
        {
            result = m_voice->IsRoomConnected(static_cast<int32_t>(comUnit));
        }
        catch (...) { this->exceptionDispatcher(Q_FUNC_INFO); }

        return result;
    }

    bool CVoiceClientVatlib::isReceiving(const ComUnit comUnit)
    {
        bool receiving;
        Q_ASSERT_X (m_voice->IsValid() && m_voice->IsSetup(), "CVoiceClientVatlib", "Cvatlib_Voice_Simple invalid or not setup!");
        Q_ASSERT_X (m_voice->IsRoomValid(static_cast<int32_t>(comUnit)), "CVoiceClientVatlib", "Room index out of bounds!");
        try
        {
            receiving = m_voice->IsAudioPlaying(static_cast<int32_t>(comUnit));
        }
        catch (...) { this->exceptionDispatcher(Q_FUNC_INFO); }

        return receiving;
    }

    void CVoiceClientVatlib::runSquelchTest()
    {
        Q_ASSERT_X (m_voice->IsValid() && m_voice->IsSetup(), "CVoiceClientVatlib", "Cvatlib_Voice_Simple invalid or not setup!");

        try
        {
            m_voice->BeginFindSquelch();
        }
        catch (...) { this->exceptionDispatcher(Q_FUNC_INFO); }

        // Start the timer only if no exception was thrown before
        QTimer::singleShot(5000, this, SLOT(onEndFindSquelch()));
    }

    void CVoiceClientVatlib::runMicTest()
    {
        Q_ASSERT_X (m_voice->IsValid() && m_voice->IsSetup(), "CVoiceClientVatlib", "Cvatlib_Voice_Simple invalid or not setup!");

        try
        {
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

    int32_t CVoiceClientVatlib::micTestResult() const
    {
        return m_micTestResult;
    }

    QString CVoiceClientVatlib::micTestResultAsString() const
    {
        QString result;
        switch (m_micTestResult)
        {
        case Cvatlib_Voice_Simple::agc_Ok:
            result = "The test went ok";
            break;
        case Cvatlib_Voice_Simple::agc_BkgndNoiseLoud:
            result = "The overall background noise is very loud and may be a nuisance to others";
            break;
        case Cvatlib_Voice_Simple::agc_TalkDrownedOut:
            result = "The overall background noise is loud enough that others probably wont be able to distinguish speech from it";
            break;
        case Cvatlib_Voice_Simple::agc_TalkMicHot:
            result = "The overall mic volume is too hot, you should lower the volume in the windows volume control panel";
            break;
        case Cvatlib_Voice_Simple::agc_TalkMicCold:
            result = "The overall mic volume is too cold, you should raise the volume in the windows control panel and enable mic boost if needed";
            break;
        default:
            result = "Unknown result.";
            break;
        }

        return result;
    }

    void CVoiceClientVatlib::setCallsign(const BlackMisc::Aviation::CCallsign &callsign)
    {
        m_callsign = callsign;
    }

    void CVoiceClientVatlib::joinVoiceRoom(const ComUnit comUnit, const BlackMisc::Voice::CVoiceRoom &voiceRoom)
    {
        Q_ASSERT_X (m_voice->IsValid() && m_voice->IsSetup(), "CVoiceClientVatlib", "Cvatlib_Voice_Simple invalid or not setup!");
        Q_ASSERT_X (m_voice->IsRoomValid(static_cast<int32_t>(comUnit)), "CVoiceClientVatlib", "Room index out of bounds!");

        if (!voiceRoom.isValid())
        {
            qDebug() << "Error: Cannot join invalid voice room.";
            return;
        }

        try
        {
            QString serverSpec = voiceRoom.hostName() + "/" + voiceRoom.channel();

            m_voice->JoinRoom(static_cast<int32_t>(comUnit), m_callsign.toQString().toLatin1().constData(), serverSpec.toLatin1().constData());
            m_voiceRooms[comUnit] = voiceRoom;
        }
        catch (...) { this->exceptionDispatcher(Q_FUNC_INFO); }
    }

    void CVoiceClientVatlib::leaveVoiceRoom(const ComUnit comUnit)
    {
        Q_ASSERT_X (m_voice->IsValid() && m_voice->IsSetup(), "CVoiceClientVatlib", "Cvatlib_Voice_Simple invalid or not setup!");
        Q_ASSERT_X (m_voice->IsRoomValid(static_cast<int32_t>(comUnit)), "CVoiceClientVatlib", "Room index out of bounds!");

        try
        {
            m_voice->LeaveRoom(static_cast<int32_t>(comUnit));
            m_voiceRooms.remove(comUnit);
        }
        catch (...) { this->exceptionDispatcher(Q_FUNC_INFO); }
    }

    void CVoiceClientVatlib::setVolume(const ComUnit comUnit, const int32_t volumne)
    {
        Q_ASSERT_X (m_voice->IsValid() && m_voice->IsSetup(), "CVoiceClientVatlib", "Cvatlib_Voice_Simple invalid or not setup!");
        Q_ASSERT_X (m_voice->IsRoomValid(static_cast<int32_t>(comUnit)), "CVoiceClientVatlib", "Room index out of bounds!");

        try
        {
            m_voice->SetRoomVolume(static_cast<int32_t>(comUnit), volumne);
        }
        catch (...) { this->exceptionDispatcher(Q_FUNC_INFO); }
    }

    void CVoiceClientVatlib::startTransmitting(const ComUnit comUnit)
    {
        Q_ASSERT_X (m_voice->IsValid() && m_voice->IsSetup(), "CVoiceClientVatlib", "Cvatlib_Voice_Simple invalid or not setup!");
        Q_ASSERT_X (m_voice->IsRoomValid(static_cast<int32_t>(comUnit)), "CVoiceClientVatlib", "Room index out of bounds!");

        try
        {
            m_voice->SetMicState(static_cast<int32_t>(comUnit), true);
        }
        catch (...) { this->exceptionDispatcher(Q_FUNC_INFO); }
    }

    void CVoiceClientVatlib::stopTransmitting(const ComUnit comUnit)
    {
        Q_ASSERT_X (m_voice->IsValid() && m_voice->IsSetup(), "CVoiceClientVatlib", "Cvatlib_Voice_Simple invalid or not setup!");
        Q_ASSERT_X (m_voice->IsRoomValid(static_cast<int32_t>(comUnit)), "CVoiceClientVatlib", "Room index out of bounds!");
        try
        {
            m_voice->SetMicState(static_cast<int32_t>(comUnit), false);
        }
        catch (...) { this->exceptionDispatcher(Q_FUNC_INFO); }
    }

    void CVoiceClientVatlib::changeRoomStatus(ComUnit comUnit, Cvatlib_Voice_Simple::roomStatusUpdate upd)
    {
        m_voiceRoomsStatus[comUnit] = upd;
        switch (upd)
        {
        case Cvatlib_Voice_Simple::roomStatusUpdate_JoinSuccess:
            enableAudio(comUnit);
            emit connected(comUnit);
            break;
        case Cvatlib_Voice_Simple::roomStatusUpdate_JoinFail:
            emit connectionFailed(comUnit);
            break;
        case Cvatlib_Voice_Simple::roomStatusUpdate_UnexpectedDisconnectOrKicked:
            emit kicked(comUnit);
            break;
        case Cvatlib_Voice_Simple::roomStatusUpdate_LeaveComplete:
            m_voiceRoomsUsers.clear();
            emit disconnected(comUnit);
            break;
        case Cvatlib_Voice_Simple::roomStatusUpdate_UserJoinsLeaves:
            // FIXME: We cannot call GetRoomUserList because vatlib is reentrent safe.
            emit userJoinedLeft (comUnit);
            break;
        case Cvatlib_Voice_Simple::roomStatusUpdate_RoomAudioStarted:
            emit audioStarted (comUnit);
            break;
        case Cvatlib_Voice_Simple::roomStatusUpdate_RoomAudioStopped:
            emit audioStopped (comUnit);
            break;
        case Cvatlib_Voice_Simple::roomStatusUpdate_AudioStarted:
            emit globalAudioStarted();
            break;
        case Cvatlib_Voice_Simple::roomStatusUpdate_AudioStopped:
            emit globalAudioStopped();
            break;
        default:
            break;
        }
    }

    void CVoiceClientVatlib::timerEvent(QTimerEvent *)
    {
        Q_ASSERT_X (m_voice->IsValid() && m_voice->IsSetup(), "CVoiceClientVatlib", "Cvatlib_Voice_Simple invalid or not setup!");

        try
        {
            m_voice->DoProcessing();
        }
        catch (...) { this->exceptionDispatcher(Q_FUNC_INFO); }
    }

    void CVoiceClientVatlib::onEndFindSquelch()
    {
        Q_ASSERT_X (m_voice->IsValid() && m_voice->IsSetup(), "CVoiceClientVatlib", "Cvatlib_Voice_Simple invalid or not setup!");

        try
        {
            m_voice->EndFindSquelch();
            m_inputSquelch = m_voice->GetInputSquelch();
            emit squelchTestFinished();
        }
        catch (...) { this->exceptionDispatcher(Q_FUNC_INFO); }
    }

    void CVoiceClientVatlib::onEndMicTest()
    {
        Q_ASSERT_X (m_voice->IsValid() && m_voice->IsSetup(), "CVoiceClientVatlib", "Cvatlib_Voice_Simple invalid or not setup!");

        try
        {
            m_micTestResult = m_voice->EndMicTest();
            emit micTestFinished();
        }
        catch (...) { this->exceptionDispatcher(Q_FUNC_INFO); }
    }

    void CVoiceClientVatlib::onUserJoinedLeft(const ComUnit comUnit)
    {
        Q_ASSERT_X (m_voice->IsValid() && m_voice->IsSetup(), "CVoiceClientVatlib", "Cvatlib_Voice_Simple invalid or not setup!");
        Q_ASSERT_X (m_queryUserRoomIndex == -1, "CVoiceClientVatlib::onUserJoinedLeft", "Cannot list users for two rooms in parallel!");
        try
        {
            // Paranoia...
            if (!m_voice->IsRoomConnected(static_cast<int32_t>(comUnit)))
                return;

            // Store the room index for the slot.
            m_queryUserRoomIndex = static_cast<int32_t>(comUnit);
            m_voice->GetRoomUserList(static_cast<int32_t>(comUnit), onRoomUserReceived, this);
            m_queryUserRoomIndex = -1;

            QSet<QString> temporaryUsers;

            foreach (QString callsign, m_voiceRoomsUsers.value(comUnit))
            {
                if (m_voiceRoomUsersUpdate.contains(callsign))
                {
                    // The user is still there.
                    temporaryUsers.insert(callsign);
                }
                else
                {
                    // He is has probably left
                    emit userLeftRoom(callsign);
                }
            }

            foreach (QString callsign, m_voiceRoomUsersUpdate)
            {
                if (m_voiceRoomsUsers.value(comUnit).contains(callsign))
                {
                    // User was already there before
                    temporaryUsers.insert(callsign);
                }
                else
                {
                    // He joined
                    temporaryUsers.insert(callsign);
                    emit userJoinedRoom(callsign);
                }
            }

            // Finally we update it with our new list
            m_voiceRoomsUsers[comUnit] = temporaryUsers;
            m_voiceRoomUsersUpdate.clear();
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
        Q_UNUSED(obj)
        ComUnit comUnit = static_cast<ComUnit>(roomIndex);
        CVoiceClientVatlib *voiceClientVatlib = cbvar_cast(cbVar);
        voiceClientVatlib->changeRoomStatus(comUnit, upd);
    }

    void CVoiceClientVatlib::onRoomUserReceived(Cvatlib_Voice_Simple *obj, const char *name, void *cbVar)
    {
        Q_UNUSED(obj)
        CVoiceClientVatlib *voiceClientVatlib = cbvar_cast(cbVar);
        ComUnit comUnit = static_cast<ComUnit>(voiceClientVatlib->queryUserRoomIndex());
        // This method retrieves first of all a list of the known users
        QSet<QString> users = voiceClientVatlib->roomUserList(comUnit);
        QString callsign = QString(name);
        if (callsign.isEmpty())
            return;

        voiceClientVatlib->addUserInRoom(comUnit, callsign);
    }

    void CVoiceClientVatlib::onInputHardwareDeviceReceived(Cvatlib_Voice_Simple *obj, const char *name, void *cbVar)
    {
        Q_UNUSED(obj)
        BlackMisc::Voice::CInputAudioDevice inputDevice(cbvar_cast(cbVar)->m_inputDevices.size(), QString(name));
        cbvar_cast(cbVar)->m_inputDevices.append(inputDevice);
    }

    void CVoiceClientVatlib::onOutputHardwareDeviceReceived(Cvatlib_Voice_Simple *obj, const char *name, void *cbVar)
    {
        Q_UNUSED(obj)
        BlackMisc::Voice::COutputAudioDevice outputDevice(cbvar_cast(cbVar)->m_outputDevices.size(), QString(name));
        cbvar_cast(cbVar)->m_outputDevices.append(outputDevice);
    }

    void CVoiceClientVatlib::addUserInRoom(const ComUnit comUnit, const QString &callsign)
    {
        m_voiceRoomUsersUpdate.insert(callsign);
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
