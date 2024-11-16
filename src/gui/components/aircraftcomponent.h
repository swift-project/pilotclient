// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_AIRCRAFTCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_AIRCRAFTCOMPONENT_H

#include <QObject>
#include <QScopedPointer>
#include <QTabWidget>
#include <QTimer>
#include <QtGlobal>

#include "gui/enablefordockwidgetinfoarea.h"
#include "gui/settings/viewupdatesettings.h"
#include "gui/swiftguiexport.h"
#include "misc/network/connectionstatus.h"

namespace Ui
{
    class CAircraftComponent;
}
namespace swift::misc
{
    namespace aviation
    {
        class CCallsign;
    }
    namespace simulation
    {
        class CSimulatedAircraft;
    }
} // namespace swift::misc
namespace swift::gui
{
    class CDockWidgetInfoArea;

    namespace components
    {
        //! Aircraft widget
        class SWIFT_GUI_EXPORT CAircraftComponent :
            public QTabWidget,
            public CEnableForDockWidgetInfoArea
        {
            Q_OBJECT

        public:
            //! Tab
            //! \remark needs to be in sync with tab order
            enum AircraftTab
            {
                TabAircraftInRange = 0,
                TabAirportsInRange
            };

            //! Constructor
            explicit CAircraftComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CAircraftComponent() override;

            //! Aircraft in range
            int countAircraftInView() const;

            //! Airports in range
            int countAirportsInRangeInView() const;

            //! \copydoc CEnableForDockWidgetInfoArea::setParentDockWidgetInfoArea
            virtual bool setParentDockWidgetInfoArea(swift::gui::CDockWidgetInfoArea *parentDockableWidget) override;

            //! Update aircraft/airport view
            void update();

            //! Set tab
            void setTab(AircraftTab tab);

        signals:
            //! Request a text message
            void requestTextMessageWidget(const swift::misc::aviation::CCallsign &callsign);

        private:
            //! Update the views
            void updateViews();

            //! Info area tab bar has changed
            void onInfoAreaTabBarChanged(int index);

            //! Number of elements changed
            void onRowCountChanged(int count, bool withFilter);

            //! Connection status has been changed
            void onConnectionStatusChanged(const swift::misc::network::CConnectionStatus &from, const swift::misc::network::CConnectionStatus &to);

            //! Settings have been changed
            void onSettingsChanged();

            //! Own aircraft has been moved
            void onOwnAircraftMoved(const swift::misc::physical_quantities::CLength &distance);

            QScopedPointer<Ui::CAircraftComponent> ui;
            swift::misc::CSettingReadOnly<swift::gui::settings::TViewUpdateSettings> m_settings { this, &CAircraftComponent::onSettingsChanged }; //!< settings changed
            QTimer m_updateTimer;
            int m_updateCounter = 0;
        };
    } // namespace components
} // namespace swift::gui

#endif // guard
