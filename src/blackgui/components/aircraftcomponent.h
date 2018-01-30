/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_AIRCRAFTCOMPONENT_H
#define BLACKGUI_AIRCRAFTCOMPONENT_H

#include "blackcore/network.h"
#include "blackgui/blackguiexport.h"
#include "blackgui/settings/viewupdatesettings.h"
#include "blackgui/components/enablefordockwidgetinfoarea.h"

#include <QObject>
#include <QScopedPointer>
#include <QTabWidget>
#include <QtGlobal>
#include <QTimer>

class QWidget;

namespace BlackMisc
{
    namespace Aviation { class CCallsign; }
    namespace Simulation { class CSimulatedAircraft; }
}
namespace Ui { class CAircraftComponent; }
namespace BlackGui
{
    class CDockWidgetInfoArea;
    namespace Components
    {
        //! Aircraft widget
        class BLACKGUI_EXPORT CAircraftComponent :
            public QTabWidget,
            public CEnableForDockWidgetInfoArea
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CAircraftComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CAircraftComponent();

            //! Aircraft in range
            int countAircraftInView() const;

            //! Airports in range
            int countAirportsInRangeInView() const;

            //! \copydoc CEnableForDockWidgetInfoArea::setParentDockWidgetInfoArea
            virtual bool setParentDockWidgetInfoArea(BlackGui::CDockWidgetInfoArea *parentDockableWidget) override;

        signals:
            //! Request a text message
            void requestTextMessageWidget(const BlackMisc::Aviation::CCallsign &callsign);

        public slots:
            //! Update aircraft/airport view
            void update();

        private slots:
            //! Info area tab bar has changed
            void ps_infoAreaTabBarChanged(int index);

            //! Number of elements changed
            void ps_onRowCountChanged(int count, bool withFilter);

            //! Connection status has been changed
            void ps_connectionStatusChanged(BlackCore::INetwork::ConnectionStatus from, BlackCore::INetwork::ConnectionStatus to);

            //! Highlight in simulator
            void ps_onMenuHighlightInSimulator(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

            //! Settings have been changed
            void ps_settingsChanged();

        private:
            QScopedPointer<Ui::CAircraftComponent> ui;
            BlackMisc::CSettingReadOnly<BlackGui::Settings::TViewUpdateSettings> m_settings { this, &CAircraftComponent::ps_settingsChanged }; //!< settings changed
            QTimer m_updateTimer;
        };
    } // ns
} // ns

#endif // guard
