/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_ATCBUTTONCOMPONENT_H
#define BLACKGUI_COMPONENTS_ATCBUTTONCOMPONENT_H

#include "blackcore/context/contextnetwork.h"
#include "blackmisc/network/connectionstatus.h"
#include <QFrame>
#include <QScopedPointer>

namespace Ui
{
    class CAtcButtonComponent;
}
namespace BlackGui::Components
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
        void requestAtcStation(const BlackMisc::Aviation::CAtcStation &station);

    private:
        //! Changed ATC stations
        void onChangedAtcStations();

        //! Connection status did change
        void onConnectionStatusChanged(const BlackMisc::Network::CConnectionStatus &from, const BlackMisc::Network::CConnectionStatus &to);

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
