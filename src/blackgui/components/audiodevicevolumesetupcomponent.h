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
#include "blackcore/afv/audio/receiversampleprovider.h"
#include "blackmisc/audio/audiosettings.h"
#include "blackmisc/audio/audiodeviceinfolist.h"
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

            //! Get input and output volume values @{
            int getInValue(int from  = BlackMisc::Audio::CSettings::InMin,  int to = BlackMisc::Audio::CSettings::InMax) const;
            int getOutValue(int from = BlackMisc::Audio::CSettings::OutMin, int to = BlackMisc::Audio::CSettings::OutMax) const;
            //! @}

            //! Set input and output volume values @{
            void setInValue(int value,  int from = BlackMisc::Audio::CSettings::InMin,  int to = BlackMisc::Audio::CSettings::InMax);
            void setOutValue(int value, int from = BlackMisc::Audio::CSettings::OutMin, int to = BlackMisc::Audio::CSettings::OutMax);
            //! @}

            //! Set input and output level values @{
            void setInLevel(int value,  int from, int to);
            void setOutLevel(int value, int from, int to);
            //! @}

            //! Info string
            void setInfo(const QString &info);

            //! Transmit and receive state
            void setTransmitReceive(bool tx1, bool rec1, bool tx2, bool rec2);

        private:
            //! Init
            void init();

            //! Reload settings
            void reloadSettings();

            //! Audio device selected
            //! \param index audio device index (COM1, COM2)
            void onAudioDeviceSelected(int index);

            //! Current audio devices changed
            void onCurrentAudioDevicesChanged(const BlackMisc::Audio::CAudioDeviceInfoList &devices);

            //! Audio devices changed
            void onAudioDevicesChanged(const BlackMisc::Audio::CAudioDeviceInfoList &devices);

            //! Loopback toggled
            void onLoopbackToggled(bool loopback);

            //! Disable audio effects disable
            void onDisableAudioEffectsToggled(bool disabled);

            //! Audio device lists from settings
            void initAudioDeviceLists();

            //! Audio is optional, check if available
            bool hasAudio() const;

            //! Volume slider has been changed
            void onVolumeSliderChanged(int v);

            //! Save the audio volumes
            void saveVolumes();

            void onOutputVU(double vu);
            void onInputVU(double vu);

            void onReloadDevices();
            void onResetVolumeIn();
            void onResetVolumeOut();
            void onReceivingCallsignsChanged(const BlackCore::Afv::Audio::TransceiverReceivingCallsignsChangedArgs &args);

            BlackMisc::Audio::CAudioDeviceInfo getSelectedInputDevice() const;
            BlackMisc::Audio::CAudioDeviceInfo getSelectedOutputDevice() const;

            static BlackCore::Afv::Clients::CAfvClient *afvClient();

            QScopedPointer<Ui::CAudioDeviceVolumeSetupComponent> ui;
            BlackMisc::CDigestSignal m_volumeSliderChanged { this, &CAudioDeviceVolumeSetupComponent::saveVolumes, 1000, 10 };
            BlackMisc::CSetting<BlackMisc::Audio::TSettings> m_audioSettings { this, &CAudioDeviceVolumeSetupComponent::reloadSettings };
        };
    } // namespace
} // namespace

#endif // guard
