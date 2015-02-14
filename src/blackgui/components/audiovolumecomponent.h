/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_AUDIOVOLUME_H
#define BLACKGUI_AUDIOVOLUME_H

#include <QFrame>
#include <QScopedPointer>
#include "enableforruntime.h"

namespace Ui { class CAudioVolumeComponent; }
namespace BlackGui
{
    namespace Components
    {

        //! Audio volume, mixer
        class CAudioVolumeComponent :
            public QFrame,
            public CEnableForRuntime
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CAudioVolumeComponent(QWidget *parent = nullptr);

            //! Destructor
            ~CAudioVolumeComponent();

        protected:
            //! \copydoc CEnableForRuntime::runtimeHasBeenSet
            void runtimeHasBeenSet() override;

        private slots:
            //! Mute toggle
            void ps_onMuteChanged(bool muted);

            //! Volumes changed (elsewhere)
            void ps_onOutputVolumeChanged(int volume);

            //! Set volume to 100
            void ps_setVolume100();

            //! Change values because of volume GUI controls
            void ps_changeOutputVolumeFromSlider(int volume);

            //! Change values because of volume GUI controls
            void ps_changeOutputVolumeFromSpinBox(int volume);

            //! Requested windows mixer
            void ps_onWindowsMixerRequested();

        private:
            QScopedPointer<Ui::CAudioVolumeComponent> ui;

        };

    } // namespace
} // namespace

#endif // guard
