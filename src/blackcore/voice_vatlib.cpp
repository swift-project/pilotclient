/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "voice_vatlib.h"
#include <QDebug>
#include <QTimer>

using namespace BlackMisc;
using namespace BlackMisc::Audio;
using namespace BlackMisc::Aviation;

namespace BlackCore
{
    /*
     * Constructor
     */
    CVoiceVatlib::CVoiceVatlib(QObject *parent) :
        IVoice(parent),
        m_voice(nullptr),
        m_audioOutput(nullptr),
        m_inputSquelch(-1),
        m_micTestResult(Cvatlib_Voice_Simple::agc_Ok),
        m_isAudioLoopbackEnabled(false),
        m_temporaryUserRoomIndex(CVoiceVatlib::InvalidRoomIndex),
        m_lockVoiceRooms(QReadWriteLock::Recursive),
        m_lockCallsigns(QReadWriteLock::Recursive),
        m_lockCurrentOutputDevice(QReadWriteLock::Recursive),
        m_lockCurrentInputDevice(QReadWriteLock::Recursive),
        m_lockDeviceList(QReadWriteLock::Recursive),
        m_lockOutputEnabled(QReadWriteLock::Recursive),
        m_lockSquelch(QReadWriteLock::Recursive),
        m_lockTestResult(QReadWriteLock::Recursive),
        m_lockMyCallsign(QReadWriteLock::Recursive),
        m_lockConnectionStatus(QReadWriteLock::Recursive),
        m_mutexVatlib(QMutex::Recursive)
    {
        try
        {
            // we use reset here until issue #277 is resolved
            // easier to find root cause
            m_audioOutput.reset(new QAudioOutput());
            m_voice.reset(Cvatlib_Voice_Simple::Create());
            m_voice->Setup(true, 3290, 2, 1, onRoomStatusUpdate, this);
            m_voice->GetInputDevices(onInputHardwareDeviceReceived, this);
            m_voice->GetOutputDevices(onOutputHardwareDeviceReceived, this);

            connect(this, &CVoiceVatlib::userJoinedLeft, this, &CVoiceVatlib::onUserJoinedLeft, Qt::QueuedConnection);

            this->m_voiceRooms.push_back(CVoiceRoom()); // COM1
            this->m_voiceRooms.push_back(CVoiceRoom()); // COM2
            this->m_outputEnabled.insert(COM1, true);
            this->m_outputEnabled.insert(COM2, true);
            this->m_currentInputDevice = this->defaultAudioInputDevice();
            this->m_currentOutputDevice = this->defaultAudioOutputDevice();
            this->m_audioOutput->setVolume(1.0); // make sure the overall sound is not muted

            // do processing
            this->startTimer(10);
        }
        catch (...)
        {
            this->exceptionDispatcher(Q_FUNC_INFO);
        }
    }

    /*
     * Destructor
     */
    CVoiceVatlib::~CVoiceVatlib() {}

    /*
     * Devices
     */
    const BlackMisc::Audio::CAudioDeviceList &CVoiceVatlib::audioDevices() const
    {
        QReadLocker lockForReading(&m_lockDeviceList);
        return m_devices;
    }

    /*
     * Default input device
     */
    const BlackMisc::Audio::CAudioDevice CVoiceVatlib::defaultAudioInputDevice() const
    {
        // Constructor creates already a default device
        return BlackMisc::Audio::CAudioDevice(BlackMisc::Audio::CAudioDevice::InputDevice, BlackMisc::Audio::CAudioDevice::defaultDeviceIndex(), "default");
    }

    /*
     * Default output device
     */
    const BlackMisc::Audio::CAudioDevice CVoiceVatlib::defaultAudioOutputDevice() const
    {
        // Constructor creates already a default device
        return BlackMisc::Audio::CAudioDevice(BlackMisc::Audio::CAudioDevice::OutputDevice, BlackMisc::Audio::CAudioDevice::defaultDeviceIndex(), "default");
    }

    /*
     * Current output device
     */
    CAudioDevice CVoiceVatlib::getCurrentOutputDevice() const
    {
        QReadLocker lockForReading(&m_lockCurrentOutputDevice);
        return m_currentOutputDevice;
    }

    /*
     * Current input device
     */
    CAudioDevice CVoiceVatlib::getCurrentInputDevice() const
    {
        QReadLocker lockForReading(&m_lockCurrentInputDevice);
        return m_currentInputDevice;
    }

