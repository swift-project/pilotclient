/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "voice_vatlib.h"
#include "voice_channel_vatlib.h"
#include "blackmisc/logmessage.h"
#include <QDebug>
#include <QTimer>

#include <mutex>

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
        m_vatlib(Cvatlib_Voice_Simple::Create(), Cvatlib_Voice_Simple_Deleter::cleanup),
        // m_audioOutput(nullptr), // removed #227
        m_inputSquelch(-1),
        m_micTestResult(Cvatlib_Voice_Simple::agc_Ok),
        m_lockCurrentOutputDevice(QMutex::Recursive),
        m_lockCurrentInputDevice(QMutex::Recursive),
        m_lockDeviceList(QMutex::Recursive)
    {
        try
        {
            // we use reset here until issue #277 is resolved
            // easier to find root cause
            // m_audioOutput.reset(new QAudioOutput());
            m_vatlib->Setup(true, 3290, 2, 1, onRoomStatusUpdate, this);
            m_vatlib->GetInputDevices(onInputHardwareDeviceReceived, this);
            m_vatlib->GetOutputDevices(onOutputHardwareDeviceReceived, this);

            this->m_currentInputDevice = this->defaultAudioInputDevice();
            this->m_currentOutputDevice = this->defaultAudioOutputDevice();

            for (int ii = 0; ii < 2; ++ii)
            {
                IVoiceChannel *channel = new CVoiceChannelVatlib(m_vatlib, this);
                Q_ASSERT(channel);
                m_hashChannelIndex.insert(ii, channel);
            }

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
    const BlackMisc::Audio::CAudioDeviceInfoList &CVoiceVatlib::audioDevices() const
    {
        QMutexLocker lockForReading(&m_lockDeviceList);
        return m_devices;
    }

    /*
     * Default input device
     */
    const BlackMisc::Audio::CAudioDeviceInfo CVoiceVatlib::defaultAudioInputDevice() const
    {
        // Constructor creates already a default device
        return BlackMisc::Audio::CAudioDeviceInfo(BlackMisc::Audio::CAudioDeviceInfo::InputDevice, BlackMisc::Audio::CAudioDeviceInfo::defaultDeviceIndex(), "default");
    }

    /*
     * Default output device
     */
    const BlackMisc::Audio::CAudioDeviceInfo CVoiceVatlib::defaultAudioOutputDevice() const
    {
        // Constructor creates already a default device
        return BlackMisc::Audio::CAudioDeviceInfo(BlackMisc::Audio::CAudioDeviceInfo::OutputDevice, BlackMisc::Audio::CAudioDeviceInfo::defaultDeviceIndex(), "default");
    }

    /*
     * Current output device
     */
    CAudioDeviceInfo CVoiceVatlib::getCurrentOutputDevice() const
    {
        QMutexLocker locker(&m_lockCurrentOutputDevice);
        return m_currentOutputDevice;
    }

    /*
     * Current input device
     */
    CAudioDeviceInfo CVoiceVatlib::getCurrentInputDevice() const
    {
        QMutexLocker locker(&m_lockCurrentInputDevice);
        return m_currentInputDevice;
    }

    /*
     * Set input device
     */
    void CVoiceVatlib::setInputDevice(const BlackMisc::Audio::CAudioDeviceInfo &device)
    {
        std::lock_guard<TVatlibPointer> locker(m_vatlib);
        Q_ASSERT_X(m_vatlib->IsValid() && m_vatlib->IsSetup(), "CVoiceVatlib", "Cvatlib_Voice_Simple invalid or not setup!");
        if (!device.isValid())
        {
            qWarning() << "Cannot set invalid input device!";
            return;
        }

        try
        {
            if (!m_vatlib->SetInputDevice(device.getIndex()))
            {
                qWarning() << "Setting input device failed";
            }
            if (!m_vatlib->IsInputDeviceAlive())
            {
                qWarning() << "Input device hit a fatal error";
            }
            QMutexLocker writeLocker(&m_lockCurrentInputDevice);
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
    void CVoiceVatlib::setOutputDevice(const BlackMisc::Audio::CAudioDeviceInfo &device)
    {
        std::lock_guard<TVatlibPointer> locker(m_vatlib);
        Q_ASSERT_X(m_vatlib->IsValid() && m_vatlib->IsSetup(), "CVoiceVatlib", "Cvatlib_Voice_Simple invalid or not setup!");
        if (!device.isValid())
        {
            qWarning() << "Cannot set invalid output device!";
            return;
        }

        try
        {
            // there is no return value here: https://dev.vatsim-germany.org/issues/115
            m_vatlib->SetOutputDevice(0, device.getIndex());
            if (!m_vatlib->IsOutputDeviceAlive(0))
            {
                qWarning() << "Output device hit a fatal error";
            }
            QMutexLocker writeLocker(&m_lockCurrentOutputDevice);
            this->m_currentOutputDevice = device;
        }
        catch (...)
        {
            exceptionDispatcher(Q_FUNC_INFO);
        }
    }

    void CVoiceVatlib::enableAudioLoopback(bool enable)
    {
        if (enable == m_isAudioLoopbackEnabled)
            return;

        std::lock_guard<TVatlibPointer> locker(m_vatlib);
        Q_ASSERT_X(m_vatlib->IsValid() && m_vatlib->IsSetup(), "CVoiceVatlib", "Cvatlib_Voice_Simple invalid or not setup!");
        try
        {
            m_vatlib->SetAudioLoopback(0, enable);
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
        std::lock_guard<TVatlibPointer> locker(m_vatlib);
        Q_ASSERT_X(m_vatlib->IsValid() && m_vatlib->IsSetup(), "CVoiceVatlib", "Cvatlib_Voice_Simple invalid or not setup!");

        try
        {
            m_vatlib->BeginFindSquelch();
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
        std::lock_guard<TVatlibPointer> locker(m_vatlib);
        Q_ASSERT_X(m_vatlib->IsValid() && m_vatlib->IsSetup(), "CVoiceVatlib", "Cvatlib_Voice_Simple invalid or not setup!");

        try
        {
            m_vatlib->BeginMicTest();
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
        return m_inputSquelch;
    }

    /*
     * Mic test
     */
    qint32 CVoiceVatlib::micTestResult() const
    {
        return m_micTestResult;
    }

    /*
     * Mic test, result as string
     */
    QString CVoiceVatlib::micTestResultAsString() const
    {
        QString result;

        // Due to a standard defect, some compilers do not accept two consecutive implicit conversions.
        // Hence calling load() here.
        // References:
        // http://stackoverflow.com/questions/25143860/implicit-convertion-from-class-to-enumeration-type-in-switch-conditional
        // http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3323.pdf
        switch (m_micTestResult.load())
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

    IVoiceChannel *CVoiceVatlib::getVoiceChannel(qint32 channelIndex) const
    {
        IVoiceChannel *channel = m_hashChannelIndex.value(channelIndex, nullptr);
        Q_ASSERT(channel);

        return channel;
    }

    /*
     * Handle PTT
     */
    void CVoiceVatlib::handlePushToTalk(bool value)
    {
        qDebug() << "PTT";
        if (!this->m_vatlib) return;

        if (value) qDebug() << "Start transmitting...";
        else qDebug() << "Stop transmitting...";

        // FIXME: Set only once channel to active for transmitting
        if (value)
        {
            getVoiceChannel(0)->startTransmitting();
            getVoiceChannel(1)->startTransmitting();
        }
        else
        {
            getVoiceChannel(0)->stopTransmitting();
            getVoiceChannel(1)->stopTransmitting();
        }
    }

    /*
     * Process voice handling
     */
    void CVoiceVatlib::timerEvent(QTimerEvent *)
    {
        std::lock_guard<TVatlibPointer> locker(m_vatlib);
        Q_ASSERT_X(m_vatlib->IsValid() && m_vatlib->IsSetup(), "CVoiceVatlib", "Cvatlib_Voice_Simple invalid or not setup!");

        try
        {
            this->m_vatlib->DoProcessing();
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
        std::lock_guard<TVatlibPointer> locker(m_vatlib);
        Q_ASSERT_X(m_vatlib->IsValid() && m_vatlib->IsSetup(), "CVoiceVatlib", "Cvatlib_Voice_Simple invalid or not setup!");

        try
        {
            m_vatlib->EndFindSquelch();
            m_inputSquelch = m_vatlib->GetInputSquelch();
            emit squelchTestFinished();
        }
        catch (...)
        {
            this->exceptionDispatcher(Q_FUNC_INFO);
        }
    }

    void CVoiceVatlib::onEndMicTest()
    {
        std::lock_guard<TVatlibPointer> locker(m_vatlib);
        Q_ASSERT_X(m_vatlib->IsValid() && m_vatlib->IsSetup(), "CVoiceVatlib", "Cvatlib_Voice_Simple invalid or not setup!");

        try
        {
            m_micTestResult = m_vatlib->EndMicTest();
            emit micTestFinished();
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
        CVoiceVatlib *vatlibRoom = cbvar_cast_voice(cbVar);
        vatlibRoom->onRoomStatusUpdate(roomIndex, upd);
    }

    /*
     * Input hardware received
     */
    void CVoiceVatlib::onInputHardwareDeviceReceived(Cvatlib_Voice_Simple *obj, const char *name, void *cbVar)
    {
        Q_UNUSED(obj)
        BlackMisc::Audio::CAudioDeviceInfo inputDevice(BlackMisc::Audio::CAudioDeviceInfo::InputDevice, cbvar_cast_voice(cbVar)->m_devices.count(BlackMisc::Audio::CAudioDeviceInfo::InputDevice), QString(name));
        QMutexLocker lockForWriting(&(cbvar_cast_voice(cbVar)->m_lockDeviceList));
        cbvar_cast_voice(cbVar)->m_devices.push_back(inputDevice);
    }

    /*
     * Output hardware received
     */
    void CVoiceVatlib::onOutputHardwareDeviceReceived(Cvatlib_Voice_Simple *obj, const char *name, void *cbVar)
    {
        Q_UNUSED(obj)
        BlackMisc::Audio::CAudioDeviceInfo outputDevice(BlackMisc::Audio::CAudioDeviceInfo::OutputDevice, cbvar_cast_voice(cbVar)->m_devices.count(BlackMisc::Audio::CAudioDeviceInfo::OutputDevice), QString(name));
        QMutexLocker lockForWriting(&(cbvar_cast_voice(cbVar)->m_lockDeviceList));
        cbvar_cast_voice(cbVar)->m_devices.push_back(outputDevice);
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
            CLogMessage(this).debug() << "NetworkNotConnectedException" << e.what() << "in" << caller;
        }
        catch (const VatlibException &e)
        {
            CLogMessage(this).error("VatlibException %1 in %2") << e.what() << caller;
            Q_ASSERT(false);
        }
        catch (const std::exception &e)
        {
            CLogMessage(this).error("std::exception %1 in %2") << e.what() << caller;
            Q_ASSERT(false);
        }
        catch (...)
        {
            CLogMessage(this).error("Unknown exception in %1") << caller;
            Q_ASSERT(false);
        }
    }

    void CVoiceVatlib::onRoomStatusUpdate(qint32 roomIndex, Cvatlib_Voice_Simple::roomStatusUpdate roomStatus)
    {
        QList<IVoiceChannel *> voiceChannels = m_hashChannelIndex.values();
        auto iterator = std::find_if(voiceChannels.begin(), voiceChannels.end(), [&](const IVoiceChannel * voiceChannel)
        {
            return voiceChannel->getRoomIndex() == roomIndex;
        });

        if (iterator == voiceChannels.end())
        {
            qWarning() << "Unknown room index";
            return;
        }

        (*iterator)->updateRoomStatus(roomStatus);
    }

} // namespace
