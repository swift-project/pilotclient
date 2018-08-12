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

namespace Ui { class CInfoBarStatusComponent; }
namespace BlackGui
{
    namespace Components
    {
        //! Info bar displaying status (Network, Simulator, DBus)
        class BLACKGUI_EXPORT CInfoBarStatusComponent : public QFrame
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CInfoBarStatusComponent(QWidget *parent = nullptr);

            //!Destructor
            virtual ~CInfoBarStatusComponent();

            //! DBus used?
            void setDBusStatus(bool dbus);

            //! Tooltip for DBus
            void setDBusTooltip(const QString &tooltip);

            //! Set the spacing
            void setSpacing(int spacing);

        private:
            QScopedPointer<Ui::CInfoBarStatusComponent> ui;
            BlackCore::CActionBind m_actionPtt { "/Voice/Activate push-to-talk", BlackMisc::CIcons::radio16(), this, &CInfoBarStatusComponent::onPttChanged };

            //! Simulator connection has been changed
            void onSimulatorStatusChanged(int status);

            //! Network connection has been changed
            void onNetworkConnectionChanged(BlackCore::INetwork::ConnectionStatus from, BlackCore::INetwork::ConnectionStatus to);

            //! Context menu requested
            void onCustomAudioContextMenuRequested(const QPoint &position);

            //! Mute changed
            void onMuteChanged(bool muted);

            //! Mapper is ready
            void onMapperReady();

            //! Ptt button changed
            void onPttChanged(bool enabled);

            //! Internet accessible?
            void onInternetAccessibleChanged(bool access);

            //! Update spacing based on main window size
            void updateSpacing();
        };
    } // ns
} // ns
#endif // guard
