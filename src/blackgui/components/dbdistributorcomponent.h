// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_DBDISTRIBUTORCOMPONENT_H
#define BLACKGUI_COMPONENTS_DBDISTRIBUTORCOMPONENT_H

#include "blackgui/enablefordockwidgetinfoarea.h"
#include "blackgui/enableforviewbasedindicator.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/simulation/distributorlist.h"
#include "blackmisc/network/entityflags.h"
#include <QFrame>
#include <QObject>
#include <QScopedPointer>

namespace Ui
{
    class CDbDistributorComponent;
}
namespace BlackMisc::Simulation
{
    class CSimulatorInfo;
}
namespace BlackGui
{
    namespace Views
    {
        class CDistributorView;
    }
    namespace Components
    {
        /*!
         * Distributors
         */
        class BLACKGUI_EXPORT CDbDistributorComponent :
            public QFrame,
            public CEnableForDockWidgetInfoArea,
            public CEnableForViewBasedIndicator
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CDbDistributorComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CDbDistributorComponent() override;

            //! Get the view
            BlackGui::Views::CDistributorView *view() const;

            //! Filter by simulator
            void filterBySimulator(const BlackMisc::Simulation::CSimulatorInfo &simulator);

            //! The selected distributors
            BlackMisc::Simulation::CDistributorList getSelectedDistributors() const;

            //! Select the given distributor
            bool selectDistributor(const BlackMisc::Simulation::CDistributor &distributor);

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
