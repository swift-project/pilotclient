// SPDX-FileCopyrightText: Copyright (C) 2026 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_MINIATCSTATIONWIDGET_H
#define SWIFT_GUI_COMPONENTS_MINIATCSTATIONWIDGET_H

#include <QFrame>
#include <QObject>

#include "gui/swiftguiexport.h"
#include "misc/network/connectionstatus.h"

namespace swift::gui::views
{
    class CAtcStationView;
}
namespace swift::gui::components
{
    /*!
     * Stark vereinfachte ATC Stationsliste.
     * Im Gegensatz zu CAtcStationComponent komplett ohne Abhaengigkeit
     * zu CDockWidgetInfoArea / CInfoArea - dadurch direkt in jedem
     * beliebigen Fenster (auch ohne Infoarea-Infrastruktur) nutzbar.
     * \remark benoetigt sGui mit gestartetem IContextNetwork
     */
    class SWIFT_GUI_EXPORT CMiniAtcStationWidget : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CMiniAtcStationWidget(QWidget *parent = nullptr);

        //! Destructor
        ~CMiniAtcStationWidget() override;

        //! Number of online stations
        int countOnlineStations() const;

    public slots:
        //! Pull current stations from context and refresh the view
        void update();

    private:
        //! ATC station list has changed (digest signal from context)
        void onChangedAtcStationsOnline();

        //! Connection status has changed
        void onConnectionStatusChanged(const swift::misc::network::CConnectionStatus &from,
                                       const swift::misc::network::CConnectionStatus &to);

        swift::gui::views::CAtcStationView *m_view = nullptr;
    };
} // namespace swift::gui::components

#endif // SWIFT_GUI_COMPONENTS_MINIATCSTATIONWIDGET_H