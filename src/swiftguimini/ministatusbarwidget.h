// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFTGUIMINI_MINISTATUSBARWIDGET_H
#define SWIFTGUIMINI_MINISTATUSBARWIDGET_H

#include <QFrame>

#include "misc/network/connectionstatus.h"

namespace swift::gui
{
    class CLedWidget;
}

//! Schlanke Statusanzeige (LEDs) fuer Simulator- und FSD-Netzwerkverbindung,
//! gedacht zur Platzierung oberhalb des zentralen Tab-Bereichs (z.B. unterhalb der Menueleiste).
class CMiniStatusBarWidget : public QFrame
{
    Q_OBJECT

public:
    //! Constructor
    explicit CMiniStatusBarWidget(QWidget *parent = nullptr);

    //! Destructor
    ~CMiniStatusBarWidget() override;

private:
    //! Simulator status changed
    void onSimulatorStatusChanged(int status);

    //! Network status changed
    void onNetworkStatusChanged(const swift::misc::network::CConnectionStatus &from,
                                const swift::misc::network::CConnectionStatus &to);

    //! Initial values (e.g. after construction, or if contexts become available later)
    void updateValues();

    swift::gui::CLedWidget *m_ledSimulator = nullptr;
    swift::gui::CLedWidget *m_ledNetwork = nullptr;
};

#endif // SWIFTGUIMINI_MINISTATUSBARWIDGET_H