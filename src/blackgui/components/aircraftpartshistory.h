/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENT_AIRCRAFTPARTSHISTORY_H
#define BLACKGUI_COMPONENT_AIRCRAFTPARTSHISTORY_H

#include "blackcore/network.h"
#include <QFrame>
#include <QTabWidget>
#include <QTimer>
#include <QTextDocument>

namespace Ui { class CAircraftPartsHistory; }
namespace BlackGui
{
    namespace Components
    {
        /*!
         * History frame for received aircraft parts
         */
        class CAircraftPartsHistory : public QFrame
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CAircraftPartsHistory(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CAircraftPartsHistory();

        private:
            QScopedPointer<Ui::CAircraftPartsHistory> ui;
            QTimer m_timerUpdateHistory;
            QTextDocument m_text { this };
            uint m_htmlHash = 0;

            //! Init
            void initGui();

            //! Contexts available
            bool hasContexts() const;

            //! Is parts history enabled?
            bool partsHistoryEnabled() const;

        private:
            //! Update parts history
            void updatePartsHistory();

            //! Callsign was entered
            void callsignEntered();

            //! When values changed elsewhere
            void valuesChanged();

            //! Toggle between enabling and disabling of history
            void toggleHistoryEnabled(bool enabled);

            //! Connection status changed
            void connectionStatusChanged(BlackCore::INetwork::ConnectionStatus from, BlackCore::INetwork::ConnectionStatus to);
        };
    } // ns
} // ns

#endif // guard
