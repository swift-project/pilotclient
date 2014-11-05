/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_AUDIOSETUPCOMPONENT_H
#define BLACKGUI_AUDIOSETUPCOMPONENT_H

#include "enableforruntime.h"
#include "blackmisc/audiodevicelist.h"
#include <QFrame>
#include <QScopedPointer>

namespace Ui { class CAudioSetupComponent; }

namespace BlackGui
{
    namespace Components
    {
        //! Audio setup such as input / output devices
        class CAudioSetupComponent :
            public QFrame,
            public CEnableForRuntime
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CAudioSetupComponent(QWidget *parent = nullptr);

            //! Destructor
            ~CAudioSetupComponent();

            //! Play notification sounds (at all)
            bool playNotificationSounds() const;

        public slots:
            //! Reload settings
            void reloadSettings();

        protected:
            //! \copydoc CRuntimeBasedComponent::runtimeHasBeenSet
            virtual void runtimeHasBeenSet() override;

        private slots:

            //! Settings have been changed
            void ps_changedSettings(uint typeValue);

            //! start the MIC tests (Squelch)
            void ps_startAudioTest();

            //! Audio test updates (timer) for progressbar and fetching results
            void ps_audioTestUpdate();

            /*!
             * \brief Audio device selected
             * \param index audio device index (COM1, COM2)
             */
            void ps_audioDeviceSelected(int index);

            //! Current audio devices changed
            void ps_onCurrentAudioDevicesChanged(const Audio::CAudioDeviceList &devices);

            //! Audio devices changed
            void ps_onAudioDevicesChanged(const Audio::CAudioDeviceList &devices);

        private:
            //! Audio test modes
            enum AudioTest
            {
                NoAudioTest,
                SquelchTest,
                MicrophoneTest
            };

            //! Audio device lists from settings
            void initAudioDeviceLists();

            QScopedPointer<Ui::CAudioSetupComponent> ui;
            QTimer   *m_timerAudioTests; //!< audio tests: progress bar, disable/enable buttons
            AudioTest m_audioTestRunning = NoAudioTest;
        };
    } // namespace
} // namespace

#endif // guard
