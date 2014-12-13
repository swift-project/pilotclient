/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_INFOBARSTATUSCOMPONENT_H
#define BLACKGUI_INFOBARSTATUSCOMPONENT_H

#include "enableforruntime.h"
#include "../led.h"
#include <QFrame>
#include <QScopedPointer>

namespace Ui { class CInfoBarStatusComponent; }
namespace BlackGui
{
    namespace Components
    {
        //! Info bar displaying status (Network, Simulator, DBus)
        class CInfoBarStatusComponent :
            public QFrame,
            public CEnableForRuntime
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CInfoBarStatusComponent(QWidget *parent = nullptr);

            //!Destructor
            ~CInfoBarStatusComponent();

            //! Init the LEDs
            void initLeds();

            //! DBus used
            void setDBusStatus(bool dbus);

            //! Tooltip for DBus
            void setDBusTooltip(const QString &tooltip);

        protected:
            //! \copydoc CRuntimeBasedComponent::runtimeHasBeenSet
            virtual void runtimeHasBeenSet() override;

        private:
            QScopedPointer<Ui::CInfoBarStatusComponent> ui;

        private slots:
            //! Simulator connection has been changed
            void ps_onSimulatorStatusChanged(bool connected, bool running, bool paused);

            //! Network connection has been changed
            void ps_onNetworkConnectionChanged(uint from, uint to);

            //! Context menu requested
            void ps_customAudioContextMenuRequested(const QPoint &position);

            //! Volumes changed 0..100
            void ps_onVolumesChanged(qint32 com1Volume, qint32 com2Volume);

            //! Mute changed
            void ps_onMuteChanged(bool muted);
        };
    }
}
#endif // guard
