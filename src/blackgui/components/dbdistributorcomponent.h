/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_DBDISTRIBUTORCOMPONENT_H
#define BLACKGUI_COMPONENTS_DBDISTRIBUTORCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "blackgui/components/enablefordockwidgetinfoarea.h"
#include "blackgui/enableforviewbasedindicator.h"
#include "blackmisc/simulation/distributorlist.h"
#include "blackmisc/network/entityflags.h"
#include <QFrame>
#include <QObject>
#include <QScopedPointer>

namespace Ui { class CDbDistributorComponent; }
namespace BlackMisc { namespace Simulation { class CSimulatorInfo; }}
namespace BlackGui
{
    namespace Views { class CDistributorView; }
    namespace Components
    {
        /**
         * Distributors
         */
        class BLACKGUI_EXPORT CDbDistributorComponent :
            public QFrame,
            public CEnableForDockWidgetInfoArea,
            public BlackGui::CEnableForViewBasedIndicator
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CDbDistributorComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CDbDistributorComponent();

            //! Get the view
            BlackGui::Views::CDistributorView *view() const;

            //! Filter by simulator
            void filterBySimulator(const BlackMisc::Simulation::CSimulatorInfo &simulator);

            //! The selected distributors
            BlackMisc::Simulation::CDistributorList getSelectedDistributors() const;

            //! Having selected distributors
            bool hasSelectedDistributors() const;

        private:
            QScopedPointer<Ui::CDbDistributorComponent> ui;

            //! Distributors have been read
            void onDistributorsRead(BlackMisc::Network::CEntityFlags::Entity entity, BlackMisc::Network::CEntityFlags::ReadState readState, int count);

            //! Select all FSX standard models
            void selectStandardModels();

            //! Reload models
            void reload();
        };
    } // ns
} // ns

#endif // guard
