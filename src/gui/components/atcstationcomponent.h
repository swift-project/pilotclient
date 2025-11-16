// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_ATCSTATIONCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_ATCSTATIONCOMPONENT_H

#include <QDateTime>
#include <QFrame>
#include <QList>
#include <QModelIndex>
#include <QObject>
#include <QScopedPointer>
#include <QString>
#include <QTabWidget>
#include <QTimer>
#include <QVBoxLayout>
#include <QtGlobal>

#include "gui/overlaymessagesframe.h"
#include "gui/settings/viewupdatesettings.h"
#include "gui/swiftguiexport.h"
#include "misc/aviation/atcstation.h"
#include "misc/aviation/comsystem.h"
#include "misc/identifiable.h"
#include "misc/network/connectionstatus.h"
#include "misc/pq/frequency.h"

namespace Ui
{
    class CAtcStationComponent;
}
namespace swift::misc::aviation
{
    class CCallsign;
}
namespace swift::gui
{
    class CDockWidgetInfoArea;

    namespace components
    {
        //! ATC stations component
        class SWIFT_GUI_EXPORT CAtcStationComponent :
            public COverlayMessagesFrameEnableForDockWidgetInfoArea,
            public swift::misc::CIdentifiable
        {
            Q_OBJECT

        public:
            //! Tab
            //! \remark needs to be in sync with tab order
            enum AtcTab
            {
                TabAtcOnline = 0,
                TabAtcOnlineTree,
            };

            //! Constructor
            explicit CAtcStationComponent(QWidget *parent = nullptr);

            //! Destructor
            ~CAtcStationComponent() override;

            //! Set tab
            void setTab(AtcTab tab);

            //! Number of online stations
            int countOnlineStations() const;

            //! Update stations
            void update();

            //! Get METAR for given ICAO airport code
            void getMetar(const QString &airportIcaoCode);

            //! \copydoc CEnableForDockWidgetInfoArea::setParentDockWidgetInfoArea
            bool setParentDockWidgetInfoArea(swift::gui::CDockWidgetInfoArea *parentDockableWidget) override;

        signals:
            //! Request audio widget
            void requestAudioWidget();

        private:
            //! ATC station disconnected
            void atcStationDisconnected(const swift::misc::aviation::CAtcStation &station);

            //! Get all METARs
            void getMetarAsEntered();

            //! Request new ATIS
            void requestAtisUpdates();

            //! A station has been selected
            void onOnlineAtcStationSelected(const swift::misc::aviation::CAtcStation &station);

            //! Online ATC station selected
            void onOnlineAtcStationVariantSelected(const swift::misc::CVariant &object);

            //! Trigger a selection of an onlie station (async)
            void triggerOnlineAtcStationSelected(const swift::misc::aviation::CAtcStation &station);

            //! Online stations changed
            void changedAtcStationsOnline();

            //! Connection status has been changed
            void connectionStatusChanged(const swift::misc::network::CConnectionStatus &from,
                                         const swift::misc::network::CConnectionStatus &to);

            //! Request dummy ATC online stations
            void testCreateDummyOnlineAtcStations(int number);

            //! Request udpate
            void requestOnlineStationsUpdate();

            //! Info area tab bar has changed
            void infoAreaTabBarChanged(int index);

            //! Online tab info
            void setOnlineTabs(int count, int filtered);

            //! Set COM frequency
            void setComFrequency(const swift::misc::physical_quantities::CFrequency &frequency,
                                 swift::misc::aviation::CComSystem::ComUnit unit, bool active);

            //! Airports read from web readers
            void airportsRead();

            //! Build a tree view for ATC stations
            void updateTreeView();

            //! Init the completers
            void initCompleters();

            //! Settings have been changed
            void settingsChanged();

            //! Contexts?
            bool canAccessContext() const;

            //! Clear the online views
            void clearOnlineViews();

            //! Inline message
            void showOverlayInlineTextMessage();

            //! Details toggled
            void onDetailsToggled(bool checked);

            //! Get the vertical layout
            QVBoxLayout *vLayout() const;

            QScopedPointer<Ui::CAtcStationComponent> ui;
            QTimer m_updateTimer;
            QList<int> m_splitterSizes;
            swift::misc::aviation::CCallsign m_selectedCallsign;
            QDateTime m_timestampLastReadOnlineStations; //!< stations read
            QDateTime m_timestampOnlineStationsChanged; //!< stations marked as changed
            swift::misc::CSettingReadOnly<swift::gui::settings::TViewUpdateSettings> m_settingsView {
                this, &CAtcStationComponent::settingsChanged
            };
        };
    } // namespace components
} // namespace swift::gui
#endif // SWIFT_GUI_COMPONENTS_ATCSTATIONCOMPONENT_H
