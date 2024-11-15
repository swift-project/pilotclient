// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_MODELS_DISTRIBUTORLISTMODEL_H
#define SWIFT_GUI_MODELS_DISTRIBUTORLISTMODEL_H

#include "gui/swiftguiexport.h"
#include "gui/models/listmodeldbobjects.h"
#include "misc/simulation/distributorlist.h"

#include <QString>

namespace swift::misc::simulation
{
    class CDistributor;
}
namespace swift::gui::models
{
    //! Distributor list model
    class SWIFT_GUI_EXPORT CDistributorListModel :
        public COrderableListModelDbObjects<swift::misc::simulation::CDistributorList, QString, true>
    {
        Q_OBJECT

    public:
        //! What kind of stations
        enum DistributorMode
        {
            NotSet,
            Normal,
            NormalWithOrder,
            Minimal,
            MinimalWithOrder
        };

        //! Constructor
        explicit CDistributorListModel(QObject *parent = nullptr);

        //! Destructor
        virtual ~CDistributorListModel() override {}

        //! Set mode
        void setDistributorMode(DistributorMode distributorMode);

        //! Mode
        DistributorMode getDistributorMode() const { return this->m_distributorMode; }

        //! \copydoc swift::gui::models::CListModelBaseNonTemplate::isOrderable
        virtual bool isOrderable() const override { return true; }

    private:
        DistributorMode m_distributorMode = NotSet;
    };
} // ns

#endif // guard