    /*
     * Set input device
     */
    void CVoiceVatlib::setInputDevice(const BlackMisc::Audio::CAudioDevice &device)
    {
        QMutexLocker lockerVatlib(&m_mutexVatlib);
        Q_ASSERT_X(m_voice->IsValid() && m_voice->IsSetup(), "CVoiceVatlib", "Cvatlib_Voice_Simple invalid or not setup!");
        if (!device.isValid())
        {
            qWarning() << "Cannot set invalid input device!";
            return;
        }

        try
        {
            if (!m_voice->SetInputDevice(device.getIndex()))
            {
                qWarning() << "Setting input device failed";
            }
            if (!m_voice->IsInputDeviceAlive())
            {
                qWarning() << "Input device hit a fatal error";
            }
            QWriteLocker lockForWriting(&m_lockCurrentInputDevice);
            this->m_currentInputDevice = device;
        }
        catch (...)
        {
            exceptionDispatcher(Q_FUNC_INFO);
        }
    }

    /*
     * Set output device
     */
    void CVoiceVatlib::setOutputDevice(const BlackMisc::Audio::CAudioDevice &device)
    {
        QMutexLocker lockerVatlib(&m_mutexVatlib);
        Q_ASSERT_X(m_voice->IsValid() && m_voice->IsSetup(), "CVoiceVatlib", "Cvatlib_Voice_Simple invalid or not setup!");
        if (!device.isValid())
        {
            qWarning() << "Cannot set invalid output device!";
            return;
        }

        try
        {
            // there is no return value here: https://dev.vatsim-germany.org/issues/115
            m_voice->SetOutputDevice(0, device.getIndex());
            if (!m_voice->IsOutputDeviceAlive(0))
            {
                qWarning() << "Output device hit a fatal error";
            }
            QWriteLocker lockForWriting(&m_lockCurrentOutputDevice);
            this->m_currentOutputDevice = device;
        }
        catch (...)
        {
            exceptionDispatcher(Q_FUNC_INFO);
        }
    }

    /*
     * Get voice rooms, with the latest status updated
     */
    BlackMisc::Audio::CVoiceRoomList CVoiceVatlib::getComVoiceRoomsWithAudioStatus() const
    {
        QReadLocker lockForReading(&m_lockVoiceRooms);
        Q_ASSERT_X(m_voiceRooms.size() == 2, "CVoiceVatlib", "Wrong numer of COM voice rooms");

        if (!m_voice->IsValid() || !m_voice->IsSetup()) return CVoiceRoomList::twoEmptyRooms();

        QMutexLocker lockerVatlib(&m_mutexVatlib);
        // valid state, update
        CVoiceRoom com1 = this->m_voiceRooms[0];
        CVoiceRoom com2 = this->m_voiceRooms[1];
        com1.setConnected(m_voice->IsRoomConnected(static_cast<qint32>(COM1)));
        com2.setConnected(m_voice->IsRoomConnected(static_cast<qint32>(COM2)));
        com1.setAudioPlaying(com1.isConnected() ? m_voice->IsAudioPlaying(static_cast<qint32>(COM1)) : false);
        com2.setAudioPlaying(com2.isConnected() ? m_voice->IsAudioPlaying(static_cast<qint32>(COM2)) : false);
        CVoiceRoomList voiceRooms;
        voiceRooms.push_back(com1);
        voiceRooms.push_back(com2);
        return voiceRooms;
    }

    /*
     * Voice room callsigns
     */
    CCallsignList CVoiceVatlib::getVoiceRoomCallsigns(const IVoice::ComUnit comUnit) const
    {
        QReadLocker lockForReading(&m_lockCallsigns);
        CCallsignList callsigns;
        if (!this->m_voiceRoomCallsigns.contains(comUnit)) return callsigns;
        return this->m_voiceRoomCallsigns[comUnit];
    }

