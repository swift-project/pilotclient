// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_ATCSTATIONCOMPONENT_H
#define BLACKGUI_COMPONENTS_ATCSTATIONCOMPONENT_H

#include "blackgui/settings/viewupdatesettings.h"
#include "blackgui/settings/atcstationssettings.h"
#include "blackgui/overlaymessagesframe.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/aviation/atcstation.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/pq/frequency.h"
#include "blackmisc/identifiable.h"
#include "blackmisc/network/connectionstatus.h"

#include <QDateTime>
#include <QModelIndex>
#include <QObject>
#include <QScopedPointer>
#include <QString>
#include <QTabWidget>
#include <QFrame>
#include <QtGlobal>
#include <QVBoxLayout>
#include <QTimer>
#include <QList>

namespace Ui
{
    class CAtcStationComponent;
}
namespace BlackMisc::Aviation
{
    class CCallsign;
}
namespace BlackGui
{
    class CDockWidgetInfoArea;

    namespace Components
    {
        //! ATC stations component
        class BLACKGUI_EXPORT CAtcStationComponent :
            public COverlayMessagesFrameEnableForDockWidgetInfoArea,
            public BlackMisc::CIdentifiable
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
            virtual ~CAtcStationComponent() override;

            //! Set tab
            void setTab(AtcTab tab);

            //! Number of online stations
            int countOnlineStations() const;

            //! Update stations
            void update();

            //! Get METAR for given ICAO airport code
            void getMetar(const QString &airportIcaoCode);

            //! \copydoc CEnableForDockWidgetInfoArea::setParentDockWidgetInfoArea
            virtual bool setParentDockWidgetInfoArea(BlackGui::CDockWidgetInfoArea *parentDockableWidget) override;

        signals:
            //! Request audio widget
            void requestAudioWidget();

        private:
            //! Set timestampd and call update
            void forceUpdate();

            //! ATC station disconnected
            void atcStationDisconnected(const BlackMisc::Aviation::CAtcStation &station);

            //! Get all METARs
            void getMetarAsEntered();

            //! Request new ATIS
            void requestAtisUpdates();

            //! A station has been selected
            void onOnlineAtcStationSelected(const BlackMisc::Aviation::CAtcStation &station);

            //! Online ATC station selected
            void onOnlineAtcStationVariantSelected(const BlackMisc::CVariant &object);

            //! Trigger a selection of an onlie station (async)
            void triggerOnlineAtcStationSelected(const BlackMisc::Aviation::CAtcStation &station);

            //! Online stations changed
            void changedAtcStationsOnline();

            //! Connection status has been changed
            void connectionStatusChanged(const BlackMisc::Network::CConnectionStatus &from, const BlackMisc::Network::CConnectionStatus &to);

            //! Request dummy ATC online stations
            void testCreateDummyOnlineAtcStations(int number);

            //! Request udpate
            void requestOnlineStationsUpdate();

            //! Info area tab bar has changed
            void infoAreaTabBarChanged(int index);

            //! Online tab info
            void setOnlineTabs(int count, int filtered);

            //! Set COM frequency
            void setComFrequency(const BlackMisc::PhysicalQuantities::CFrequency &frequency, BlackMisc::Aviation::CComSystem::ComUnit unit);

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
            BlackMisc::Aviation::CCallsign m_selectedCallsign;
            QDateTime m_timestampLastReadOnlineStations; //!< stations read
            QDateTime m_timestampOnlineStationsChanged; //!< stations marked as changed
            BlackMisc::CSettingReadOnly<BlackGui::Settings::TViewUpdateSettings> m_settingsView { this, &CAtcStationComponent::settingsChanged };
        };
    } // namespace
} // namespace
#endif // guard
