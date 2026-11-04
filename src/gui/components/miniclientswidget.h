// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_MINICLIENTSWIDGET_H
#define SWIFT_GUI_COMPONENTS_MINICLIENTSWIDGET_H

#include <QFrame>
#include <QObject>

#include "gui/settings/minisettings.h"
#include "gui/swiftguiexport.h"
#include "misc/aviation/atcstationlist.h"
#include "misc/network/connectionstatus.h"

namespace swift::gui::views
{
    class CAtcStationView;
    class CSimulatedAircraftView;
} // namespace swift::gui::views

namespace swift::gui::components
{
    /*!
     * Zeigt alle verbundenen Clients (ATC + Piloten) nebeneinander an:
     * ein Drittel links fuer ATC-Clients, zwei Drittel rechts fuer Piloten-Clients.
     * Komplett ohne Abhaengigkeit zu CDockWidgetInfoArea / CInfoArea,
     * analog zu CMiniAtcStationWidget / CMiniRadarWidget.
     * \remark benoetigt sGui mit gestartetem IContextNetwork
     */
    class SWIFT_GUI_EXPORT CMiniClientsWidget : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CMiniClientsWidget(QWidget *parent = nullptr);

        //! Destructor
        ~CMiniClientsWidget() override;

    public slots:
        //! Pull current stations/aircraft from context and refresh the views
        void update();

    private:
        //! ATC station list has changed (digest signal from context)
        void onChangedAtcStationsOnline();

        //! Aircraft (pilot clients) list has changed (digest signal from context)
        void onChangedAircraftInRange();

        //! Connection status has changed
        void onConnectionStatusChanged(const swift::misc::network::CConnectionStatus &from,
                                       const swift::misc::network::CConnectionStatus &to);

        //! Sortiert die ATC-Stationen gemaess aktueller Einstellung (Entfernung oder Kategorie)
        swift::misc::aviation::CAtcStationList sortAtcStations(swift::misc::aviation::CAtcStationList stations) const;

        swift::gui::views::CAtcStationView *m_atcView = nullptr;
        swift::gui::views::CSimulatedAircraftView *m_aircraftView = nullptr;
        swift::misc::CSetting<swift::gui::settings::TMiniAtcSortMode> m_atcSortSetting { this };
    };
} // namespace swift::gui::components

#endif // SWIFT_GUI_COMPONENTS_MINICLIENTSWIDGET_H