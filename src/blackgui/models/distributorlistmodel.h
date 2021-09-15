/* Copyright (C) 2015
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_MODELS_DISTRIBUTORLISTMODEL_H
#define BLACKGUI_MODELS_DISTRIBUTORLISTMODEL_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/listmodeldbobjects.h"
#include "blackmisc/simulation/distributorlist.h"

#include <QString>

namespace BlackMisc::Simulation { class CDistributor; }
namespace BlackGui::Models
{
    //! Distributor list model
    class BLACKGUI_EXPORT CDistributorListModel :
        public COrderableListModelDbObjects<BlackMisc::Simulation::CDistributorList, QString, true>
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

        //! \copydoc BlackGui::Models::CListModelBaseNonTemplate::isOrderable
        virtual bool isOrderable() const override { return true; }

    private:
        DistributorMode m_distributorMode = NotSet;
    };
} // ns

#endif // guard
