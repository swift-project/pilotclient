/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_AUDIODEVICEVOLUMESETUPCOMPONENT_H
#define BLACKGUI_COMPONENTS_AUDIODEVICEVOLUMESETUPCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/audio/audiosettings.h"
#include "blackmisc/audio/audiodeviceinfolist.h"
#include "blackmisc/aviation/callsignset.h"
#include "blackmisc/connectionguard.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/digestsignal.h"

#include <QFrame>
#include <QCheckBox>
#include <QObject>
#include <QScopedPointer>

namespace Ui { class CAudioDeviceVolumeSetupComponent; }
namespace BlackCore { namespace Afv { namespace Clients { class CAfvClient; }}}
namespace BlackGui
{
    namespace Components
    {
        //! Audio setup such as input / output devices
        class BLACKGUI_EXPORT CAudioDeviceVolumeSetupComponent : public QFrame
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CAudioDeviceVolumeSetupComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CAudioDeviceVolumeSetupComponent() override;

            //! Get input and output volume values
            //! @{
            int getInValue(int from  = BlackMisc::Audio::CSettings::InMin,  int to = BlackMisc::Audio::CSettings::InMax) const;
            int getOutValueCom1(int from = BlackMisc::Audio::CSettings::OutMin, int to = BlackMisc::Audio::CSettings::OutMax) const;
            int getOutValueCom2(int from = BlackMisc::Audio::CSettings::OutMin, int to = BlackMisc::Audio::CSettings::OutMax) const;
            //! @}

            //! Set input and output volume values
            //! @{
            void setInValue(int value,  int from = BlackMisc::Audio::CSettings::InMin,  int to = BlackMisc::Audio::CSettings::InMax);
            void setOutValueCom1(int value, int from = BlackMisc::Audio::CSettings::OutMin, int to = BlackMisc::Audio::CSettings::OutMax);
            void setOutValueCom2(int value, int from = BlackMisc::Audio::CSettings::OutMin, int to = BlackMisc::Audio::CSettings::OutMax);
            //! @}

            //! Set input and output level values 0..1
            //! @{
            void setInLevel(double value);
            void setOutLevel(double value);
            //! @}

            //! Info string
            void setInfo(const QString &info);

        private:
            //! Init
            void init();

            //! AFV client related part
            void initWithAfvClient();

            //! Reload settings
            void reloadSettings();

            //! Audio device selected
            //! \param index audio device index (COM1, COM2)
            void onAudioDeviceSelected(int index);

            //! Current audio devices changed
            void onAudioStarted(const BlackMisc::Audio::CAudioDeviceInfo &input, const BlackMisc::Audio::CAudioDeviceInfo &output);

            //! Audio has been stopped
            void onAudioStopped();

            //! Audio devices changed
            bool onAudioDevicesChanged(const BlackMisc::Audio::CAudioDeviceInfoList &devices);

            //! Loopback toggled
            void onLoopbackToggled(bool loopback);

            //! Disable audio effects disable
            void onDisableAudioEffectsToggled(bool disabled);

            //! Audio device lists from settings
            void initAudioDeviceLists();

            //! Audio is optional, check if available
            bool hasAudio() const;

            //! Check if simulator is available
            bool hasSimulator() const;

            //! Volume slider has been changed
            void onVolumeSliderChanged(int v);

            //! Save the audio volumes
            void saveVolumes();

            void onOutputVU(double vu);
            void onInputVU(double vu);

            void onReloadDevices();
            void onResetVolumeIn();
            void onResetVolumeOutCom1();
            void onResetVolumeOutCom2();

            void setAudioRunsWhere();

            bool updateIntegrateWithComFlagUi();
            bool isComIntegrated() const;

            void onIntegratedFlagChanged(bool checked);
            void onRxTxChanged(bool checked);
            void setRxTxCheckboxes(bool rx1, bool tx1, bool rx2, bool tx2);

            // TODO: Move TransceiverReceivingCallsignsChangedArgs to Blackmisc
            void onReceivingCallsignsChanged(const BlackMisc::Aviation::CCallsignSet &com1Callsigns, const BlackMisc::Aviation::CCallsignSet &com2Callsigns);
            void onUpdatedClientWithCockpitData();

            BlackMisc::Audio::CAudioDeviceInfo getSelectedInputDevice() const;
            BlackMisc::Audio::CAudioDeviceInfo getSelectedOutputDevice() const;

            //! Transmit and receive state
            //! @{
            void setTransmitReceiveInUi(bool tx1, bool rec1, bool tx2, bool rec2, bool integrated);
            void setTransmitReceiveInUiFromVoiceClient();
            //! @}

            //! Checkboxes readonly?
            void setCheckBoxesReadOnly(bool readonly);

            //! Direct access to client
            static BlackCore::Afv::Clients::CAfvClient *afvClient();

            bool m_init = false;
            BlackMisc::CConnectionGuard m_afvConnections;
            QScopedPointer<Ui::CAudioDeviceVolumeSetupComponent> ui;
            BlackMisc::Audio::CAudioDeviceInfoList m_cbDevices; //!< devices to be displayed in the checkbox
            BlackMisc::CDigestSignal m_volumeSliderChanged { this, &CAudioDeviceVolumeSetupComponent::saveVolumes, 1000, 10 };
            BlackMisc::CSetting<BlackMisc::Audio::TSettings> m_audioSettings { this, &CAudioDeviceVolumeSetupComponent::reloadSettings };
        };
    } // namespace
} // namespace

#endif // guard
