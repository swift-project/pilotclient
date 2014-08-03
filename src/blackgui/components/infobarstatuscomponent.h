/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_INFOBARSTATUSCOMPONENT_H
#define BLACKGUI_INFOBARSTATUSCOMPONENT_H

#include "runtimebasedcomponent.h"
#include "../led.h"
#include <QFrame>

namespace Ui { class CInfoBarStatusComponent; }
namespace BlackGui
{
    namespace Components
    {
        //! Info bar displaying status (Network, Simulator, DBus)
        class CInfoBarStatusComponent : public QFrame, public CRuntimeBasedComponent
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CInfoBarStatusComponent(QWidget *parent = nullptr);

            //!Constructor
            ~CInfoBarStatusComponent();

            //! Init the LEDs
            void initLeds();

            //! DBus used
            void setDBusStatus(bool dbus);

            //! Tooltip for DBus
            void setDBusTooltip(const QString &tooltip);

            //! Volume 0.100
            void setVolume(int volume);

        protected:
            //! \copydoc CRuntimeBasedComponent::runtimeHasBeenSet
            virtual void runtimeHasBeenSet() override;

        private:
            Ui::CInfoBarStatusComponent *ui;

        private slots:
            //! Simulator connection has been changed
            void ps_simulatorConnectionChanged(bool connected);

            //! Network connection has been changed
            void ps_networkConnectionChanged(uint from, uint to, const QString &message);

            //! Context menu requested
            void ps_customAudioContextMenuRequested(const QPoint &position);
        };
    }
}
#endif // guard