    /*
     * Enable audio
     */
    void CVoiceVatlib::switchAudioOutput(const ComUnit comUnit, bool enable)
    {
        QMutexLocker lockerVatlib(&m_mutexVatlib);
        Q_ASSERT_X(m_voice->IsValid() && m_voice->IsSetup(), "CVoiceVatlib", "Cvatlib_Voice_Simple invalid or not setup!");
        Q_ASSERT_X(m_voice->IsRoomValid(static_cast<qint32>(comUnit)), "CVoiceVatlib", "Room index out of bounds!");
        try
        {
            m_voice->SetOutputState(static_cast<qint32>(comUnit), 0, enable);
            QWriteLocker lockForWriting(&m_lockOutputEnabled);
            this->m_outputEnabled[comUnit] = enable;
        }
        catch (...)
        {
            this->exceptionDispatcher(Q_FUNC_INFO);
        }
    }

    void CVoiceVatlib::enableAudioLoopback(bool enable)
    {
        if (enable == m_isAudioLoopbackEnabled)
            return;

        QMutexLocker lockerVatlib(&m_mutexVatlib);
        Q_ASSERT_X(m_voice->IsValid() && m_voice->IsSetup(), "CVoiceVatlib", "Cvatlib_Voice_Simple invalid or not setup!");
        try
        {
            m_voice->SetAudioLoopback(0, enable);
        }
        catch (...)
        {
            this->exceptionDispatcher(Q_FUNC_INFO);
        }

        // bools are atomic. No need to protect it with a mutex
        m_isAudioLoopbackEnabled = enable;
    }

    /*
     * Squelch test
     */
    void CVoiceVatlib::runSquelchTest()
    {
        QMutexLocker lockerVatlib(&m_mutexVatlib);
        Q_ASSERT_X(m_voice->IsValid() && m_voice->IsSetup(), "CVoiceVatlib", "Cvatlib_Voice_Simple invalid or not setup!");

        try
        {
            m_voice->BeginFindSquelch();
        }
        catch (...)
        {
            this->exceptionDispatcher(Q_FUNC_INFO);
        }

        // Start the timer only if no exception was thrown before
        QTimer::singleShot(5000, this, SLOT(onEndFindSquelch()));
    }

    /*
     * Start microphone test
     */
    void CVoiceVatlib::runMicrophoneTest()
    {
        QMutexLocker lockerVatlib(&m_mutexVatlib);
        Q_ASSERT_X(m_voice->IsValid() && m_voice->IsSetup(), "CVoiceVatlib", "Cvatlib_Voice_Simple invalid or not setup!");

        try
        {
            m_voice->BeginMicTest();
            // Start the timer only if no exception was thrown before
            QTimer::singleShot(5000, this, SLOT(onEndMicTest()));
        }
        catch (...)
        {
            this->exceptionDispatcher(Q_FUNC_INFO);
        }
    }

    /*
     * Input squelch volume
     */
    float CVoiceVatlib::inputSquelch() const
    {
        QReadLocker lockForReading(&m_lockSquelch);
        return m_inputSquelch;
    }

    /*
     * Mic test
     */
    qint32 CVoiceVatlib::micTestResult() const
    {
        QReadLocker lockForReading(&m_lockTestResult);
        return m_micTestResult;
    }

