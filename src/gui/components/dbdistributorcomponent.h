// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_DBDISTRIBUTORCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_DBDISTRIBUTORCOMPONENT_H

#include <QFrame>
#include <QObject>
#include <QScopedPointer>

#include "gui/enablefordockwidgetinfoarea.h"
#include "gui/enableforviewbasedindicator.h"
#include "gui/swiftguiexport.h"
#include "misc/network/entityflags.h"
#include "misc/simulation/distributorlist.h"

namespace Ui
{
    class CDbDistributorComponent;
}
namespace swift::misc::simulation
{
    class CSimulatorInfo;
}
namespace swift::gui
{
    namespace views
    {
        class CDistributorView;
    }
    namespace components
    {
        /*!
         * Distributors
         */
        class SWIFT_GUI_EXPORT CDbDistributorComponent :
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
            swift::gui::views::CDistributorView *view() const;

            //! Filter by simulator
            void filterBySimulator(const swift::misc::simulation::CSimulatorInfo &simulator);

            //! The selected distributors
            swift::misc::simulation::CDistributorList getSelectedDistributors() const;

            //! Select the given distributor
            bool selectDistributor(const swift::misc::simulation::CDistributor &distributor);

            //! Having selected distributors
            bool hasSelectedDistributors() const;

        private:
            QScopedPointer<Ui::CDbDistributorComponent> ui;

            //! Distributors have been read
            void onDistributorsRead(swift::misc::network::CEntityFlags::Entity entity, swift::misc::network::CEntityFlags::ReadState readState, int count);

            //! Select all FSX standard models
            void selectStandardModels();

            //! Reload models
            void reload();
        };
    } // namespace components
} // namespace swift::gui

#endif // guard
