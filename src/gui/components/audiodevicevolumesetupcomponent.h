// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_AUDIODEVICEVOLUMESETUPCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_AUDIODEVICEVOLUMESETUPCOMPONENT_H

#include <QCheckBox>
#include <QFrame>
#include <QObject>
#include <QScopedPointer>

#include "gui/swiftguiexport.h"
#include "misc/audio/audiodeviceinfolist.h"
#include "misc/audio/audiosettings.h"
#include "misc/aviation/callsignset.h"
#include "misc/connectionguard.h"
#include "misc/digestsignal.h"
#include "misc/settingscache.h"

namespace Ui
{
    class CAudioDeviceVolumeSetupComponent;
}
namespace swift::core::afv::clients
{
    class CAfvClient;
}
namespace swift::gui::components
{
    //! Audio setup such as input / output devices
    class SWIFT_GUI_EXPORT CAudioDeviceVolumeSetupComponent : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAudioDeviceVolumeSetupComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CAudioDeviceVolumeSetupComponent() override;

        //! @{
        //! Get input and output volume values
        int getInValue(int from = swift::misc::audio::CSettings::InMin,
                       int to = swift::misc::audio::CSettings::InMax) const;
        int getOutValue(int from = swift::misc::audio::CSettings::OutMin,
                        int to = swift::misc::audio::CSettings::OutMax) const;
        int getOutValueCom1(int from = swift::misc::audio::CSettings::OutMin,
                            int to = swift::misc::audio::CSettings::OutMax) const;
        int getOutValueCom2(int from = swift::misc::audio::CSettings::OutMin,
                            int to = swift::misc::audio::CSettings::OutMax) const;
        //! @}

        //! @{
        //! Set input and output volume values
        void setInValue(int value, int from = swift::misc::audio::CSettings::InMin,
                        int to = swift::misc::audio::CSettings::InMax);
        void setOutValue(int value, int from = swift::misc::audio::CSettings::InMin,
                         int to = swift::misc::audio::CSettings::InMax);
        void setOutValueCom1(int value, int from = swift::misc::audio::CSettings::OutMin,
                             int to = swift::misc::audio::CSettings::OutMax);
        void setOutValueCom2(int value, int from = swift::misc::audio::CSettings::OutMin,
                             int to = swift::misc::audio::CSettings::OutMax);
        //! @}

        //! @{
        //! Set input and output level values 0..1
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
        void onAudioStarted(const swift::misc::audio::CAudioDeviceInfo &input,
                            const swift::misc::audio::CAudioDeviceInfo &output);

        //! Audio has been stopped
        void onAudioStopped();

        //! Audio devices changed
        bool onAudioDevicesChanged(const swift::misc::audio::CAudioDeviceInfoList &devices);

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
        void onResetVolumeOut();
        void onResetVolumeOutCom1();
        void onResetVolumeOutCom2();

        void setAudioRunsWhere();

        bool isComIntegrated() const;

        void onRxTxChanged(bool checked);
        void setRxTxCheckboxes(bool rx1, bool tx1, bool rx2, bool tx2);

        // TODO: Move TransceiverReceivingCallsignsChangedArgs to Misc
        void onReceivingCallsignsChanged(const swift::misc::aviation::CCallsignSet &com1Callsigns,
                                         const swift::misc::aviation::CCallsignSet &com2Callsigns);
        void onUpdatedClientWithCockpitData();

        swift::misc::audio::CAudioDeviceInfo getSelectedInputDevice() const;
        swift::misc::audio::CAudioDeviceInfo getSelectedOutputDevice() const;

        //! @{
        //! Transmit and receive state
        void setTransmitReceiveInUi(bool tx1, bool rec1, bool tx2, bool rec2, bool integrated);
        void setTransmitReceiveInUiFromVoiceClient();
        //! @}

        //! Checkboxes readonly?
        void setCheckBoxesReadOnly(bool readonly);

        //! Volume sliders readlonly?
        void setVolumeSlidersReadOnly(bool readonly);

        //! Direct access to client
        static swift::core::afv::clients::CAfvClient *afvClient();

        bool m_init = false;
        swift::misc::CConnectionGuard m_afvConnections;
        QScopedPointer<Ui::CAudioDeviceVolumeSetupComponent> ui;
        swift::misc::audio::CAudioDeviceInfoList m_cbDevices; //!< devices to be displayed in the checkbox
        swift::misc::CDigestSignal m_volumeSliderChanged { this, &CAudioDeviceVolumeSetupComponent::saveVolumes,
                                                           std::chrono::milliseconds(1000), 10 };
        swift::misc::CSetting<swift::misc::audio::TSettings> m_audioSettings {
            this, &CAudioDeviceVolumeSetupComponent::reloadSettings
        };

    private slots:
        void simulatorSettingsChanged();
    };
} // namespace swift::gui::components

#endif // guard
