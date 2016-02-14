/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_REMOTEAIRCRAFTSELECTOR_H
#define BLACKGUI_REMOTEAIRCRAFTSELECTOR_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"


#include <QFrame>
#include <QScopedPointer>

namespace Ui { class CRemoteAircraftSelector; }

namespace BlackGui
{
    namespace Components
    {
        //! Select a remote aircraft
        class BLACKGUI_EXPORT CRemoteAircraftSelector : public QFrame
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CRemoteAircraftSelector(QWidget *parent = nullptr);

            //! Destructor
            ~CRemoteAircraftSelector();

            //! Selected callsign
            BlackMisc::Aviation::CCallsign getSelectedCallsign() const;

        protected:
            //! \copydoc QWidget::showEvent
            virtual void showEvent(QShowEvent *event) override;

        private slots:
            //! Change content of combobox
            void ps_onAddedAircraft(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

            //! IContextNetwork::removedAircraft
            void ps_onRemovedAircraft(const BlackMisc::Aviation::CCallsign &callsign);

        private:
            QScopedPointer<Ui::CRemoteAircraftSelector> ui;
            BlackMisc::Simulation::CSimulatedAircraftList m_aircraft;

            //! Set combobox items
            void fillComboBox();
        };

    } // namespace
} // namespace

#endif // guard
