// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENT_AIRCRAFTPARTSHISTORY_H
#define SWIFT_GUI_COMPONENT_AIRCRAFTPARTSHISTORY_H

#include <QFrame>
#include <QTabWidget>
#include <QTextDocument>
#include <QTimer>

#include "misc/network/connectionstatus.h"

namespace Ui
{
    class CAircraftPartsHistory;
}
namespace swift::gui::components
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
        size_t m_htmlHash = 0;

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
        void connectionStatusChanged(const swift::misc::network::CConnectionStatus &from,
                                     const swift::misc::network::CConnectionStatus &to);
    };
} // namespace swift::gui::components

#endif // SWIFT_GUI_COMPONENT_AIRCRAFTPARTSHISTORY_H
