/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_FILTERS_FILTERBARAIRCRAFTMODEL_H
#define BLACKGUI_FILTERS_FILTERBARAIRCRAFTMODEL_H

#include "blackguiexport.h"
#include "blackgui/models/aircraftmodelfilter.h"
#include "blackgui/filters/filterwidget.h"
#include "blackmisc/network/webdataservicesprovider.h"
#include <QScopedPointer>

namespace Ui { class CAircraftModelFilterBar; }

namespace BlackGui
{
    namespace Filters
    {
        /*!
         * Filter bar for aircraft models
         */
        class BLACKGUI_EXPORT CAircraftModelFilterBar :
            public CFilterWidget,
            public BlackGui::Models::IModelFilterProvider<BlackMisc::Simulation::CAircraftModelList>,
            public BlackMisc::Network::CWebDataServicesAware
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CAircraftModelFilterBar(QWidget *parent = nullptr);

            //! Destructor
            ~CAircraftModelFilterBar();

            //! \copydoc Models::IModelFilterProvider::createModelFilter
            virtual std::unique_ptr<BlackGui::Models::IModelFilter<BlackMisc::Simulation::CAircraftModelList>> createModelFilter() const override;

            //! Set the provider
            virtual void setProvider(BlackMisc::Network::IWebDataServicesProvider *webDataReaderProvider) override;

        public slots:
            //! \copydoc CFilterWidget::onRowCountChanged
            virtual void onRowCountChanged(int count, bool withFilter) override;

        protected:
            //! Clear form
            virtual void clearForm() override;

        private slots:
            //! Simulator selection changed
            void ps_simulatorSelectionChanged(const BlackMisc::Simulation::CSimulatorInfo &info);

            //! Distributor changed
            void ps_distributorChanged(const BlackMisc::Simulation::CDistributor &distributor);

        private:
            QScopedPointer<Ui::CAircraftModelFilterBar> ui;
        };
    } // ns
} // ns

#endif // guard
