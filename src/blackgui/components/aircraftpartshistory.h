/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENT_AIRCRAFTPARTSHISTORY_H
#define BLACKGUI_COMPONENT_AIRCRAFTPARTSHISTORY_H

#include <QFrame>
#include <QTabWidget>
#include <QTimer>
#include <QTextDocument>
#include "blackmisc/network/connectionstatus.h"

namespace Ui { class CAircraftPartsHistory; }
namespace BlackGui::Components
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
        void connectionStatusChanged(const BlackMisc::Network::CConnectionStatus &from, const BlackMisc::Network::CConnectionStatus &to);
    };
} // ns

#endif // guard