    /*
     * Mic test, result as string
     */
    QString CVoiceVatlib::micTestResultAsString() const
    {
        QString result;

        QReadLocker lockForReading(&m_lockTestResult);
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

    /*
     * Callsign
     */
    void CVoiceVatlib::setMyAircraftCallsign(const BlackMisc::Aviation::CCallsign &callsign)
    {
        QWriteLocker lockForWriting(&m_lockMyCallsign);
        m_aircraftCallsign = callsign;
    }

    /*
     * Voice room
     */
    void CVoiceVatlib::joinVoiceRoom(const ComUnit comUnit, const BlackMisc::Audio::CVoiceRoom &voiceRoom)
    {
        QMutexLocker lockerVatlib(&m_mutexVatlib);
        Q_ASSERT_X(m_voice->IsValid() && m_voice->IsSetup(), "CVoiceVatlib", "Cvatlib_Voice_Simple invalid or not setup!");
        Q_ASSERT_X(m_voice->IsRoomValid(static_cast<qint32>(comUnit)), "CVoiceVatlib", "Room index out of bounds!");
        if (!voiceRoom.isValid())
        {
            qDebug() << "Error: Cannot join invalid voice room.";
            return;
        }

        try
        {
            // only connect, if not yet connected
            if (this->m_voice->IsRoomConnected(static_cast<qint32>(comUnit))) return;
            changeConnectionStatus(comUnit, Connecting);
            QString serverSpec = voiceRoom.getVoiceRoomUrl();
            QReadLocker lockForReading(&m_lockMyCallsign);
            bool jr = m_voice->JoinRoom(static_cast<qint32>(comUnit), m_aircraftCallsign.toQString().toLatin1().constData(), serverSpec.toLatin1().constData());
            if (jr)
            {
                // do not(!) set as connected right now, this will be done in "status changed"
                // when room really is connected
                this->setVoiceRoomForUnit(comUnit, voiceRoom);
            }
            else
            {
                qWarning("Could not join voice room");
            }
        }
        catch (...)
        {
            this->exceptionDispatcher(Q_FUNC_INFO);
        }
    }

    /*
     * Leave a room
     */
    void CVoiceVatlib::leaveVoiceRoom(const ComUnit comUnit)
    {
        QMutexLocker lockerVatlib(&m_mutexVatlib);

        Q_ASSERT_X(m_voice->IsValid() && m_voice->IsSetup(), "CVoiceVatlib", "Cvatlib_Voice_Simple invalid or not setup!");
        Q_ASSERT_X(m_voice->IsRoomValid(static_cast<qint32>(comUnit)), "CVoiceVatlib", "Room index out of bounds!");

        try
        {
            // update "meta" object for room
            this->setVoiceRoomForUnit(comUnit, CVoiceRoom()); // an empty voice room is easier to detect

            // only leave is room is physically connected
            if (this->m_voice->IsRoomConnected(static_cast<qint32>(comUnit)))
            {
                m_voice->LeaveRoom(static_cast<qint32>(comUnit));
                this->m_voiceRoomCallsigns[comUnit] = CCallsignList(); // empty list for this room
                changeConnectionStatus(comUnit, Disconnecting);
            }
        }
        catch (...)
        {
            this->exceptionDispatcher(Q_FUNC_INFO);
        }
    }

    /*
     * Leave all voice rooms
     */
    void CVoiceVatlib::leaveAllVoiceRooms()
    {
        this->leaveVoiceRoom(COM1);
        this->leaveVoiceRoom(COM2);
    }

    /*
     * Room output volume as per COM unit
     */
    void CVoiceVatlib::setRoomOutputVolume(const ComUnit comUnit, const qint32 volume)
    {
        QMutexLocker lockerVatlib(&m_mutexVatlib);
        Q_ASSERT_X(m_voice->IsValid() && m_voice->IsSetup(), "CVoiceVatlib", "Cvatlib_Voice_Simple invalid or not setup!");
        Q_ASSERT_X(m_voice->IsRoomValid(static_cast<qint32>(comUnit)), "CVoiceVatlib", "Room index out of bounds!");

        try
        {
            m_voice->SetRoomVolume(static_cast<qint32>(comUnit), volume);
        }
        catch (...)
        {
            this->exceptionDispatcher(Q_FUNC_INFO);
        }
    }

    /*
     * Start transmitting
     */
    void CVoiceVatlib::startTransmitting(const ComUnit comUnit)
    {
        QMutexLocker lockerVatlib(&m_mutexVatlib);
        Q_ASSERT_X(m_voice->IsValid() && m_voice->IsSetup(), "CVoiceVatlib", "Cvatlib_Voice_Simple invalid or not setup!");
        Q_ASSERT_X(m_voice->IsRoomValid(static_cast<qint32>(comUnit)), "CVoiceVatlib", "Room index out of bounds!");

        try
        {
            m_voice->SetMicState(static_cast<qint32>(comUnit), true);
        }
        catch (...)
        {
            this->exceptionDispatcher(Q_FUNC_INFO);
        }
    }

    /*
     * Start transmitting
     */
    void CVoiceVatlib::stopTransmitting(const ComUnit comUnit)
    {
        QMutexLocker lockerVatlib(&m_mutexVatlib);
        Q_ASSERT_X(m_voice->IsValid() && m_voice->IsSetup(), "CVoiceVatlib", "Cvatlib_Voice_Simple invalid or not setup!");
        Q_ASSERT_X(m_voice->IsRoomValid(static_cast<qint32>(comUnit)), "CVoiceVatlib", "Room index out of bounds!");
        try
        {
            m_voice->SetMicState(static_cast<qint32>(comUnit), false);
        }
        catch (...)
        {
            this->exceptionDispatcher(Q_FUNC_INFO);
        }
    }

    /*
     * Change room status
     */
    void CVoiceVatlib::changeRoomStatus(ComUnit comUnit, Cvatlib_Voice_Simple::roomStatusUpdate roomStatus)
    {
        qDebug() << comUnit << roomStatus; // KB_REMOVE
        CVoiceRoom vr = this->voiceRoomForUnit(comUnit);
        switch (roomStatus)
        {
        case Cvatlib_Voice_Simple::roomStatusUpdate_JoinSuccess:
            {
                m_lockOutputEnabled.lockForRead();
                bool isOutputEnabled = this->m_outputEnabled[comUnit];
                m_lockOutputEnabled.unlock();
                switchAudioOutput(comUnit, isOutputEnabled);
                vr.setConnected(true);
                this->setVoiceRoomForUnit(comUnit, vr);
                changeConnectionStatus(comUnit, Connected);
                emit userJoinedLeft(comUnit);
                break;
            }
        case Cvatlib_Voice_Simple::roomStatusUpdate_JoinFail:
            vr.setConnected(false);
            this->setVoiceRoomForUnit(comUnit, vr);
            changeConnectionStatus(comUnit, ConnectingFailed);
            break;
        case Cvatlib_Voice_Simple::roomStatusUpdate_UnexpectedDisconnectOrKicked:
            vr.setConnected(false);
            this->setVoiceRoomForUnit(comUnit, vr);
            changeConnectionStatus(comUnit, DisconnectedError);
            break;
        case Cvatlib_Voice_Simple::roomStatusUpdate_LeaveComplete:
            m_lockCallsigns.lockForWrite();
            m_voiceRoomCallsigns.clear();
            m_lockCallsigns.unlock();
            changeConnectionStatus(comUnit, Disconnected);
            break;
        case Cvatlib_Voice_Simple::roomStatusUpdate_UserJoinsLeaves:
            // FIXME: We cannot call GetRoomUserList because vatlib is not reentrent safe.
            emit userJoinedLeft(comUnit);
            break;
        case Cvatlib_Voice_Simple::roomStatusUpdate_RoomAudioStarted:
            emit audioStarted(comUnit);
            break;
        case Cvatlib_Voice_Simple::roomStatusUpdate_RoomAudioStopped:
            emit audioStopped(comUnit);
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

    /*
     * Process voice handling
     */
    void CVoiceVatlib::timerEvent(QTimerEvent *)
    {
        QMutexLocker lockerVatlib(&m_mutexVatlib);
        Q_ASSERT_X(m_voice->IsValid() && m_voice->IsSetup(), "CVoiceVatlib", "Cvatlib_Voice_Simple invalid or not setup!");

        try
        {
            this->m_voice->DoProcessing();
        }
        catch (...)
        {
            this->exceptionDispatcher(Q_FUNC_INFO);
        }
    }

    /*
     * Find squelch
     */
    void CVoiceVatlib::onEndFindSquelch()
    {
        QMutexLocker lockerVatlib(&m_mutexVatlib);
        Q_ASSERT_X(m_voice->IsValid() && m_voice->IsSetup(), "CVoiceVatlib", "Cvatlib_Voice_Simple invalid or not setup!");

        try
        {
            m_voice->EndFindSquelch();
            QWriteLocker lockForWriting(&m_lockSquelch);
            m_inputSquelch = m_voice->GetInputSquelch();
            emit squelchTestFinished();
        }
        catch (...)
        {
            this->exceptionDispatcher(Q_FUNC_INFO);
        }
    }

    void CVoiceVatlib::onEndMicTest()
    {
        QMutexLocker lockerVatlib(&m_mutexVatlib);
        Q_ASSERT_X(m_voice->IsValid() && m_voice->IsSetup(), "CVoiceVatlib", "Cvatlib_Voice_Simple invalid or not setup!");

        try
        {
            QWriteLocker lockForWriting(&m_lockTestResult);
            m_micTestResult = m_voice->EndMicTest();
            emit micTestFinished();
        }
        catch (...)
        {
            this->exceptionDispatcher(Q_FUNC_INFO);
        }
    }

    /*
     * User joined / left room
     */
    void CVoiceVatlib::onUserJoinedLeft(const ComUnit comUnit)
    {
        QMutexLocker lockerVatlib(&m_mutexVatlib);
        Q_ASSERT_X(m_voice->IsValid() && m_voice->IsSetup(), "CVoiceVatlib", "Cvatlib_Voice_Simple invalid or not setup!");
        Q_ASSERT_X(m_temporaryUserRoomIndex == CVoiceVatlib::InvalidRoomIndex, "CVoiceClientVatlib::onUserJoinedLeft", "Cannot list users for two rooms in parallel!");
        try
        {
            // Paranoia... clear list completely
            if (!m_voice->IsRoomConnected(static_cast<qint32>(comUnit)))
            {
                this->m_voiceRoomCallsigns[comUnit] = CCallsignList();
                this->m_temporaryVoiceRoomCallsigns.clear();
                return;
            }

            // Store the room index for the slot (called in static callback)
            m_temporaryUserRoomIndex = static_cast<qint32>(comUnit);

            // Callbacks already completed when function GetRoomUserList returns,
            // thereafter m_voiceRoomCallsignsUpdate is filled with the latest callsigns
            m_voice->GetRoomUserList(static_cast<qint32>(comUnit), onRoomUserReceived, this);
            m_temporaryUserRoomIndex = CVoiceVatlib::InvalidRoomIndex; // reset

            // we have all current users in m_temporaryVoiceRoomCallsigns
            foreach(CCallsign callsign, m_voiceRoomCallsigns.value(comUnit))
            {
                if (!m_temporaryVoiceRoomCallsigns.contains(callsign))
                {
                    // User has left
                    emit userLeftRoom(callsign);
                }
            }

            QWriteLocker lockForWriting(&m_lockCallsigns);
            foreach(CCallsign callsign, m_temporaryVoiceRoomCallsigns)
            {
                if (!m_voiceRoomCallsigns.value(comUnit).contains(callsign))
                {
                    // he joined
                    emit userJoinedRoom(callsign);
                }
            }

            // Finally we update it with our new list
            this->m_voiceRoomCallsigns[comUnit] = this->m_temporaryVoiceRoomCallsigns;
            this->m_temporaryVoiceRoomCallsigns.clear();
        }
        catch (...)
        {
            this->exceptionDispatcher(Q_FUNC_INFO);
        }
    }

    /********************************** * * * * * * * * * * * * * * * * * * * ************************************/
    /**********************************           shimlib callbacks           ************************************/
    /********************************** * * * * * * * * * * * * * * * * * * * ************************************/

    /*!
     * \brief Cast from *void to *CVoiceVatlib
     * \param cbvar
     * \return
     */
    CVoiceVatlib *cbvar_cast_voice(void *cbvar)
    {
        return static_cast<CVoiceVatlib *>(cbvar);
    }

    /*
     * Room status update
     */
    void CVoiceVatlib::onRoomStatusUpdate(Cvatlib_Voice_Simple *obj, Cvatlib_Voice_Simple::roomStatusUpdate upd, qint32 roomIndex, void *cbVar)
    {
        Q_UNUSED(obj)
        ComUnit comUnit = static_cast<ComUnit>(roomIndex);
        CVoiceVatlib *voiceClientVatlib = cbvar_cast_voice(cbVar);
        voiceClientVatlib->changeRoomStatus(comUnit, upd);
    }

    /*
     * Room user received
     */
    void CVoiceVatlib::onRoomUserReceived(Cvatlib_Voice_Simple *obj, const char *name, void *cbVar)
    {
        Q_UNUSED(obj)

        // sanity check
        QString callsign = QString(name);
        if (callsign.isEmpty()) return;

        // add callsign
        CVoiceVatlib *voiceClientVatlib = cbvar_cast_voice(cbVar);
        ComUnit comUnit = static_cast<ComUnit>(voiceClientVatlib->temporaryUserRoomIndex());

        // add user
        // callsign might contain: VATSIM id, user name
        if (callsign.contains(" "))
        {
            QStringList parts = callsign.split(" ");
            callsign = parts[0];
            // I throw away VATSIM id here, maybe we could use it
        }

        voiceClientVatlib->addTemporaryCallsignForRoom(comUnit, CCallsign(callsign));
    }

    /*
     * Input hardware received
     */
    void CVoiceVatlib::onInputHardwareDeviceReceived(Cvatlib_Voice_Simple *obj, const char *name, void *cbVar)
    {
        Q_UNUSED(obj)
        BlackMisc::Audio::CAudioDevice inputDevice(BlackMisc::Audio::CAudioDevice::InputDevice, cbvar_cast_voice(cbVar)->m_devices.count(BlackMisc::Audio::CAudioDevice::InputDevice), QString(name));
        QWriteLocker lockForWriting(&(cbvar_cast_voice(cbVar)->m_lockDeviceList));
        cbvar_cast_voice(cbVar)->m_devices.push_back(inputDevice);
    }

    /*
     * Output hardware received
     */
    void CVoiceVatlib::onOutputHardwareDeviceReceived(Cvatlib_Voice_Simple *obj, const char *name, void *cbVar)
    {
        Q_UNUSED(obj)
        BlackMisc::Audio::CAudioDevice outputDevice(BlackMisc::Audio::CAudioDevice::OutputDevice, cbvar_cast_voice(cbVar)->m_devices.count(BlackMisc::Audio::CAudioDevice::OutputDevice), QString(name));
        QWriteLocker lockForWriting(&(cbvar_cast_voice(cbVar)->m_lockDeviceList));
        cbvar_cast_voice(cbVar)->m_devices.push_back(outputDevice);
    }

    /*
     * COM unit to Voice room
     */
    CVoiceRoom CVoiceVatlib::voiceRoomForUnit(const IVoice::ComUnit comUnit) const
    {
        QReadLocker lockForReading(&m_lockVoiceRooms);
        return (comUnit == COM1) ? this->m_voiceRooms[0] : this->m_voiceRooms[1];
    }

    /*
     * Voice room for respectice COM unit
     */
    void CVoiceVatlib::setVoiceRoomForUnit(const IVoice::ComUnit comUnit, const CVoiceRoom &voiceRoom)
    {
        QWriteLocker lockForWriting(&m_lockVoiceRooms);
        this->m_voiceRooms[comUnit == COM1 ? 0 : 1] = voiceRoom;
    }

    /*
     * Add temp.callsign for room
     */
    void CVoiceVatlib::addTemporaryCallsignForRoom(const ComUnit /** comUnit **/, const CCallsign &callsign)
    {
        if (m_temporaryVoiceRoomCallsigns.contains(callsign)) return;
        m_temporaryVoiceRoomCallsigns.push_back(callsign);
    }

    /*
     * Forward exception as signal
     */
    void CVoiceVatlib::exceptionDispatcher(const char *caller)
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
            emit this->statusMessage(CStatusMessage::getErrorMessage(msg, CStatusMessage::TypeAudio));
            qDebug() << "NetworkNotConnectedException caught in " << caller << "\n" << e.what();
        }
        catch (const VatlibException &e)
        {
            msg.append("VatlibException").append(" ").append(e.what());
            emit this->statusMessage(CStatusMessage::getErrorMessage(msg, CStatusMessage::TypeAudio));
            qFatal("VatlibException caught in %s\n%s", caller, e.what());
        }
        catch (const std::exception &e)
        {
            msg.append("std::exception").append(" ").append(e.what());
            emit this->statusMessage(CStatusMessage::getErrorMessage(msg, CStatusMessage::TypeAudio));
            qFatal("std::exception caught in %s\n%s", caller, e.what());
        }
        catch (...)
        {
            msg.append("unknown exception");
            emit this->statusMessage(CStatusMessage::getErrorMessage(msg, CStatusMessage::TypeAudio));
            qFatal("Unknown exception caught in %s", caller);
        }
    }

    // Change voice room status and emit signal
    void CVoiceVatlib::changeConnectionStatus(ComUnit comUnit, ConnectionStatus newStatus)
    {
        QWriteLocker lockForWriting(&m_lockConnectionStatus);
        ConnectionStatus currentStatus = m_connectionStatus.value(comUnit);
        if (newStatus != currentStatus)
        {
            if (newStatus == Connected)
            {
                CVoiceRoom vr = this->voiceRoomForUnit(comUnit);
                vr.setConnected(true);
                this->setVoiceRoomForUnit(comUnit, vr);
            }

            // disconnecting the voice room will already be
            // set in leave voice room

            m_connectionStatus.insert(comUnit, newStatus);
            emit connectionStatusChanged(comUnit, currentStatus, newStatus);
        }
    }

} // namespace
