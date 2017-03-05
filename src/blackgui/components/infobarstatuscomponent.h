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

#include "blackcore/actionbind.h"
#include "blackcore/network.h"
#include "blackgui/blackguiexport.h"

#include <QFrame>
#include <QObject>
#include <QScopedPointer>
#include <QString>

class QPoint;
class QWidget;

namespace Ui { class CInfoBarStatusComponent; }

namespace BlackGui
{
    namespace Components
    {
        //! Info bar displaying status (Network, Simulator, DBus)
        class BLACKGUI_EXPORT CInfoBarStatusComponent :
            public QFrame
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

        private:
            QScopedPointer<Ui::CInfoBarStatusComponent> ui;
            BlackCore::CActionBind m_actionPtt { "/Voice/Activate push-to-talk", BlackMisc::CIcons::radio16(), this, &CInfoBarStatusComponent::ps_onPttChanged };

        private slots:
            //! Simulator connection has been changed
            void ps_onSimulatorStatusChanged(int status);

            //! Network connection has been changed
            void ps_onNetworkConnectionChanged(BlackCore::INetwork::ConnectionStatus from, BlackCore::INetwork::ConnectionStatus to);

            //! Context menu requested
            void ps_customAudioContextMenuRequested(const QPoint &position);

            //! Mute changed
            void ps_onMuteChanged(bool muted);

            //! Mapper is ready
            void ps_onMapperReady();

            //! Ptt button changed
            void ps_onPttChanged(bool enabled);
        };
    }
}
#endif // guard
