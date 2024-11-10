// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_ATCBUTTONCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_ATCBUTTONCOMPONENT_H

#include "core/context/contextnetwork.h"
#include "misc/network/connectionstatus.h"
#include <QFrame>
#include <QScopedPointer>

namespace Ui
{
    class CAtcButtonComponent;
}
namespace swift::gui::components
{
    //! ATC stations as button bar
    class CAtcButtonComponent : public QFrame
    {
        Q_OBJECT

    public:
        //! Ctor
        explicit CAtcButtonComponent(QWidget *parent = nullptr);

        //! Dtor
        virtual ~CAtcButtonComponent();

        //! Update
        void updateStations();

        //! Max.number
        void setMaxNumber(int number) { m_maxNumber = number; }

        //! With icons
        void setWithIcons(bool withIcons) { m_withIcons = withIcons; }

        //! Max.number
        int getMaxNumber() const { return m_maxNumber; }

        //! Rows/columns
        void setRowsColumns(int rows, int cols, bool setMaxElements);

        //! Ignore non ATC callsigns
        void setIgnoreNonAtcCallsigns(bool ignore) { m_ignoreNonAtc = ignore; }

        //! Background updates
        void setBackgroundUpdates(bool backgroundUpdates) { m_backgroundUpdates = backgroundUpdates; }

    signals:
        //! ATC station clicked
        void requestAtcStation(const swift::misc::aviation::CAtcStation &station);

    private:
        //! Changed ATC stations
        void onChangedAtcStations();

        //! Connection status did change
        void onConnectionStatusChanged(const swift::misc::network::CConnectionStatus &from, const swift::misc::network::CConnectionStatus &to);

        //! Button has been clicked
        void onButtonClicked();

        QScopedPointer<Ui::CAtcButtonComponent> ui;
        bool m_withIcons = true;
        bool m_ignoreNonAtc = true;
        bool m_backgroundUpdates = true;
        int m_maxNumber = 8;
        int m_rows = 2;
        int m_cols = 4;
    };
} // ns

#endif // guard
