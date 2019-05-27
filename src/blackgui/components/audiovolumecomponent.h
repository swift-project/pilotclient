/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_AUDIOVOLUME_H
#define BLACKGUI_AUDIOVOLUME_H

#include "blackgui/blackguiexport.h"

#include <QFrame>
#include <QObject>
#include <QScopedPointer>

namespace Ui { class CAudioVolumeComponent; }
namespace BlackGui
{
    namespace Components
    {
        //! Audio volume, mixer
        class BLACKGUI_EXPORT CAudioVolumeComponent : public QFrame
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CAudioVolumeComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CAudioVolumeComponent();

        private:
            //! Mute toggleBlackGui::Components::CHotkeyDialog
            void onMuteChanged(bool muted);

            //! Volumes changed (elsewhere)
            void onOutputVolumeChanged(int volume);

            //! Set volume to 100
            void setVolume100();

            //! Change values because of volume GUI controls
            void changeOutputVolumeFromSlider(int volume);

            //! Change values because of volume GUI controls
            void changeOutputVolumeFromSpinBox(int volume);

            //! Requested windows mixer
            void onWindowsMixerRequested();

            //! slider value
            static int volumeToSliderValue(int volume);

            QScopedPointer<Ui::CAudioVolumeComponent> ui;
        };
    } // namespace
} // namespace

#endif // guard
