/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_AUDIOADVANCEDDISTRIBUTEDCOMPONENT_H
#define BLACKGUI_COMPONENTS_AUDIOADVANCEDDISTRIBUTEDCOMPONENT_H

#include "blackmisc/audio/audiodeviceinfo.h"

#include <QFrame>
#include <QScopedPointer>

namespace Ui { class CAudioAdvancedDistributedComponent; }
namespace BlackGui::Components
{
    //! Adv. and sitributed audio setup
    class CAudioAdvancedDistributedComponent : public QFrame
    {
        Q_OBJECT

    public:
        //! Ctor
        explicit CAudioAdvancedDistributedComponent(QWidget *parent = nullptr);

        //! Dtor
        virtual ~CAudioAdvancedDistributedComponent() override;

        //! Reload registered devices
        void reloadRegisteredDevices();

    private:
        QScopedPointer<Ui::CAudioAdvancedDistributedComponent> ui;

        //! Audio start/stop
        void toggleAudioStartStop();

        //! Audio enable/disable
        void toggleAudioEnableDisable();

        //! Start/stop button
        //! @{
        void setButtons();
        void setButtons(int delayMs);
        //! @}

        void onAudioStarted(const BlackMisc::Audio::CAudioDeviceInfo &inputDevice, const BlackMisc::Audio::CAudioDeviceInfo &outputDevice);
        void onAudioStoppend();

        //! Contexts
        static bool hasContexts();
    };
} // ns

#endif // guard
